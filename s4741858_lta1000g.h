 /** 
 **************************************************************
 * @file mylib/s4741858_lta1000g.h
 * @author flynn kelly - s4741858
 * @date 27022023
 * @brief LED bar (peripheral) driver file, includes initialising
 *        periphery and then writing the bits to the LED bar
 *        in binary.
 *
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4741858_reg_lta1000g_init() - initialises all the registers
 * in output mode for the LED bar.
 * s4741858_reg_lta1000g_write() - writes the input int to
 * the LED bar in binary.
 *************************************************************** 
 **/

#include "board.h"
#include "processor_hal.h"

extern void s4741858_reg_lta1000g_init();
extern void s4741858_reg_lta1000g_write(unsigned short value);
extern void lta1000g_seg_set(int segment, unsigned char value);


