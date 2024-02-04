 /** 
 **************************************************************
 * @file mylib/s4741858_sysmon.h
 * @author flynn kelly - s4741858
 * @date 05042023
 * @brief Sysmon for operation with analog discovery 2 logic
 * analyzer. FREE RTOS CONFIG??
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


/* FUNCTIONS ---------------------------------------------------------*/
extern void s4741858_sysmon_init();

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
#define S4741858_REG_SYSMON_CHAN0_SET()		SYSMON_CHAN0_GPIO->ODR |= (0x01 << SYSMON_CHAN0_PIN);
#define S4741858_REG_SYSMON_CHAN0_CLR()		SYSMON_CHAN0_GPIO->ODR &= ~(0x01 << SYSMON_CHAN0_PIN);
#define S4741858_REG_SYSMON_CHAN1_SET()		SYSMON_CHAN1_GPIO->ODR |= (0x01 << SYSMON_CHAN1_PIN);
#define S4741858_REG_SYSMON_CHAN1_CLR()		SYSMON_CHAN1_GPIO->ODR &= ~(0x01 << SYSMON_CHAN1_PIN);
#define S4741858_REG_SYSMON_CHAN2_SET()		SYSMON_CHAN2_GPIO->ODR |= (0x01 << SYSMON_CHAN2_PIN);
#define S4741858_REG_SYSMON_CHAN2_CLR()		SYSMON_CHAN2_GPIO->ODR &= ~(0x01 << SYSMON_CHAN2_PIN);


// Task Priorities (Idle Priority is the lowest priority)
#define TASK1_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define TASK2_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define TASK3_PRIORITY					( tskIDLE_PRIORITY + 2 )

// Task Stack Allocations (must be a multiple of the minimal stack size)
#define TASK1_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define TASK2_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define TASK3_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
