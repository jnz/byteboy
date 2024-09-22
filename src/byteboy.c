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

static inline void bb_add(bb_state_t* bb, int a, int b)
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
    if (result > 0xFF)
    {
        bb->reg.f |= BB_CARRY_FLAG;
    }
    else
    {
        bb->reg.f &= ~BB_CARRY_FLAG;
    }
    if ((bb->reg.a & 0xF) + (b & 0xF) > 0xF)
    {
        bb->reg.f |= BB_HALF_CARRY_FLAG;
    }
    else
    {
        bb->reg.f &= ~BB_HALF_CARRY_FLAG;
    }
    bb->reg.f &= ~BB_SUBTRACT_FLAG;
}

static inline void bb_add_carry(bb_state_t* bb, int a, int b)
{
    bb_add(bb, a, b + ((bb->reg.f & BB_CARRY_FLAG) ? 1 : 0));
}

static inline void bb_and(bb_state_t* bb, uint8_t a, uint8_t b)
{
    int result = a & b;
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
    bb->reg.f &= ~BB_CARRY_FLAG;
    bb->reg.f |=  BB_HALF_CARRY_FLAG; // always set to 1 for some reason
    bb->reg.f &= ~BB_SUBTRACT_FLAG;
}

static inline void bb_daa(bb_state_t* bb)
{
    uint8_t correction = 0;

    // Check if the previous operation was a subtraction
    if (!(bb->reg.f & BB_SUBTRACT_FLAG)) // BB_SUBTRACT_FLAG == 0 means addition
    {
        if ((bb->reg.f & BB_CARRY_FLAG) || bb->reg.a > 0x99)
        {
            correction |= 0x60;
            bb->reg.f |= BB_CARRY_FLAG;
        }
        if ((bb->reg.f & BB_HALF_CARRY_FLAG) || (bb->reg.a & 0x0F) > 0x09)
        {
            correction |= 0x06;
        }
    }
    else
    {
        if (bb->reg.f & BB_CARRY_FLAG)
        {
            correction |= 0x60;
        }
        if (bb->reg.f & BB_HALF_CARRY_FLAG)
        {
            correction |= 0x06;
        }
    }

    // Apply the correction
    bb->reg.a += (bb->reg.f & BB_SUBTRACT_FLAG) ? -correction : correction;

    // Set Zero flag if A is zero
    bb->reg.f = (bb->reg.a == 0) ? (bb->reg.f | BB_ZERO_FLAG) : (bb->reg.f & ~BB_ZERO_FLAG);

    bb->reg.f &= ~BB_HALF_CARRY_FLAG; // Half carry flag should be cleared
}

