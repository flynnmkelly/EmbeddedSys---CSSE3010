 /** 
 **************************************************************
 * @file mylib/s4741858_boardpb.c
 * @author flynn kelly - s4741858
 * @date 21032023
 * @brief Driver functions for the nucleo board pushbutton.
 *************************************************************** 
  * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4741858_reg_board_pb_init() - Initialises relevant port and
 * pin for user button on nucleo board (interrupt enable)
 * s4741858_reg_boardpb_status_get() - Returns the toggle value
 * from the pushbutton (essentially pressed isr).
 *************************************************************** 
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"
#include "s4741858_boardpb.h"

// Static variable which is only accessible by this document.
static volatile int operatingMode = ENCODE_MODE; 

#ifdef FreeRTOS
extern SemaphoreHandle_t s4741858SemaphorePBSig;
#endif


/**
 * @brief Initialises all registers for inputs and interrrupts
 */
void s4741858_reg_board_pb_init() {

     // Initialise onboard pushbutton (PC13) - MAKE MYLIB FILE
     __GPIOC_CLK_ENABLE();

    GPIOC->OSPEEDR |= (GPIO_SPEED_FAST << 13);	//Set fast speed.
	GPIOC->PUPDR &= ~(0x03 << (13 * 2));			//Clear bits for no push/pull
	GPIOC->MODER &= ~(0x03 << (13 * 2));			//Clear bits for input mode

    // Enable EXTI clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//select trigger source (port c, pin 13) on EXTICR4.
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	EXTI->RTSR |= EXTI_RTSR_TR13;	//enable rising dedge
	EXTI->FTSR &= ~EXTI_FTSR_TR13;	//disable falling edge
	EXTI->IMR |= EXTI_IMR_IM13;		//Enable external interrupt

	//Enable priority (10) and interrupt callback. Do not set a priority lower than 5.
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/**
 * @brief Interrupt service routine that toggles encode/decode.
 */
void s4741858_reg_board_pb_isr() {
	
	static int ptime = 0;
	static int pressed = 0;

    // DEBOUNCING and MODE TOGGLE

	if (((HAL_GetTick() - ptime) > 200) && (!pressed)) {
        
		pressed = 1;

        if(operatingMode == ENCODE_MODE) {
            operatingMode = DECODE_MODE;
        } else {
            operatingMode = ENCODE_MODE;
        }

		// FreeRTOS Interrupt
		#ifdef FreeRTOS

		BaseType_t xHigherPriorityTaskWoken;
		xHigherPriorityTaskWoken = pdFALSE;

		if (s4741858SemaphorePBSig != NULL) {	// Check if semaphore exists 
			xSemaphoreGiveFromISR( s4741858SemaphorePBSig, &xHigherPriorityTaskWoken );		// Give PB Semaphore from ISR
		}

		// Perform context switching, if required.
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		
		#endif

	} else {
		pressed = 0;
	}
	ptime = HAL_GetTick();
}

/**
 * @brief Get function which returns the value of 
 * interrupt triger static.
 * @return operating mode variable. 
 */
int s4741858_reg_boardpb_status_get() {
    return operatingMode;
}

/*
 * Interrupt handler (ISR) for EXTI 15 to 10 IRQ Handler
 * Note ISR should only execute a callback
 */ 
void EXTI15_10_IRQHandler(void) {

	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);

	// PR: Pending register
	if ((EXTI->PR & EXTI_PR_PR13) == EXTI_PR_PR13) {

		// cleared by writing a 1 to this bit
		EXTI->PR |= EXTI_PR_PR13;	//Clear interrupt flag.

		s4741858_reg_board_pb_isr();   // ISR function

	}
}

