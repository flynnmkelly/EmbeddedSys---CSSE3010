#ifndef OLED_H
#define OLED_H
 /** 
 **************************************************************
 * @file mylib/s4741858_oled.h
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
#include <stdio.h>

#include "oled_pixel.h" // OLED STUFF
#include "oled_string.h"
#include "fonts.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* i2c Defines ---------------------------------------------------------*/
#define I2C_DEV_SDA_PIN		9
#define I2C_DEV_SCL_PIN		8
#define I2C_DEV_GPIO		GPIOB
#define I2C_DEV_GPIO_AF 	GPIO_AF4_I2C1
#define I2C_DEV_GPIO_CLK()	__GPIOB_CLK_ENABLE()

#define I2C_DEV				I2C1
#define I2C_DEV_CLOCKSPEED 	100000

/* FreeRTOS Defines -----------------------------------------*/
#define OLEDTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define OLEDTASK_PRIORITY		( tskIDLE_PRIORITY + 5 ) // Higher priority due to latency


// Define a queue message struct - STAGE 4
typedef struct {
    char* string;
    int cursorLocation;
} OLED_Message;

// Project queue mesage struct
typedef struct {
    char* string;
    int cursorXLocation;
    int cursorYLocation;
    int z;
    int angle;
} OLED_ASCMessage;

extern QueueHandle_t s4741858QueueOLEDMessage; // global define.

/* FUNCTIONS ---------------------------------------------------------*/
void s4741858_reg_oled_init();
void s4741858_reg_oled_asc_grid_init();
void s4741858TaskOLEDControl( void ) ;
extern void s4741858_tsk_oled_init();

#endif
