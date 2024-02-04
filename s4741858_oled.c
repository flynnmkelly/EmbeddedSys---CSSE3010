  /** 
 **************************************************************
 * @file mylib/s4741858_oled.c
 * @author flynn kelly - s4741858
 * @date 05042023
 * @brief Periphery driver function for the SSD1306 OLED display
 * using i2c communications
 *
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 
 *************************************************************** 
 **/

#include "board.h"
#include "processor_hal.h"

#include "s4741858_oled.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

QueueHandle_t s4741858QueueOLEDMessage;

/* REGULAR FUNCTIONS ---------------------------------------------------------*/

/**
 * @brief FreeRTOS task to share data via queue
 * 
 * Need to worry about enter critical if priority higher??
 */
void s4741858_reg_oled_init() {
    uint32_t pclk1;
    uint32_t freqrange;

    // Enable GPIO clock
    I2C_DEV_GPIO_CLK();

    //******************************************************
    // IMPORTANT NOTE: SCL Must be Initialised BEFORE SDA
    //******************************************************

    //Clear and Set Alternate Function for pin (lower ARF register) 
    MODIFY_REG(I2C_DEV_GPIO->AFR[1], ((0x0F) << ((I2C_DEV_SCL_PIN-8) * 4)) | ((0x0F) << ((I2C_DEV_SDA_PIN-8)* 4)), ((I2C_DEV_GPIO_AF << ((I2C_DEV_SCL_PIN-8) * 4)) | (I2C_DEV_GPIO_AF << ((I2C_DEV_SDA_PIN-8)) * 4)));
    
    //Clear and Set Alternate Function Push Pull Mode
    MODIFY_REG(I2C_DEV_GPIO->MODER, ((0x03 << (I2C_DEV_SCL_PIN * 2)) | (0x03 << (I2C_DEV_SDA_PIN * 2))), ((GPIO_MODE_AF_OD << (I2C_DEV_SCL_PIN * 2)) | (GPIO_MODE_AF_OD << (I2C_DEV_SDA_PIN * 2))));
    
    //Set low speed.
    SET_BIT(I2C_DEV_GPIO->OSPEEDR, (GPIO_SPEED_LOW << I2C_DEV_SCL_PIN) | (GPIO_SPEED_LOW << I2C_DEV_SDA_PIN));

    //Set Bit for Push/Pull output
    SET_BIT(I2C_DEV_GPIO->OTYPER, ((0x01 << I2C_DEV_SCL_PIN) | (0x01 << I2C_DEV_SDA_PIN)));

    //Clear and set bits for no push/pull
    MODIFY_REG(I2C_DEV_GPIO->PUPDR, (0x03 << (I2C_DEV_SCL_PIN * 2)) | (0x03 << (I2C_DEV_SDA_PIN * 2)), (GPIO_PULLUP << (I2C_DEV_SCL_PIN * 2)) | (GPIO_PULLUP << (I2C_DEV_SDA_PIN * 2)));

    // Configure the I2C peripheral
    // Enable I2C peripheral clock
    __I2C1_CLK_ENABLE();

    // Disable the selected I2C peripheral
    CLEAR_BIT(I2C_DEV->CR1, I2C_CR1_PE);

  	pclk1 = HAL_RCC_GetPCLK1Freq();			// Get PCLK1 frequency
  	freqrange = I2C_FREQRANGE(pclk1);		// Calculate frequency range 

  	//I2Cx CR2 Configuration - Configure I2Cx: Frequency range
  	MODIFY_REG(I2C_DEV->CR2, I2C_CR2_FREQ, freqrange);

	  // I2Cx TRISE Configuration - Configure I2Cx: Rise Time
  	MODIFY_REG(I2C_DEV->TRISE, I2C_TRISE_TRISE, I2C_RISE_TIME(freqrange, I2C_DEV_CLOCKSPEED));

   	// I2Cx CCR Configuration - Configure I2Cx: Speed
  	MODIFY_REG(I2C_DEV->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), I2C_SPEED(pclk1, I2C_DEV_CLOCKSPEED, I2C_DUTYCYCLE_2));

   	// I2Cx CR1 Configuration - Configure I2Cx: Generalcall and NoStretch mode
  	MODIFY_REG(I2C_DEV->CR1, (I2C_CR1_ENGC | I2C_CR1_NOSTRETCH), (I2C_GENERALCALL_DISABLE| I2C_NOSTRETCH_DISABLE));

   	// I2Cx OAR1 Configuration - Configure I2Cx: Own Address1 and addressing mode
  	MODIFY_REG(I2C_DEV->OAR1, (I2C_OAR1_ADDMODE | I2C_OAR1_ADD8_9 | I2C_OAR1_ADD1_7 | I2C_OAR1_ADD0), I2C_ADDRESSINGMODE_7BIT);

   	// I2Cx OAR2 Configuration - Configure I2Cx: Dual mode and Own Address2
  	MODIFY_REG(I2C_DEV->OAR2, (I2C_OAR2_ENDUAL | I2C_OAR2_ADD2), I2C_DUALADDRESS_DISABLE);

  	// Enable the selected I2C peripheral
	  SET_BIT(I2C_DEV->CR1, I2C_CR1_PE);

	  ssd1306_Init();	//Initialise SSD1306 OLED.

}


