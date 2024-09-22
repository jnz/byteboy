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

#define BB_MEM_MASK            0xFFFF

/******************************************************************************
 * TYPEDEFS
 ******************************************************************************/

/* Source: https://rgbds.gbdev.io/docs/v0.8.0/gbz80.7 */

typedef enum {
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

    BB_OP_HALT      = 0x76,

} bb_op_code_t;

typedef enum {
    BB_CARRY_FLAG      = 0x10,
    BB_HALF_CARRY_FLAG = 0x20,
    BB_SUBTRACT_FLAG   = 0x40,
    BB_ZERO_FLAG       = 0x80,
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
            uint8_t a, f, b, c, d, e, h, l;
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

