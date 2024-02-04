 /** 
 **************************************************************
 * @file mylib/s4741858_keypad.c
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

/* INCLUDES ----------------------------------------------------------*/
#include "s4741858_keypad.h"
#include "debug_log.h"


#ifdef FreeRTOS
/* RTOS GLOBAL Structures (defined in .h) ----------------------------*/
EventGroupHandle_t keypadctrlEventGroup; // event group flags - mapping in .h

#endif

/* FSM VARABLES (states enumerated in .h) ----------------------------*/
static int KeypadFsmCurrentstate; //initial state is 0
static int KeypadStatus = 0; // if key pressed keypadStatus = 1
static unsigned char KeypadValue; // hex value 0x00 through 0x0F
static int debounceCount = 0;


/* FreeRTOS CODE-----------------------------------------------------*/

#ifdef FreeRTOS

/**
 * @brief Init task function to be called in main.c
 */
extern void s4741858_tsk_keypad_init() {

    xTaskCreate((void *) &s4741858TaskKeypadControl, (const signed char *) "KEYPAD", KEYPADTASK_STACK_SIZE, NULL, KEYPADTASK_PRIORITY, NULL);
    

}

/**
 * @brief FreeRTOS task to signal keypad press to main system,
 * should have relatively high priority. * 
 * 
 */
void s4741858TaskKeypadControl( void ) {

    // Init Hardware
    s4741858_reg_keypad_init();

    // Create event group
    keypadctrlEventGroup = xEventGroupCreate();
    EventBits_t keypadBits;

    unsigned char keypadValue;

    // Start cyclic executive
    for(;;) {
 
  
        taskENTER_CRITICAL();	
        s4741858_reg_keypad_fsmprocessing(); //implement row scan
        taskEXIT_CRITICAL();

        static int ptime = 0;

        // debounce count tweaked depending on priority and vTaskDelay
        if ( debounceCount > 2000 && s4741858_reg_keypad_read_status() != 0) {
            // TEST TOGGLE
            keypadValue = s4741858_reg_keypad_read_key();
            debounceCount = 0;
        }
        
        if (keypadValue != 0xFF) {

            switch (keypadValue) {
                case 0x01:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_1);
                    break;
                case 0x02:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_2);
                    break;
                case 0x03:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_3);
                    break;
                case 0x04:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_4);
                    break;
                case 0x05:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_5);
                    break;
                case 0x06:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_6);
                    break;
                case 0x07:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_7);
                    break;
                case 0x08:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_8);
                    break;
                case 0x09:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_9);
                    break;
                case 0x0A:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_A);
                    break;
                case 0x0B:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_B);
                    break;
                case 0x0C:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_C);
                    break;
                case 0x00:
                    keypadBits = xEventGroupSetBits(keypadctrlEventGroup, EVT_KEY_0);
                    break;
                // FILL OUT REST FOR EXTENSION
            }

            keypadValue = 0xFF;

        }
    }

    vTaskDelay(2);


}


#endif


/* Stages C Code -----------------------------------------------------*/

/**
 * @brief Processing function that implements the keypad rowscan
 * FSM, implemented within the main function and toggles between
 * states (where column is high). Modified with FreeRTOS 
 * capability - send event bit
 */
void s4741858_reg_keypad_fsmprocessing(void) {

    int nextState = INIT_STATE; //default starting next state

    switch(KeypadFsmCurrentstate) {

        case INIT_STATE:
            // Implement automatic transition to next state
            nextState = RSCAN1_STATE;
            break;
        
        case RSCAN1_STATE:
            // Row Scan State 1 - activate column 1 and read
            KEYPAD_COL1();
            uint8_t row1read = keypad_readrow();
            if (row1read != 0) {
                KeypadStatus = 1;
                debounceCount++;
                if (row1read == 0x01) {
                    KeypadValue = 0x01;
                }
                if (row1read == 0x02) {
                    KeypadValue = 0x04;
                }
                if (row1read == 0x04) {
                    KeypadValue = 0x07;
                }
                if (row1read == 0x08) {
                    KeypadValue = 0x00;
                }
            }
            nextState = RSCAN2_STATE;
            break;

        case RSCAN2_STATE:
           // Row Scan State 2 - activate column 2 and read
            KEYPAD_COL2();
            uint8_t row2read = keypad_readrow();
            if (row2read != 0) {
                
                KeypadStatus = 1;
                debounceCount++;
                if (row2read == 0x01) {
                    KeypadValue = 0x02;
                }
                if (row2read == 0x02) {
                    KeypadValue = 0x05;
                }
                if (row2read == 0x04) {
                    KeypadValue = 0x08;
                }
                if (row2read == 0x08) {
                    KeypadValue = 0x0F;
                }
            }
            nextState = RSCAN3_STATE;
            break;
        
        case RSCAN3_STATE:
           // Row Scan State 3 - activate column 3 and read
            KEYPAD_COL3();
            uint8_t row3read = keypad_readrow();
            if (row3read != 0) {
                KeypadStatus = 1;
                debounceCount++; // HANDLES THE Fluctuation
                if (row3read == 0x01) {
                    KeypadValue = 0x03;
                }
                if (row3read == 0x02) {
                    KeypadValue = 0x06;
                }
                if (row3read == 0x04) {
                    KeypadValue = 0x09;
                }
                if (row3read == 0x08) {
                    KeypadValue = 0x0E;
                }
            }
            nextState = RSCAN4_STATE;
            break;

        case RSCAN4_STATE:
           // Row Scan State 4 - activate column 2 and read
            KEYPAD_COL4();
            uint8_t row4read = keypad_readrow();
            if (row4read != 0) {
                KeypadStatus = 1;
                debounceCount++;
                if (row4read == 0x01) {
                    KeypadValue = 0x0A;
                }
                if (row4read == 0x02) {
                    KeypadValue = 0x0B;
                }
                if (row4read == 0x04) {
                    KeypadValue = 0x0C;
                }
                if (row4read == 0x08) {
                    KeypadValue = 0x0D;
                }
            }
            nextState = RSCAN1_STATE;
            break;
        }
    KeypadFsmCurrentstate = nextState;

}

