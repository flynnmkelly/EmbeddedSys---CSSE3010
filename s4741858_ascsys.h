  /** 
 **************************************************************
 * @file mylib/s4741858_ascsys.h
 * @author flynn kelly - s4741858
 * @date 28042023
 * @brief Nucleus of design project - co-ordinating all the 
 * FreeRTOS tasks and approriate peripheries.
 *
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 
 *************************************************************** 
 **/

#include "board.h"
#include "processor_hal.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"


/* Periphery Includes ---------------------------------------*/
#include "s4741858_oled.h"
#include "s4741858_txradio.h"
#include "s4741858_keypad.h"

#include "debug_log.h"


/* FreeRTOS Defines -----------------------------------------*/
// Task Priorities
#define SYSTASK_PRIORITY					( tskIDLE_PRIORITY + 4 ) // priorities

// Task Stack Allocations
#define SYSTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 5 )

/* State Enumerating -----------------------------------------*/
#define INIT_STATE 0
#define IDLE_STATE 1
#define TRANSMITTING_STATE 2
#define DISPLAYING_STATE 3


/* PACKET STARTERS  -----------------------------------------*/
#define XYZ_TYPE 0x22
#define ROT_TYPE 0x23
#define VAC_TYPE 0x24


/* EVENT KEYPAD MAPPINGS -----------------------------------------*/
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

#define KEYPAD_PRESS_EVENT    (EVT_KEY_0 | EVT_KEY_1 | EVT_KEY_2 | EVT_KEY_3 \
                             | EVT_KEY_4 | EVT_KEY_5 | EVT_KEY_6 | EVT_KEY_7 \
                             | EVT_KEY_8 | EVT_KEY_9 | EVT_KEY_A | EVT_KEY_B \
                             | EVT_KEY_C)

// Define Rest Later


/* Functions -----------------------------------------*/
void s4741858_reg_ascsys_hardware_init();
void s4741858_TaskAscFSMcontroller( void ); // RTOS
extern void s4741858_tsk_ascsystem_init();
void editRadioPacket(uint8_t sendRadioPacket[TASK_RADIO_PACKET_SIZE], int startIndex, uint8_t *newData, int newDataSize);
