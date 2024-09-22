/*
 *   _____ __ __ _____ _____ _____ _____ __ __
 *  | __  |  |  |_   _|   __| __  |     |  |  |
 *  | __ -|_   _| | | |   __| __ -|  |  |_   _|
 *  |_____| |_|   |_| |_____|_____|_____| |_|
 *
 *   Simple Game Boy Emulator, Jan Zwiener (jan@zwiener.org)
 */

/******************************************************************************
 * SYSTEM INCLUDE FILES
 ******************************************************************************/

#include <stdint.h>

/******************************************************************************
 * PROJECT INCLUDE FILES
 ******************************************************************************/

/******************************************************************************
 * DEFINES
 ******************************************************************************/

#define BB_RAM_MASK            0xFFFF
#define BB_ROM_MASK            0xFFFF

/******************************************************************************
 * TYPEDEFS
 ******************************************************************************/

/* Source: https://rgbds.gbdev.io/docs/v0.8.0/gbz80.7 */
/* https://gbdev.io/gb-opcodes/optables/ */
/* https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html */

typedef enum {
    BB_OP_PREFIX    = 0xCB,

    BB_OP_ADD_A_B   = 0x80, // 128
    BB_OP_ADD_A_C   = 0x81, // 129
    BB_OP_ADD_A_D   = 0x82, // 130
    BB_OP_ADD_A_E   = 0x83, // 131
    BB_OP_ADD_A_H   = 0x84, // 132
    BB_OP_ADD_A_L   = 0x85, // 133
    BB_OP_ADD_A_HL  = 0x86, // 134
    BB_OP_ADD_A_A   = 0x87, // 135
    BB_OP_ADD_A_N   = 0xC6, // 198

    BB_OP_ADC_A_B   = 0x88,
    BB_OP_ADC_A_C   = 0x89,
    BB_OP_ADC_A_D   = 0x8A,
    BB_OP_ADC_A_E   = 0x8B,
    BB_OP_ADC_A_H   = 0x8C,
    BB_OP_ADC_A_L   = 0x8D,
    BB_OP_ADC_A_HL  = 0x8E,
    BB_OP_ADC_A_A   = 0x8F,
    BB_OP_ADC_A_N   = 0xCE,

    BB_OP_AND_A_B   = 0xA0,
    BB_OP_AND_A_C   = 0xA1,
    BB_OP_AND_A_D   = 0xA2,
    BB_OP_AND_A_E   = 0xA3,
    BB_OP_AND_A_H   = 0xA4,
    BB_OP_AND_A_L   = 0xA5,
    BB_OP_AND_A_HL  = 0xA6,
    BB_OP_AND_A_A   = 0xA7,
    BB_OP_AND_A_N   = 0xE6,

    BB_OP_JP_NZ     = 0xC2,
    BB_OP_JP        = 0xC3,
    BB_OP_JP_Z      = 0xCA,
    BB_OP_JP_NC     = 0xD2, // JUMP NOT CARRY
    BB_OP_JP_C      = 0xDA, // JUMP CARRY
    BB_OP_JP_HL     = 0xE9,

    BB_OP_CALL_NZ   = 0xC4,
    BB_OP_CALL_Z    = 0xCC,
    BB_OP_CALL      = 0xCD,
    BB_OP_CALL_NC   = 0xD4,
    BB_OP_CALL_C    = 0xDC,

    BB_OP_CCF       = 0x3F, // COMPLEMENT CARRY

    BB_OP_CP_A_B    = 0xB8,
    BB_OP_CP_A_C    = 0xB9,
    BB_OP_CP_A_D    = 0xBA,
    BB_OP_CP_A_E    = 0xBB,
    BB_OP_CP_A_H    = 0xBC,
    BB_OP_CP_A_L    = 0xBD,
    BB_OP_CP_A_HL   = 0xBE,
    BB_OP_CP_A_A    = 0xBF,
    BB_OP_CP_A_N    = 0xFE,

    BB_OP_CPL       = 0x2F,
    BB_OP_DAA       = 0x27,

    BB_OP_INC_BC    = 0x03,
    BB_OP_INC_B     = 0x04,
    BB_OP_INC_C     = 0x0C,
    BB_OP_INC_DE    = 0x13,
    BB_OP_INC_D     = 0x14,
    BB_OP_INC_E     = 0x1C,
    BB_OP_INC_HL    = 0x23,
    BB_OP_INC_H     = 0x24,
    BB_OP_INC_L     = 0x2C,
    BB_OP_INC_SP    = 0x33,
    BB_OP_INC_HL_P  = 0x34, // increment mem HL is pointing at
    BB_OP_INC_A     = 0x3C,

    BB_LD_BC        = 0x01,
    BB_LD_BC_A      = 0x02, // Store value in register A into the byte pointed to by register BC.
    BB_LD_B         = 0x06,
    BB_LD_N16_SP    = 0x08, // LD (a16),SP, store value of SP into mem location specified by next two bytes
    BB_LD_A_BC      = 0x0A,

    BB_LD_H_N       = 0x26, // LD H, n8

    BB_OP_HALT      = 0x76,

} bb_op_code_t;

typedef enum {
    BB_FLAG_CARRY      = 0x10,
    BB_FLAG_HALF_CARRY = 0x20,
    BB_FLAG_SUBTRACT   = 0x40,
    BB_FLAG_ZERO       = 0x80,
} bb_flags_t;

typedef struct bb_reg_s
{
    union {
        struct
        {
            /* f: flags register. lower 4 bits are always zero.
             *    bit 7: zero.
             *    bit 6: subtraction.
             *    bit 5: half carry
             *    bit 4: carry */
            uint8_t b, c, d, e, h, l, a, f; /* don't change the order */
        };
        uint8_t arr[8];
    };
} bb_reg_t;

typedef struct bb_state_s
{
    bb_reg_t reg;
    uint8_t* rom;
    uint8_t* ram;
    int pc; /**< program counter */
    int sp; /**< stack pointer */
} bb_state_t;

/******************************************************************************
 * LOCAL DATA DEFINITIONS
 ******************************************************************************/

/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 ******************************************************************************/

/******************************************************************************
 * FUNCTION PROTOTYPES
 ******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef __cplusplus
}
#endif

/* @} */

