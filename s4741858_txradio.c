  /** 
 **************************************************************
 * @file mylib/s4741858_txradio.c
 * @author flynn kelly - s4741858
 * @date 28042023
 * @brief Mylib driver for the NRF24l01plus Radio Module,
 * responsible for sending ASC radio control packets.
 *
 ***************************************************************
 **/

/* INCLUDES ----------------------------------------------------------*/
#include "s4741858_txradio.h"

#ifdef FreeRTOS
/* RTOS Structures (defined in .h) ----------------------------*/
QueueHandle_t s4741858QueueRadioTXMessage; // event group flags - mapping in .h
SemaphoreHandle_t s4741858SemaphorePBSig;
#endif

/* FreeRTOS CODE-----------------------------------------------------*/

#ifdef FreeRTOS

/**
 * @brief Creates the task that operates
 */
extern void s4741858_tsk_txradio_init() {

  // Create the radio controller FSM task
  xTaskCreate( (void *) &s4741858TaskTxradioControl, (const signed char *) "RADIO", RADIOTASK_STACK_SIZE, NULL, RADIOTASK_PRIORITY, NULL );

}

/**
 * @brief Initialises LED's for task requirements
 */
void s4741858_reg_board_hardware_init(){

  taskENTER_CRITICAL();	// No Interruption - DEBUG LOG

	BRD_LEDInit();				//Initialise Green LED
	BRD_LEDGreenOff();		//Turn off Green LED
  BRD_LEDGreenOn();	
  BRD_LEDBlueOff();
  BRD_LEDRedOff();

  BRD_debuguart_init();

	taskEXIT_CRITICAL();

}

/**
 * @brief FreeRTOS task to control operaiton of the
 * NRF24l01plus - FSM Controller Operation
 * 
 */
void s4741858TaskTxradioControl( void ) {

  // init hardware
  nrf24l01plus_init();
  s4741858_reg_board_hardware_init();
  s4741858_reg_board_pb_init(); // NEED ENTER CRITICAL??
  // init the channel myconfig.h!!

  // QUEUE MESSAGE
  uint8_t ReceiveRadioPacket[TASK_RADIO_PACKET_SIZE];

  // Create Queue
  s4741858QueueRadioTXMessage = xQueueCreate(10, sizeof(ReceiveRadioPacket));	
  s4741858SemaphorePBSig = xSemaphoreCreateBinary();
  uint8_t pbPressed = 0;

  // enter loop - FSM for radio
  static int RadioFSMCurrentState = INIT_STATE;

  // PACKET VARIABLES - Refresh to Zero Padding
  uint8_t global_packet_unencoded[TASK_RADIO_PACKET_SIZE] = {0}; // default zero padded
  uint8_t global_packet_encoded[ENCODED_RADIO_PACKET_SIZE] = {0}; // for the different states

  
  for (;;) {

    int nextState = INIT_STATE;


    switch(RadioFSMCurrentState) {
    
      case INIT_STATE:
        
        nextState = IDLE_STATE;
        
        break;
      
      // Essentially Waits for Board PB or Button Press
      case IDLE_STATE:

        // BOARD PB 
        if (s4741858SemaphorePBSig != NULL) {
          
          // SEND JOIN MESSAGE
          if (xSemaphoreTake( s4741858SemaphorePBSig, 10 ) == pdTRUE ) {
            
            // TOGGLE LED - GETS HERE
            BRD_LEDBlueToggle(); // Toggled here due to semaphore
            
            // SET UNENCODED BYTE
            global_packet_unencoded[0] = 0x20; // JOIN TYPE
            global_packet_unencoded[1] = 0x47; // SENDER ADDRESS
            global_packet_unencoded[2] = 0x41;
            global_packet_unencoded[3] = 0x85;
            global_packet_unencoded[4] = 0x89;
            global_packet_unencoded[5] = 'J'; // JOIN MESSAGE - chars ASCII cast
            global_packet_unencoded[6] = 'O';
            global_packet_unencoded[7] = 'I';
            global_packet_unencoded[8] = 'N';

            pbPressed = 1;
            // Progress to Hamming
            nextState = ENCODE_STATE;
  
          } else {
            nextState = IDLE_STATE;
            pbPressed = 0;
          }
        }

        if (s4741858QueueRadioTXMessage != NULL) {


          if (xQueueReceive(s4741858QueueRadioTXMessage, &ReceiveRadioPacket, 10 )) {
          
            // Store the unencoded radio packet in the global variable
            for (int i = 0; i < TASK_RADIO_PACKET_SIZE; i++) {
              global_packet_unencoded[i] = ReceiveRadioPacket[i];
            }
            
           // nrf24l01plus_send(global_packet_unencoded); // send unencoded - testing
            
            nextState = ENCODE_STATE; 
          } else if (pbPressed == 0) { // ELSE IF PUSHBUTTON SEMAPHORE FLAG - looping to IDLE
            nextState = IDLE_STATE;
          }

          //nextState = IDLE_STATE; // stay waiting for message

        }
        break;
      
      case ENCODE_STATE: // gets here if queue received

        // DO THE HAMMING ENCODING
                    // loop over each byte in the input packet and encode it with Hamming code
        for (int i = 0; i < TASK_RADIO_PACKET_SIZE; i++) {
          uint16_t encoded_byte =
              s4741858_lib_hamming_byte_encoder(global_packet_unencoded[i]);

          // copy the two bytes of the encoded byte into the output packet
          global_packet_encoded[i * 2] = encoded_byte & 0xFF;
          global_packet_encoded[i * 2 + 1] = (encoded_byte >> 8) & 0xFF;
          
        
        }

        nextState = TRANSMIT_STATE;
        break;
      
      case TRANSMIT_STATE:

        nrf24l01plus_send(global_packet_encoded); // sends encoded -
        // RESET BUFFERS TO ZERO - FUNCTION!!!
        nextState = IDLE_STATE; 
        break;

      
    }
    RadioFSMCurrentState = nextState;

    vTaskDelay(8);
  }

  


}


#endif
