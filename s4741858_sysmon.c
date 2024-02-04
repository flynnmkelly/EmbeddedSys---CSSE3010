 /** 
 **************************************************************
 * @file mylib/s4741858_sysmon.c
 * @author flynn kelly - s4741858
 * @date 05042023
 * @brief Sysmon for operation with analog discovery 2 logic
 * analyzer.
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


//System monitor pins
#define SYSMON_CHAN0_PIN		8
#define SYSMON_CHAN0_GPIO		GPIOC
#define SYSMON_CHAN0_GPIO_CLK()	__GPIOC_CLK_ENABLE()

#define SYSMON_CHAN1_PIN		9
#define SYSMON_CHAN1_GPIO		GPIOC
#define SYSMON_CHAN1_GPIO_CLK()	__GPIOC_CLK_ENABLE()

#define SYSMON_CHAN2_PIN		10
#define SYSMON_CHAN2_GPIO		GPIOC
#define SYSMON_CHAN2_GPIO_CLK()	__GPIOC_CLK_ENABLE()

//System monitor macros
#define S4741858_REG_SYSMON_CHAN0_SET()		SYSMON_CHAN0_GPIO->BSRR |= (0x01 << SYSMON_CHAN0_PIN); // BSRR REGISTER STUFF
#define S4741858_REG_SYSMON_CHAN0_CLR()		SYSMON_CHAN0_GPIO->BSRR |= (0x01 << SYSMON_CHAN0_PIN + 16);
#define S4741858_REG_SYSMON_CHAN1_SET()		SYSMON_CHAN1_GPIO->ODR |= (0x01 << SYSMON_CHAN1_PIN);
#define S4741858_REG_SYSMON_CHAN1_CLR()		SYSMON_CHAN1_GPIO->ODR &= ~(0x01 << SYSMON_CHAN1_PIN);
#define S4741858_REG_SYSMON_CHAN2_SET()		SYSMON_CHAN2_GPIO->ODR |= (0x01 << SYSMON_CHAN2_PIN);
#define S4741858_REG_SYSMON_CHAN2_CLR()		SYSMON_CHAN2_GPIO->ODR &= ~(0x01 << SYSMON_CHAN2_PIN);




/*
 * Hardware Initialisation - modular within task
 */
void s4741858_sysmon_init( void ) {

	taskENTER_CRITICAL();	//Stop any interruption of the critical section

	BRD_LEDInit();				//Initialise Green LED
	BRD_LEDGreenOff();			//Turn off Green LED
	BRD_LEDRedOff();
	BRD_LEDBlueOff();

	// Enable the GPIO G Clock
  	SYSMON_CHAN0_GPIO_CLK();

  	//Initialise G9 as an output.
  	SYSMON_CHAN0_GPIO->MODER &= ~(0x03 << (SYSMON_CHAN0_PIN * 2));  //clear bits
  	SYSMON_CHAN0_GPIO->MODER |= (0x01 << (SYSMON_CHAN0_PIN * 2));   //Set for push pull

  	SYSMON_CHAN0_GPIO->OSPEEDR &= ~(0x03<<(SYSMON_CHAN0_PIN * 2));
	  SYSMON_CHAN0_GPIO->OSPEEDR |=   0x02<<(SYSMON_CHAN0_PIN * 2);  // Set for Fast speed

  	SYSMON_CHAN0_GPIO->OTYPER &= ~(0x01 << SYSMON_CHAN0_PIN);       //Clear Bit for Push/Pull utput

  	// Activate the Pull-up or Pull down resistor for the current IO
  	SYSMON_CHAN0_GPIO->PUPDR &= ~(0x03 << (SYSMON_CHAN0_PIN * 2));   //Clear Bits
  	SYSMON_CHAN0_GPIO->PUPDR |= ((0x01) << (SYSMON_CHAN0_PIN * 2));  //Set for Pull down output
  
	  SYSMON_CHAN1_GPIO_CLK();

  	//Initialise G9 as an output.
  	SYSMON_CHAN1_GPIO->MODER &= ~(0x03 << (SYSMON_CHAN1_PIN * 2));  //clear bits
  	SYSMON_CHAN1_GPIO->MODER |= (0x01 << (SYSMON_CHAN1_PIN * 2));   //Set for push pull

  	SYSMON_CHAN1_GPIO->OSPEEDR &= ~(0x03<<(SYSMON_CHAN1_PIN * 2));
	  SYSMON_CHAN1_GPIO->OSPEEDR |=   0x02<<(SYSMON_CHAN1_PIN * 2);  // Set for Fast speed

  	SYSMON_CHAN1_GPIO->OTYPER &= ~(0x01 << SYSMON_CHAN1_PIN);       //Clear Bit for Push/Pull utput

  	// Activate the Pull-up or Pull down resistor for the current IO
  	SYSMON_CHAN1_GPIO->PUPDR &= ~(0x03 << (SYSMON_CHAN1_PIN * 2));   //Clear Bits
  	SYSMON_CHAN1_GPIO->PUPDR |= ((0x01) << (SYSMON_CHAN1_PIN * 2));  //Set for Pull down output

	  SYSMON_CHAN2_GPIO_CLK();

  	//Initialise G9 as an output.
  	SYSMON_CHAN2_GPIO->MODER &= ~(0x03 << (SYSMON_CHAN2_PIN * 2));  //clear bits
  	SYSMON_CHAN2_GPIO->MODER |= (0x01 << (SYSMON_CHAN2_PIN * 2));   //Set for push pull

  	SYSMON_CHAN2_GPIO->OSPEEDR &= ~(0x03<<(SYSMON_CHAN2_PIN * 2));
	  SYSMON_CHAN2_GPIO->OSPEEDR |=   0x02<<(SYSMON_CHAN2_PIN * 2);  // Set for Fast speed

  	SYSMON_CHAN2_GPIO->OTYPER &= ~(0x01 << SYSMON_CHAN2_PIN);       //Clear Bit for Push/Pull utput

  	// Activate the Pull-up or Pull down resistor for the current IO
  	SYSMON_CHAN2_GPIO->PUPDR &= ~(0x03 << (SYSMON_CHAN2_PIN * 2));   //Clear Bits
  	SYSMON_CHAN2_GPIO->PUPDR |= ((0x01) << (SYSMON_CHAN2_PIN * 2));  //Set for Pull down output

	taskEXIT_CRITICAL();
}
