 #ifndef SYS_H
 #define SYS_H
/** 
 **************************************************************
 * @file mylib/s4741858_ascsys.c
 * @author flynn kelly - s4741858
 * @date 28042023
 * @brief Nucleus of design project - co-ordinating all the 
 * FreeRTOS tasks and approriate peripheries.
 * 
 * Has large task memory due to debugging with UART debugn log
 *
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 
 *************************************************************** 
 **/

#include "s4741858_ascsys.h"

/* Global RTOS Structures Decleration */
extern QueueHandle_t s4741858QueueOLEDMessage; // OLED TASK
extern EventGroupHandle_t keypadctrlEventGroup; // KEYPAD TASK
extern QueueHandle_t s4741858QueueRadioTXMessage;// RADIO QUEUE

static uint16_t currentKeypadValue; // keypad value for FSM controller

/**
 * @brief Initialises LED's for task requirements
 */
void s4741858_reg_ascsys_hardware_init(){

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
 * @brief Creates this mylib task, has high priority as it essentially
 * co-ordinates the whole sorting operation.
 * 
 */
extern void s4741858_tsk_ascsystem_init() {

  // Create the asc controller FSM task
  xTaskCreate( (void *) &s4741858_TaskAscFSMcontroller, (const signed char *) "SYS", SYSTASK_STACK_SIZE, NULL, SYSTASK_PRIORITY, NULL );

}

/**
 * @brief TASK that cycles througn the states appropriately.
 */
void s4741858_TaskAscFSMcontroller( void ) {

  // INIT STUFF -----------------------------------------------------
  // KEYPAD TASK VARIABLES
  EventBits_t keypadBits;

  // KEYPAD VARIABLES / STRUCT
  OLED_ASCMessage SendValues;
  uint8_t z = 0; // default values
  uint8_t x = 0;
  uint8_t y = 0;
  uint8_t angle = 0;
  uint8_t vacumStatus = 0;

  // RADIO QUEUE MESSAGE DECLARATIONS
  uint8_t senderAdress[4] = {0x47, 0x41, 0x85, 0x89};
  uint8_t xyzMessage[3] = {'X', 'Y', 'Z'};
  uint8_t rotMessage[3] = {'R', 'O', 'T'};
  uint8_t VonMessage[3] = {'V', 'O', 'N'};
  uint8_t VoffMessage[4] = {'V', 'O', 'F', 'F'};

  s4741858_reg_ascsys_hardware_init(); // hardware 

  static int ControllerFsmCurrentstate = INIT_STATE; //INITIAL IDLE STATE

  // 1s Tick Timer
  uint32_t previous_tick = HAL_GetTick();

  // CYCLIC EXECUTIVE -----------------------------------------------------

  for (;;) {

    // RADIO PACKET (default zero pads)
    uint8_t sendRadioPacket[TASK_RADIO_PACKET_SIZE] = {0}; // should be 16 UNENCODED

    // FLASH THE GREEN LED - TASK 1
    uint32_t current_tick = HAL_GetTick();
    if (current_tick - previous_tick >= 1000) { // 1 second interval
      previous_tick = current_tick;
      BRD_LEDGreenToggle();
    }

    // STATE MACHINE CONTROLLER
    int NextState = INIT_STATE; //default next state

    switch (ControllerFsmCurrentstate) {

      case INIT_STATE:
        // Automatic transition to next state
        NextState = IDLE_STATE;
        break;
      
      // Waits for input from keypad - loops in this state
      case IDLE_STATE:
        
        if (keypadctrlEventGroup != NULL) {
          // WAIT FOR THE KEYPAD BITS
          keypadBits = xEventGroupWaitBits(keypadctrlEventGroup, KEYPAD_PRESS_EVENT, pdTRUE, pdFALSE, 10);

          // SETS CURRENT KEYPAD VALUE FOR REST OF STATE  ACHINE
          if (keypadBits != 0) {
            BRD_LEDRedToggle(); // SYSTEM STATUS INDICATOR
            NextState = DISPLAYING_STATE; // Next state only if keypad pressed!

            // Sets the global variable keypad value for next states
            switch(keypadBits) {
              
              case EVT_KEY_1:
                currentKeypadValue = EVT_KEY_1; 
                break;
              case EVT_KEY_2:
                currentKeypadValue = EVT_KEY_2;    
                break;   
              case EVT_KEY_3:
                currentKeypadValue = EVT_KEY_3;
                break;       
              case EVT_KEY_4:
                currentKeypadValue = EVT_KEY_4;
                break;
              case EVT_KEY_5:
                currentKeypadValue = EVT_KEY_5;
                break;
              case EVT_KEY_6:
                currentKeypadValue = EVT_KEY_6;
                break;
              case EVT_KEY_7:
                currentKeypadValue = EVT_KEY_7;
                break;
              case EVT_KEY_8:
                currentKeypadValue = EVT_KEY_8;
                break;
              case EVT_KEY_9:
                currentKeypadValue = EVT_KEY_9;
                break;
              case EVT_KEY_A:
                currentKeypadValue = EVT_KEY_A;
                break;
              case EVT_KEY_B:
                currentKeypadValue = EVT_KEY_B;
                break;
              case EVT_KEY_C:
                currentKeypadValue = EVT_KEY_C;
                break;
              case EVT_KEY_0:
                currentKeypadValue = EVT_KEY_0;
                break;

              // Add extension keys if attempted           
            }
          
          } else {
            NextState = IDLE_STATE; // stays in idle if no key pressed
          }       
        }  
        break; // next state

      // Co-ordinates with OLED task
      case DISPLAYING_STATE:

        // Check Queue Exists - Send Display Values
        if (s4741858QueueOLEDMessage != NULL) {

          // Send messages varied acording to keypress  
          SendValues.string = "+";     
          switch (currentKeypadValue) {
            case EVT_KEY_1:
              SendValues.cursorXLocation = 1; // corresponds to top left
              SendValues.cursorYLocation = 3; // [0, 150]
              x = 0;
              y = 150;
              break;             
            case EVT_KEY_2:
              SendValues.cursorXLocation = 12; // corresponds to top middle
              SendValues.cursorYLocation = 3; // [75, 150]
              x = 75;
              y = 150;
              break;  
            case EVT_KEY_3:
              SendValues.cursorXLocation = 25; // corresponds to 
              SendValues.cursorYLocation = 3; // [150, 150]
              y = 150; 
              x = 150;
              break; 
            case EVT_KEY_A:
              if (z >= 10) {
                z = z - 10; // change z position
              }
              break;  
            case EVT_KEY_4:
              SendValues.cursorXLocation = 1; // corresponds to middle left
              SendValues.cursorYLocation = 13; // [0, 75]    
              x = 0;
              y = 75;   
              break;
            case EVT_KEY_5:
              SendValues.cursorXLocation = 13; // corresponds to middle left
              SendValues.cursorYLocation = 13; // [75, 75] 
              x = 75;
              y = 75;      
              break; 
            case EVT_KEY_6:
              SendValues.cursorXLocation = 25; // corresponds to middle left
              SendValues.cursorYLocation = 13; // [150, 75]  
              x = 150;
              y = 75;     
              break;
            case EVT_KEY_B:
              if (z <= 80) { 
                z += 10; 
              } 
              break;
            case EVT_KEY_7:
              SendValues.cursorXLocation = 1; // corresponds to bottom left
              SendValues.cursorYLocation = 23; // [0, 0] 
              x = 0;
              y = 0;
              break;
            case EVT_KEY_8:
              SendValues.cursorXLocation = 13; // corresponds to bottom middle
              SendValues.cursorYLocation = 23; // [75, 0] 
              x = 75;
              y = 0;
              break; 
            case EVT_KEY_9:
              SendValues.cursorXLocation = 25; // corresponds to bottom right
              SendValues.cursorYLocation = 23; // [150, 0]
              x = 150;
              y = 0; 
              break;
            case EVT_KEY_C:
              if (angle <= 170) {
                angle += 10;
              }
              break;
            case EVT_KEY_0:
              vacumStatus ^= 0xFF; // TOGGLE VACCUM (bitwise)
              break; 
                                         
          } 
          NextState = TRANSMITTING_STATE;
          
          
          SendValues.z = z; // sends according to updated value
          SendValues.angle = angle;

          //send to OLED mylib task
          xQueueSend(s4741858QueueOLEDMessage, &SendValues, 10);
        }
        break;

      // Sends Radio Package to NRF
      case TRANSMITTING_STATE:

        // DECLARATION ERROR - filler code
        sendRadioPacket[0] = 0x00;

        // RADIO POSITION VARIABLES
        uint8_t ascXpos[3] = {0};
        uint8_t ascYpos[3] = {0};
        int8_t ascZpos[2] = {0};

        // FIll in position from OLED - CONVERT TO ASCII
        ascXpos[0] = (x / 100) % 10 + '0'; // calculate hundreds digit
        ascXpos[1] = (x / 10) % 10 + '0';  // calculate tens digit
        ascXpos[2] = x % 10 + '0';         // calculate ones digit
        ascYpos[0] = (y / 100) % 10 + '0';
        ascYpos[1] = (y / 10) % 10 + '0';
        ascYpos[2] = y % 10 + '0';
        ascZpos[0] = (z / 10) % 10 + '0';
        ascZpos[1] = z % 10 + '0';        

        // Check Queue Exists
        if (s4741858QueueRadioTXMessage != NULL) {
          // Fill queue according to message type - default zero padded
          switch (currentKeypadValue) {
           
           // XYZ PACKET TYPE
            case EVT_KEY_1:
            case EVT_KEY_2:
            case EVT_KEY_3:
            case EVT_KEY_A:
            case EVT_KEY_4:
            case EVT_KEY_5:   
            case EVT_KEY_6:
            case EVT_KEY_B: 
            case EVT_KEY_7:
            case EVT_KEY_8:
            case EVT_KEY_9:

              sendRadioPacket[0] = XYZ_TYPE; // packet type

              editRadioPacket(sendRadioPacket, 1, senderAdress, 4); // sender address

              editRadioPacket(sendRadioPacket, 5, xyzMessage, 3); // xyz payload

              editRadioPacket(sendRadioPacket, 8, ascXpos, 3);  // x-pos
              editRadioPacket(sendRadioPacket, 11, ascYpos, 3); // y-pos
              editRadioPacket(sendRadioPacket, 14, ascZpos, 2); // z-pos
              break;             

            // ANGLE ROTATION PACKET TYPE
            case EVT_KEY_C: 
              sendRadioPacket[0] = ROT_TYPE;
              editRadioPacket(sendRadioPacket, 1, senderAdress, 4); // sender address

              editRadioPacket(sendRadioPacket, 5, rotMessage, 3); // angle payload

              uint8_t ascAngle[3] = {0};
              ascAngle[0] = (angle / 100) % 10 + '0'; // calculate hundreds digit
              ascAngle[1] = (angle / 10) % 10 + '0';  // calculate tens digit
              ascAngle[2] = angle % 10 + '0';         // calculate ones digit
              editRadioPacket(sendRadioPacket, 8, ascAngle, 3); // angle value
              
              break;

            // VACUUM PACKET TYPE
            case EVT_KEY_0:
              sendRadioPacket[0] = VAC_TYPE;
              editRadioPacket(sendRadioPacket, 1, senderAdress, 4); // sender address
              if (vacumStatus == 0) {
                editRadioPacket(sendRadioPacket, 5, VoffMessage, 4); // vacuum off
              } else {
                editRadioPacket(sendRadioPacket, 5, VonMessage, 3); // vacuum on
              }

              break;                                          
          }          
          // send un-encoded packet to the radio mylib task
          xQueueSend(s4741858QueueRadioTXMessage, &sendRadioPacket, 10);
          BRD_LEDBlueToggle();
          // circular change
        }
          
        NextState = IDLE_STATE; // automatic transition back to idle
        break;
    }

    ControllerFsmCurrentstate = NextState; // DO THE STATE CHANGE
    
  // Delay Length?????
  vTaskDelay(50);

  }
  
}

/**
 * @brief This function takes in the radio packet array, the index at which to start editing, the new data to be inserted,
 * and the size of the new data - INTERNAL FUNCTION
 */
void editRadioPacket(uint8_t sendRadioPacket[TASK_RADIO_PACKET_SIZE], int startIndex, uint8_t *newData, int newDataSize) {

    for (int i = 0; i < newDataSize; i++) {
        sendRadioPacket[startIndex + i] = newData[i];
    }

}

#endif

