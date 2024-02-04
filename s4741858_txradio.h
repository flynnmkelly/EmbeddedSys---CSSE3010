 #ifndef RADIO_H
 #define RADIO_H
  /** 
 **************************************************************
 * @file mylib/s4741858_txradio.h
 * @author flynn kelly - s4741858
 * @date 28042023
 * @brief Mylib driver for the NRF24l01plus Radio Module,
 * responsible for sending ASC radio control packets.
 *
 ***************************************************************
 **/

#include "board.h"
#include "processor_hal.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "semphr.h"

#include "nrf24l01plus.h" // periphery source code
#include "s4741858_boardpb.h"
#include "s4741858_hamming.h"
//#include "s4741858_ascsys.h" 

#include "debug_log.h"

// COPY INIT CODE FROM HERE


#define FreeRTOS // ENABLES FREERTOS FUNCTIONALITY ----------
// This mylib designed for RTOS

/* FreeRTOS Defines -----------------------------------------*/
// Task Priorities
#define RADIOTASK_PRIORITY					( tskIDLE_PRIORITY + 3 ) // priorities

// Task Stack Allocations
#define RADIOTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 5 ) // NEED DEBUG???

// Project queue mesage struct - radio / sys control communication
// typedef struct {
//     uint8_t type;
//     uint32_t sender_addr;
//     char payload[11];
// } TXRadio_ASCMessage; // UNENCODED - CHANGE TO 16 bytes

#define TASK_RADIO_PACKET_SIZE 16 // change accordingly
#define ENCODED_RADIO_PACKET_SIZE 32 // hamming encoded

extern QueueHandle_t s4741858QueueRadioTXMessage; // global define.

/* State Enumerating -----------------------------------------*/
#define INIT_STATE 0
#define IDLE_STATE 1
#define ENCODE_STATE 2
#define TRANSMIT_STATE 3
// SHOULD SEND VIA EVENT BITS?? 

/* RTOS Functions -----------------------------------------*/
extern void s4741858_tsk_txradio_init();
void s4741858TaskTxradioControl( void );
void s4741858_reg_board_hardware_init();

#endif