void bb_vm(bb_state_t* bb)
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
            bb_add(bb, bb->reg.a, ram[((bb->reg.h << 8) | bb->reg.l) & BB_RAM_MASK]);
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
            bb_add_carry(bb, bb->reg.a, ram[((bb->reg.h << 8) | bb->reg.l) & BB_RAM_MASK]);
            break;
        case BB_OP_ADC_A_N:
            bb_add_carry(bb, bb->reg.a, code[bb->pc++]);
            break;

        /* -- AND -------------------------------------------- */
        case BB_OP_AND_A_A:
            bb_and(bb, bb->reg.a, bb->reg.a);
            break;
        case BB_OP_AND_A_B:
            bb_and(bb, bb->reg.a, bb->reg.b);
            break;
        case BB_OP_AND_A_C:
            bb_and(bb, bb->reg.a, bb->reg.c);
            break;
        case BB_OP_AND_A_D:
            bb_and(bb, bb->reg.a, bb->reg.d);
            break;
        case BB_OP_AND_A_E:
            bb_and(bb, bb->reg.a, bb->reg.e);
            break;
        case BB_OP_AND_A_H:
            bb_and(bb, bb->reg.a, bb->reg.h);
            break;
        case BB_OP_AND_A_L:
            bb_and(bb, bb->reg.a, bb->reg.l);
            break;
        case BB_OP_AND_A_HL:
            bb_and(bb, bb->reg.a, ram[((bb->reg.h << 8) | bb->reg.l) & BB_RAM_MASK]);
            break;
        case BB_OP_AND_A_N:
            bb_and(bb, bb->reg.a, code[bb->pc++]);
            break;

        /* -- JUMP ------------------------------------------- */
        case BB_OP_CALL_NZ:
            if (!(bb->reg.f & BB_ZERO_FLAG))
            {
                ram[--bb->sp] = bb->pc & 0xff;
                ram[--bb->sp] = (bb->pc >> 8) & 0xff;
                bb->pc = ((code[bb->pc+1] << 8) | code[bb->pc]) & BB_ROM_MASK;
            }
            break;
        case BB_OP_JP_NZ:
            if (!(bb->reg.f & BB_ZERO_FLAG))
            {
                bb->pc = ((code[bb->pc+1] << 8) | code[bb->pc]) & BB_ROM_MASK;
            }
            break;
        case BB_OP_CALL:
            ram[--bb->sp] = bb->pc & 0xff;
            ram[--bb->sp] = (bb->pc >> 8) & 0xff;
            bb->pc = ((code[bb->pc+1] << 8) | code[bb->pc]) & BB_ROM_MASK;
            break; /* could be done with fall-through here */
        case BB_OP_JP:
            bb->pc = ((code[bb->pc+1] << 8) | code[bb->pc]) & BB_ROM_MASK;
            break;
        case BB_OP_CALL_Z:
            if (bb->reg.f & BB_ZERO_FLAG)
            {
                ram[--bb->sp] = bb->pc & 0xff;
                ram[--bb->sp] = (bb->pc >> 8) & 0xff;
                bb->pc = ((code[bb->pc+1] << 8) | code[bb->pc]) & BB_ROM_MASK;
            }
            break;
        case BB_OP_JP_Z:
            if (bb->reg.f & BB_ZERO_FLAG)
            {
                bb->pc = ((code[bb->pc+1] << 8) | code[bb->pc]) & BB_ROM_MASK;
            }
            break;
        case BB_OP_CALL_NC:
            if (!(bb->reg.f & BB_CARRY_FLAG))
            {
                ram[--bb->sp] = bb->pc & 0xff;
                ram[--bb->sp] = (bb->pc >> 8) & 0xff;
                bb->pc = ((code[bb->pc+1] << 8) | code[bb->pc]) & BB_ROM_MASK;
            }
            break;
        case BB_OP_JP_NC:
            if (!(bb->reg.f & BB_CARRY_FLAG))
            {
                bb->pc = ((code[bb->pc+1] << 8) | code[bb->pc]) & BB_ROM_MASK;
            }
            break;
        case BB_OP_CALL_C:
            if (bb->reg.f & BB_CARRY_FLAG)
            {
                ram[--bb->sp] = bb->pc & 0xff;
                ram[--bb->sp] = (bb->pc >> 8) & 0xff;
                bb->pc = ((code[bb->pc+1] << 8) | code[bb->pc]) & BB_ROM_MASK;
            }
            break;
        case BB_OP_JP_C:
            if (bb->reg.f & BB_CARRY_FLAG)
            {
                bb->pc = ((code[bb->pc+1] << 8) | code[bb->pc]) & BB_ROM_MASK;
            }
            break;
        case BB_OP_JP_HL:
            bb->pc = ((bb->reg.h << 8) | bb->reg.l) & BB_RAM_MASK;
            break;

        /* -- CP --------------------------------------------- */
        case BB_OP_CP_A_B:
        case BB_OP_CP_A_C:
        case BB_OP_CP_A_D:
        case BB_OP_CP_A_E:
        case BB_OP_CP_A_H:
        case BB_OP_CP_A_L:
            {
                const uint8_t save = bb->reg.a;
                const int offset =  code[bb->pc-1] - BB_OP_CP_A_B;
                bb_add(bb, bb->reg.a, -bb->reg.arr[offset]);
                bb->reg.a = save;
            }
            break;
        case BB_OP_CP_A_HL:
            {
                const uint8_t save = bb->reg.a;
                bb_add(bb, bb->reg.a, -ram[((bb->reg.h << 8) | bb->reg.l) & BB_RAM_MASK]);
                bb->reg.a = save;
            }
            break;
        case BB_OP_CP_A_A:
            bb->reg.f = BB_ZERO_FLAG | BB_SUBTRACT_FLAG;
            break;
        case BB_OP_CP_A_N:
            {
                const uint8_t save = bb->reg.a;
                bb_add(bb, bb->reg.a, -code[bb->pc++]);
                bb->reg.a = save;
            }
            break;

        /* -- PREFIX ----------------------------------------- */
        case BB_OP_PREFIX:
            assert(0);
            break;

        /* -- OTHER ------------------------------------------ */
        case BB_OP_CCF: /* FLIP CARRY BIT */
            bb->reg.f ^= BB_CARRY_FLAG;
            break;
        case BB_OP_DAA:
            bb_daa(bb);
            break;
        case BB_OP_CPL:
            bb->reg.a = ~bb->reg.a;
            bb->reg.f |= BB_HALF_CARRY_FLAG;
            bb->reg.f |= BB_SUBTRACT_FLAG;
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
    uint8_t code[128] = { BB_OP_AND_A_HL, BB_OP_HALT };
    uint8_t ram[128];

    bb.rom = code;
    bb.ram = ram;
    bb.sp = sizeof(ram)-1;

    bb.reg.a = 0x0E;
    ram[0x30] = 0x06;
    bb.reg.l = 0x30;

    bb_vm(&bb);

    return 0;
}