/**
 * @brief Returns the status of the keypad pressed
 * @return If 1 - keypad pressed, default = 0
 */
int s4741858_reg_keypad_read_status(void) {
    int status = KeypadStatus;
    return status;
}

/**
 * @brief Function to be returned in main which acts
 * as a get function for keypad pressed.
 * @return Keypad value.
 */
unsigned char s4741858_reg_keypad_read_key(void) {
    KeypadStatus = 0;
    unsigned char value = KeypadValue;
    return value;
}

/**
 * @brief Read function for PMOD Keypad, implemented
 * within row scan FSM
 * @return 4 Bit mask of the associated row triggered.
 */
unsigned char keypad_readrow(void) {
    unsigned char rowMask = 0;
    //Check Row 1 - PE2 - PROJECT CHANGE!!
    if ((GPIOE->IDR & (0x0001 << 2)) == 0){
        rowMask = 0x01;
    }
    // Check Row 2 - D34 PE0
    if ((GPIOE->IDR & (0x0001 << 0)) == 0){
        rowMask = 0x02;
    }
    //Check Row 3 - PE15
    if ((GPIOE->IDR & (0x0001 << 15)) == 0){
        rowMask = 0x04;
    }
    //Check Row 4 - PE14
    if ((GPIOE->IDR & (0x0001 << 14)) == 0){
        rowMask = 0x08;
    }

    return rowMask;

}

/**
 * @brief Writes a given column to high
 * @param colval mask that sets column
 */
void keypad_writecol(unsigned char colval) {
    //check and set column 1 - PE12
    int col1 = (colval & (1 << 0)) >> 0;
    if (col1 == 0){
        GPIOE->ODR |= (0x01 << 12);
    }
    else {
        GPIOE->ODR &= ~(0x01 << 12);
    }
    //check and set column 2 - PE10
    int col2 = (colval & (1 << 1)) >> 1;
    if (col2 == 0){
        GPIOE->ODR |= (0x01 << 10);
    }
    else {
        GPIOE->ODR &= ~(0x01 << 10);
    }
    //check and set column 3 - PE7
    int col3 = (colval & (1 << 2)) >> 1;
    if (col3 == 0){
        GPIOE->ODR |= (0x01 << 7);
    }
    else {
        GPIOE->ODR &= ~(0x01 << 7);
    }
    //check and set column 4 - PE8
    int col4 = (colval & (1 << 3)) >> 3;
    if (col4 == 0){
        GPIOE->ODR |= (0x01 << 8);
    }
    else {
        GPIOE->ODR &= ~(0x01 << 8);
    }
}

/**
 * @brief Initialises all registers for inputs(rows),
 * and the outputs (columns) - PULL UP RESISTOR
 */