/**
 * @brief FreeRTOS task to share data via queue
 * 
 * Need to worry about enter critical if priority higher??
 */
void s4741858_reg_oled_asc_grid_init() {
  	
    //Clear Screen
		
		ssd1306_Fill(Black);

		//Draw Horizontal lines of boundary box - DEFINE 30
		for (int i=0; i < 30; i++) {

			ssd1306_DrawPixel(i, 0, SSD1306_WHITE);					//top line
			ssd1306_DrawPixel(i, SSD1306_HEIGHT-1, SSD1306_WHITE);	//bottom line
		}

		//Draw Vertical lines of boundary box - EDIT
		for (int i=1; i < 30; i++) {

			ssd1306_DrawPixel(0, i, SSD1306_WHITE);					//left line
			ssd1306_DrawPixel(30-1, i, SSD1306_WHITE);	//right line

		}

    //Z Value
		ssd1306_SetCursor(60,5);
		ssd1306_WriteString("Z:", Font_6x8, SSD1306_WHITE);

    //ANGLE
		ssd1306_SetCursor(60,18);
		ssd1306_WriteString("Angle:", Font_6x8, SSD1306_WHITE);

    ssd1306_UpdateScreen();

}



/* FreeRTOS Functions-----------------------------------------*/

/**
 * @brief FreeRTOS init function - creates task (call in main)
 * 
 * PRIORITY - is given highest overall priority because it is an
 * overall low latency function and it stops context switching during
 * task (equivalent to enter critical)
 */
extern void s4741858_tsk_oled_init() {

	// Start the joystick control task
  	xTaskCreate( (void *) &s4741858TaskOLEDControl, (const signed char *) "T_OLED", OLEDTASK_STACK_SIZE, NULL, OLEDTASK_PRIORITY, NULL );

}

/**
 * @brief FreeRTOS task that receives message of certain type, ie.
 * can be struct or string or int - through the s4741858QueueOLEDMessage 
 *
 */
void s4741858TaskOLEDControl( void ) {
  

  //OLED_Message RecvMessage;
  OLED_ASCMessage RecvMessage;

  taskENTER_CRITICAL();	//Stop any interruption of the critical section
  // Init the relevant hardware - ssd OLED
	s4741858_reg_oled_init();

  taskEXIT_CRITICAL();

	// Create queue - DEPENDING ON RecvMessage
  s4741858QueueOLEDMessage = xQueueCreate(10, sizeof(RecvMessage));	

  
	for (;;) {

    // STAGE 4 CODE
    if (s4741858QueueOLEDMessage != NULL) {

      // Check for item received - block atmost for 10 ticks
			if (xQueueReceive(s4741858QueueOLEDMessage, &RecvMessage, 10 )) {
        
        s4741858_reg_oled_asc_grid_init();

        // Draw the string - based on position (SCALED) and string value
        // Positioning handled in ASCSYS       
        ssd1306_SetCursor(RecvMessage.cursorXLocation, RecvMessage.cursorYLocation); // dx and PF - Y POSITION
        ssd1306_WriteString(RecvMessage.string, Font_6x8, SSD1306_WHITE);

        // PROJECT ADDITIONS -------------------
        ssd1306_SetCursor(80, 5);
        char zPos[4];
        sprintf(zPos, "%d", RecvMessage.z); 
        ssd1306_WriteString(zPos, Font_6x8, SSD1306_WHITE); // EDIT

        ssd1306_SetCursor(100, 18); 
        char angle[4];
        sprintf(angle, "%d", RecvMessage.angle); 
        ssd1306_WriteString(angle, Font_6x8, SSD1306_WHITE); // EDIT

        ssd1306_UpdateScreen();

	    }
      
    }

    vTaskDelay(2);

  }
}

