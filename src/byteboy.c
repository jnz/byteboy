/** @file byteboy.c
 * byteboy, Jan Zwiener (jan@zwiener.org)
 *
 * @brief Math functions
 */

 /******************************************************************************
  * SYSTEM INCLUDE FILES
  ******************************************************************************/

#include <assert.h>
#include <string.h> /* memset */

/******************************************************************************
 * PROJECT INCLUDE FILES
 ******************************************************************************/

#include "byteboy.h"

/******************************************************************************
 * DEFINES
 ******************************************************************************/

/******************************************************************************
 * TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 * LOCAL DATA DEFINITIONS
 ******************************************************************************/

/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 ******************************************************************************/

/******************************************************************************
 * FUNCTION BODIES
 ******************************************************************************/

static inline void bb_add(bb_state_t* bb, uint8_t a, uint8_t b)
{
    int result = a + b;
    bb->reg.a = (uint8_t)(result & 0xff);

    // ZERO FLAG
    if (result == 0)
    {
        bb->reg.f |= BB_ZERO_FLAG;
    }
    else
    {
        bb->reg.f &= ~BB_ZERO_FLAG;
    }
    // CARRY FLAG
    if (result > 0xFF)
    {
        bb->reg.f |= BB_CARRY_FLAG;
    }
    else
    {
        bb->reg.f &= ~BB_CARRY_FLAG;
    }
    // HALF-CARRY FLAG
    if ((bb->reg.a & 0xF) + (b & 0xF) > 0xF)
    {
        bb->reg.f |= BB_HALF_CARRY_FLAG;
    }
    else
    {
        bb->reg.f &= ~BB_HALF_CARRY_FLAG;
    }
}

static inline void bb_add_carry(bb_state_t* bb, uint8_t a, uint8_t b)
{
    bb_add(bb, a, b + ((bb->reg.f & BB_CARRY_FLAG) ? 1 : 0));
}

int bb_vm(bb_state_t* bb)
{
    uint8_t* code = bb->rom;
    uint8_t* ram = bb->ram;
    int run = 1;

    while (run)
    {
        switch (code[bb->pc++])
        {
        /* -- ADD -------------------------------------------- */
        case BB_OP_ADD_A_A:
            bb_add(bb, bb->reg.a, bb->reg.a);
            break;
        case BB_OP_ADD_A_B:
            bb_add(bb, bb->reg.a, bb->reg.b);
            break;
        case BB_OP_ADD_A_C:
            bb_add(bb, bb->reg.a, bb->reg.c);
            break;
        case BB_OP_ADD_A_D:
            bb_add(bb, bb->reg.a, bb->reg.d);
            break;
        case BB_OP_ADD_A_E:
            bb_add(bb, bb->reg.a, bb->reg.e);
            break;
        case BB_OP_ADD_A_H:
            bb_add(bb, bb->reg.a, bb->reg.h);
            break;
        case BB_OP_ADD_A_L:
            bb_add(bb, bb->reg.a, bb->reg.l);
            break;
        case BB_OP_ADD_A_HL:
            bb_add(bb, bb->reg.a, ram[((bb->reg.h << 8) | bb->reg.l) & BB_MEM_MASK]);
            break;
        case BB_OP_ADD_A_N:
            bb_add(bb, bb->reg.a, code[bb->pc++]);
            break;

        /* -- ADC -------------------------------------------- */
        case BB_OP_ADC_A_A:
            bb_add_carry(bb, bb->reg.a, bb->reg.a);
            break;
        case BB_OP_ADC_A_B:
            bb_add_carry(bb, bb->reg.a, bb->reg.b);
            break;
        case BB_OP_ADC_A_C:
            bb_add_carry(bb, bb->reg.a, bb->reg.c);
            break;
        case BB_OP_ADC_A_D:
            bb_add_carry(bb, bb->reg.a, bb->reg.d);
            break;
        case BB_OP_ADC_A_E:
            bb_add_carry(bb, bb->reg.a, bb->reg.e);
            break;
        case BB_OP_ADC_A_H:
            bb_add_carry(bb, bb->reg.a, bb->reg.h);
            break;
        case BB_OP_ADC_A_L:
            bb_add_carry(bb, bb->reg.a, bb->reg.l);
            break;
        case BB_OP_ADC_A_HL:
            bb_add_carry(bb, bb->reg.a, ram[((bb->reg.h << 8) | bb->reg.l) & BB_MEM_MASK]);
            break;
        case BB_OP_ADC_A_N:
            bb_add_carry(bb, bb->reg.a, code[bb->pc++]);
            break;

        case BB_OP_HALT:
            run = 0;
            break;
        default:
            assert(0);
        };
    }
}

int main(int argc, char** argv)
{
    bb_state_t bb = { 0 };
    uint8_t code[128] = { BB_OP_ADC_A_HL, BB_OP_HALT };
    uint8_t ram[128];

    bb.rom = code;
    bb.ram = ram;

    bb.reg.a = 0x10;
    ram[0x30] = 0x20;
    bb.reg.f |= BB_CARRY_FLAG;
    bb.reg.l = 0x30;

    bb_vm(&bb);

    return 0;
}