void s4741858_reg_keypad_init() {

    KeypadFsmCurrentstate = INIT_STATE;

    __GPIOE_CLK_ENABLE();
    // OUTPUT PINS - COLUMNS
    // Board D42 (col 4) - Port E Pin 8
    GPIOE->MODER &= ~(0x03 << (8* 2));  //clear bits
    GPIOE->MODER |= (0x01 << (8* 2));   //Set for push pull

    GPIOE->OSPEEDR &= ~(0x03<<(8* 2));
	GPIOE->OSPEEDR |=   0x02<<(8* 2);  // Set for Fast speed

    GPIOE->OTYPER &= ~(0x01 << 8);       //Clear Bit for Push/Pull Output
    // Activate the Pull-up resistor
    GPIOE->PUPDR &= ~(0x03 << (8* 2));   //Clear Bits
    GPIOE->PUPDR |= ((0x01) << (8* 2));  //Set for Pull UP output
    GPIOE->ODR |= (0x01 << 8); // Set default output to hiGH!!!

    // Board D41 (col3) - Port E Pin 7
    GPIOE->MODER &= ~(0x03 << (7* 2));  
    GPIOE->MODER |= (0x01 << (7* 2));  
    GPIOE->OSPEEDR &= ~(0x03<<(7* 2));
	GPIOE->OSPEEDR |=   0x02<<(7* 2); 
    GPIOE->OTYPER &= ~(0x01 << 7);
    GPIOE->PUPDR &= ~(0x03 << (7* 2));   
    GPIOE->PUPDR |= ((0x01) << (7* 2));
    GPIOE->ODR |= (0x01 << 7);  

    // Board D40 (col2) - Port E Pin 10
    GPIOE->MODER &= ~(0x03 << (10* 2));  
    GPIOE->MODER |= (0x01 << (10* 2));  
    GPIOE->OSPEEDR &= ~(0x03<<(10* 2));
	GPIOE->OSPEEDR |=   0x02<<(10* 2); 
    GPIOE->OTYPER &= ~(0x01 << 10);
    GPIOE->PUPDR &= ~(0x03 << (10* 2));   
    GPIOE->PUPDR |= ((0x01) << (10* 2));
    GPIOE->ODR |= (0x01 << 10);  

     // Board D39 (col1) - Port E Pin 12
    GPIOE->MODER &= ~(0x03 << (12* 2));  
    GPIOE->MODER |= (0x01 << (12* 2));  
    GPIOE->OSPEEDR &= ~(0x03<<(12* 2));
	GPIOE->OSPEEDR |=   0x02<<(12* 2); 
    GPIOE->OTYPER &= ~(0x01 << 12);
    GPIOE->PUPDR &= ~(0x03 << (12* 2));   
    GPIOE->PUPDR |= ((0x01) << (12* 2));
    GPIOE->ODR |= (0x01 << 12); 

    // INPUT PINS - ROWS
    // Board D38 (row4) - PORT E Pin 14
    GPIOE->MODER &= ~(0x03 << (14 * 2));    //Clear bits for input mode

    GPIOE->OSPEEDR &= ~(0x03<<(14 * 2));
	GPIOE->OSPEEDR |=   0x02<<(14 * 2);  // Fast speed

	GPIOE->PUPDR &= ~(0x03 << (14 * 2));	//Clear bits for no push/pull
    GPIOE->PUPDR |= ((0x01) << (14 * 2));  //Set for Pull UP input

    // Board D37 (row3) - PORT E Pin 15
    GPIOE->MODER &= ~(0x03 << (15 * 2));    
    GPIOE->OSPEEDR &= ~(0x03<<(15 * 2));
	GPIOE->OSPEEDR |=   0x02<<(15 * 2);  
	GPIOE->PUPDR &= ~(0x03 << (15 * 2));	
    GPIOE->PUPDR |= ((0x01) << (15 * 2));

    // Board D36 (row2) - PORT B Pin 10
    // CHANGED TO D34 for PROJECT
    // __GPIOB_CLK_ENABLE();
    // GPIOB->MODER &= ~(0x03 << (10 * 2));    
    // GPIOB->OSPEEDR &= ~(0x03<<(10 * 2));
	// GPIOB->OSPEEDR |=   0x02<<(10 * 2);  
	// GPIOB->PUPDR &= ~(0x03 << (10 * 2));	
    // GPIOB->PUPDR |= ((0x01) << (10 * 2));

    // Board D34 (row2) - PORT E Pin 0
    GPIOE->MODER &= ~(0x03 << (0 * 2));    
    GPIOE->OSPEEDR &= ~(0x03 << (0 * 2));
    GPIOE->OSPEEDR |= 0x02 << (0 * 2);  
    GPIOE->PUPDR &= ~(0x03 << (0 * 2));	
    GPIOE->PUPDR |= (0x01 << (0 * 2));

    // Board D31 (row1) - PORT E Pin 2 -> CHANGED!!
    GPIOE->MODER &= ~(0x03 << (2 * 2));    
    GPIOE->OSPEEDR &= ~(0x03 << (2 * 2));
    GPIOE->OSPEEDR |= 0x02 << (2 * 2);  
    GPIOE->PUPDR &= ~(0x03 << (2 * 2));	
    GPIOE->PUPDR |= (0x01 << (2 * 2));

}




