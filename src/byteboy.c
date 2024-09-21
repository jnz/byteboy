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

int bb_vm(bb_state_t* bb)
{
    uint8_t* code = bb->rom;
    uint8_t* ram = bb->ram;


    while (1)
    {
        switch (code[bb->pc++])
        {
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
            bb_add(bb, bb->reg.a, ram[bb->reg.hl & BB_MEM_MASK]);
            break;
        case BB_OP_ADD_A_N:
            bb_add(bb, bb->reg.a, code[bb->pc++]);
            break;

        default:
            assert(0);
        };
    }
}

int main(int argc, char** argv)
{

    return 0;
}
