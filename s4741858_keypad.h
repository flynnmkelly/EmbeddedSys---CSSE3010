 /** 
 **************************************************************
 * @file mylib/s4741858_keypad.h
 * @author flynn kelly - s4741858
 * @date 17032023
 * @brief Driver functions for the keypad periphery as 
 * stipulated in the mylibguide.
 *************************************************************** 
  * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4741858_reg_keypad_fsmprocessing() - implements fsm for the 
 * PMOK keypad rowscanning, circular FSM
 * s4741858_reg_keypad_read_status() - returns keypad hit status
 * s4741858_reg_keypad_read_key() - get function for key hit
 *************************************************************** 
 */



/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"


#define FreeRTOS // ENABLES FREERTOS FUNCTIONALITY ----------

// RTOS
#ifdef FreeRTOS

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#endif

/* State Enumerating -----------------------------------------*/
#define INIT_STATE 0
#define RSCAN1_STATE 1
#define RSCAN2_STATE 2
#define RSCAN3_STATE 3
#define RSCAN4_STATE 4

/* Column Row Scan Functions----------------------------------*/
#define KEYPAD_COL1() keypad_writecol(0x01)
#define KEYPAD_COL2() keypad_writecol(0x02)
#define KEYPAD_COL3() keypad_writecol(0x04)
#define KEYPAD_COL4() keypad_writecol(0x08)

/* .c File Functions -----------------------------------------*/
extern void s4741858_reg_keypad_init();
extern void s4741858_reg_keypad_fsmprocessing(void);
extern int s4741858_reg_keypad_read_status(void);
extern unsigned char s4741858_reg_keypad_read_key(void);
unsigned char keypad_readrow(void); 
void keypad_writecol(unsigned char colval);

/* RTOS Functions -----------------------------------------*/
extern void s4741858_tsk_keypad_init();
void s4741858TaskKeypadControl( void );

/* FreeRTOS Defines -----------------------------------------*/
// Task Priorities
#define KEYPADTASK_PRIORITY		( tskIDLE_PRIORITY + 3 ) // priorities

// Task Stack Allocations
#define KEYPADTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )


/* KEYPAD MAPPINGS -----------------------------------------*/
#define EVT_KEY_1   1 << 0 // Point [0,0]
#define EVT_KEY_2   1 << 1 // Point [75,0]
#define EVT_KEY_3   1 << 2 // Point [150,0]
#define EVT_KEY_A   1 << 3 // Lower Z
#define EVT_KEY_4   1 << 4 // Point [0,75]
#define EVT_KEY_5   1 << 5 // Point [75,75]
#define EVT_KEY_6   1 << 6 // Point [150,75]
#define EVT_KEY_B   1 << 7 // Raise Z
#define EVT_KEY_7   1 << 8 // Point [0,150]
#define EVT_KEY_8   1 << 9 // Point [75,150]
#define EVT_KEY_9   1 << 10 // Point [150,150]
#define EVT_KEY_C   1 << 11 // Point [Rotate +10]
#define EVT_KEY_0   1 << 12 // Toggle Vacuum

                             
