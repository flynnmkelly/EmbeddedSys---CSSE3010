 /** 
 **************************************************************
 * @file mylib/s4741858_joystick.h
 * @author flynn kelly - s4741858
 * @date 05032023
 * @brief Header function for joystick periphery driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4741858_reg_joystick_pb_init() - initialise all the 
 * appropriate registers and interrupts.
 * s4741858_reg_joystick_press_get() - returns current joystick
 * press count value
 * s4741858_reg_joystick_init() -  initialises A1 and A2 pins 
 * in ADC mode and then intilaises ADC handlers.
 * s4741858_reg_joystick_readxy() - reads the input axis ADC
 * and returns the value (out of 2048) as an int.
 * 
 * s4741858_tsk_joystick_init() - initiates task
 *************************************************************** 
 */


/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"

//#define FreeRTOS

#ifdef FreeRTOS
// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif

/* Offset Variables ---------------------------------------------------------*/
#define  S4741858_REG_JOYSTICK_X_ZERO_CAL_OFFSET -5
#define  S4741858_REG_JOYSTICK_Y_ZERO_CAL_OFFSET -35

#ifdef FreeRTOS
/* FreeRTOS Defines -----------------------------------------*/
#define JOYSTICKTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define JOYSTICKTASK_PRIORITY		( tskIDLE_PRIORITY + 2 ) // PLAY WITH

extern SemaphoreHandle_t s4741858SemaphoreJoystickSig;

#endif

/* ADC Handlers --------------------------------------------------------------*/
extern ADC_HandleTypeDef AdcHandleX;
extern ADC_HandleTypeDef AdcHandleY;
extern ADC_ChannelConfTypeDef AdcChanConfigX;
extern ADC_ChannelConfTypeDef AdcChanConfigY;

/* Joystick Position Read Functions -----------------------------------------*/
#define S4741858_REG_JOYSTICK_X_READ() s4741858_reg_joystick_readxy(AdcHandleX)
#define S4741858_REG_JOYSTICK_Y_READ() s4741858_reg_joystick_readxy(AdcHandleY)

/* .c File Functions -----------------------------------------*/
extern void s4741858_reg_joystick_pb_init();
extern void s4741858_reg_joystick_init();
extern int s4741858_reg_joystick_press_get();
extern int s4741858_reg_joystick_readxy();

/* FreeRTOS Functions -----------------------------------------*/
void s4741858TaskJoystickControl( void );
extern void s4741858_tsk_joystick_init();


