 /** 
 **************************************************************
 * @file mylib/s4741858_boardpb.h
 * @author flynn kelly - s4741858
 * @date 21032023
 * @brief Driver functions for the nucleo board pushbutton
 *************************************************************** 
  * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4741858_reg_board_pb_init() - Initialises relevant port and
 * pin for user button on nucleo board (interrupt enable)
 * s4741858_reg_boardpb_status_get() - Returns the toggle value
 * from the pushbutton (essentially pressed isr).
 *************************************************************** 
 */

/* FreeRTOS ------------------- */
#define FreeRTOS

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"

#ifdef FreeRTOS
// FreeRTOS
#include "s4741858_txradio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#endif

/* .c File Functions ---------------------------------------------------------*/
extern void s4741858_reg_board_pb_isr();
extern void s4741858_reg_board_pb_init();
extern int s4741858_reg_boardpb_status_get();

/* Function Variables ---------------------------------------------------------*/
#define ENCODE_MODE 0 //default state
#define DECODE_MODE 1
static volatile int operatingMode;
