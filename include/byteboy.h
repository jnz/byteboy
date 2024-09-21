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

#define BB_MAX_ROM_SIZE       0xFFFF /**< max. number of bytes for ASM
                                           instructions */

#define BB_MAX_RAM_SIZE        0xFFFF

#define BB_MEM_MASK            0xFFFF


/******************************************************************************
 * TYPEDEFS
 ******************************************************************************/

typedef enum {
    BB_OP_ADD_A_A   = 0x87, // 135
    BB_OP_ADD_A_B   = 0x80, // 128
	BB_OP_ADD_A_C   = 0x81, // 129
	BB_OP_ADD_A_D   = 0x82, // 130
	BB_OP_ADD_A_E   = 0x83, // 131
	BB_OP_ADD_A_H   = 0x84, // 132
	BB_OP_ADD_A_L   = 0x85, // 133
	BB_OP_ADD_A_HL  = 0x86, // 134
	BB_OP_ADD_A_N   = 0xC6, // 198
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
        struct
        {
            uint16_t af, bc, de, hl;
        };
        uint8_t arr[8];
    };
} bb_reg_t;

typedef struct bb_state_s
{
    bb_reg_t reg;
    uint8_t rom[BB_MAX_ROM_SIZE];
    uint8_t ram[BB_MAX_RAM_SIZE];
    int pc; /**< program counter */
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

