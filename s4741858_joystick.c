 /** 
 **************************************************************
 * @file mylib/s4741858_joystick.c
 * @author flynn kelly - s4741858
 * @date 29022023
 * @brief Driver functions for the joystick periphery as 
 * stipulated in the mylibguide.
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
 *************************************************************** 
 */

#include "s4741858_joystick.h"

//#define FreeRTOS // ENABLES FREERTOS FUNCTIONALITY ----------

// Static variable which is only accessible by this document.
static volatile int count = 0;
#ifdef FreeRTOS
SemaphoreHandle_t s4741858SemaphoreJoystickSig;
#endif


// ADC Handlers (utilising ADC 1 and ADC 2)
ADC_HandleTypeDef AdcHandleX;
ADC_HandleTypeDef AdcHandleY;
ADC_ChannelConfTypeDef AdcChanConfigX;
ADC_ChannelConfTypeDef AdcChanConfigY;


/**
 * @brief Initialises registers and 
 * ADC handler structs for ADC inputs
 */
void s4741858_reg_joystick_init() {
	
	// ENABLE PORT C GPIO CLOCK
	__GPIOC_CLK_ENABLE();

	// PORT C PIN 0 (Board A1) - X Input
	GPIOC->MODER |= (0x03 << (0 * 2));			
	GPIOC->OSPEEDR &= ~(0x03<<(0 * 2));
	GPIOC->OSPEEDR |= 0x02<<(0 * 2);
	GPIOC->PUPDR &= ~(0x03 << (0 * 2));			

	// PORT C PIN 3 (Board A2) - Y Input
	GPIOC->MODER |= (0x03 << (3 * 2));	
	GPIOC->OSPEEDR &= ~(0x03<<(3 * 2));
	GPIOC->OSPEEDR |= 0x02<<(3 * 2);  			
	GPIOC->PUPDR &= ~(0x03 << (3 * 2));

	// CREATE X-AXIS ADC FRAMEWORK
	__ADC1_CLK_ENABLE(); 
	
	AdcHandleX.Instance = (ADC_TypeDef *)(ADC1_BASE);						//Use ADC1 (For PC0)
	AdcHandleX.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;	//Set clock prescaler
	AdcHandleX.Init.Resolution            = ADC_RESOLUTION12b;				//Set 12-bit data resolution
	AdcHandleX.Init.ScanConvMode          = DISABLE;
	AdcHandleX.Init.ContinuousConvMode    = DISABLE;
	AdcHandleX.Init.DiscontinuousConvMode = DISABLE;
	AdcHandleX.Init.NbrOfDiscConversion   = 0;
	AdcHandleX.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;	//No Trigger
	AdcHandleX.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;		//No Trigger
	AdcHandleX.Init.DataAlign             = ADC_DATAALIGN_RIGHT;				//Right align data
	AdcHandleX.Init.NbrOfConversion       = 1;
	AdcHandleX.Init.DMAContinuousRequests = DISABLE;
	AdcHandleX.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&AdcHandleX);		//Initialise ADC

	// Configure ADC Channel 10 (ADC1)
	AdcChanConfigX.Channel = ADC_CHANNEL_10;	//PC0 has Analog Channel 10 connected
	AdcChanConfigX.Rank         = 1;
	AdcChanConfigX.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	AdcChanConfigX.Offset       = 0;

	HAL_ADC_ConfigChannel(&AdcHandleX, &AdcChanConfigX);	//Initialise ADC Channel (x)

	// CREATE Y-AXIS ADC FRAMEWORK
	__ADC2_CLK_ENABLE(); 
	
	AdcHandleY.Instance = (ADC_TypeDef *)(ADC2_BASE);						//Use ADC2 (For PC3)
	AdcHandleY.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;	//Set clock prescaler
	AdcHandleY.Init.Resolution            = ADC_RESOLUTION12b;				//Set 12-bit data resolution
	AdcHandleY.Init.ScanConvMode          = DISABLE;
	AdcHandleY.Init.ContinuousConvMode    = DISABLE;
	AdcHandleY.Init.DiscontinuousConvMode = DISABLE;
	AdcHandleY.Init.NbrOfDiscConversion   = 0;
	AdcHandleY.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;	//No Trigger
	AdcHandleY.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;		//No Trigger
	AdcHandleY.Init.DataAlign             = ADC_DATAALIGN_RIGHT;				//Right align data
	AdcHandleY.Init.NbrOfConversion       = 1;
	AdcHandleY.Init.DMAContinuousRequests = DISABLE;
	AdcHandleY.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&AdcHandleY);		//Initialise ADC

	// Configure ADC Channel 13 (ADC2)
	AdcChanConfigY.Channel = ADC_CHANNEL_13;	//PC3 has Analog Channel 10 connected
	AdcChanConfigY.Rank         = 1;
	AdcChanConfigY.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	AdcChanConfigY.Offset       = 0;

	HAL_ADC_ConfigChannel(&AdcHandleY, &AdcChanConfigY);	//Initialise ADC Channel (Y)
}

/**
 * @brief Returns the values that are input according to which axis
 * is selected (x or y).
 * @param AdcHandle - ADC handler struct input according to which
 * values are desired - x axis or y axis.
 * @return int out of 2048 reflecting position of joystick
 */
int s4741858_reg_joystick_readxy(ADC_HandleTypeDef AdcHandle) {

	int adc_value;

	HAL_ADC_Start(&AdcHandle); // Start ADC conversion

	while (HAL_ADC_PollForConversion(&AdcHandle, 10) != HAL_OK);

		adc_value = AdcHandle.Instance->DR;
	
	return adc_value;

}

/**
 * @brief Initialises all registers for inputs and interrrupts
 */
void s4741858_reg_joystick_pb_init() {

    // Joystick is INPUT for PORT A PIN 3
    __GPIOA_CLK_ENABLE();

    GPIOA->OSPEEDR |= (GPIO_SPEED_FAST << 3);	//Set fast speed.
	GPIOA->PUPDR &= ~(0x03 << (3 * 2));			//Clear bits for no push/pull
	GPIOA->MODER &= ~(0x03 << (3 * 2));			//Clear bits for input mode

    // Enable EXTI clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // INTERRUPT TRIGGER SOURCE (input pin essentially)
    
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA;

    EXTI->RTSR |= EXTI_RTSR_TR3;	//enable rising dedge
	EXTI->FTSR &= ~EXTI_FTSR_TR3;	//disable falling edge
	EXTI->IMR |= EXTI_IMR_IM3;		//Enable external interrupt

    HAL_NVIC_SetPriority(EXTI3_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

/**
 * @brief Interrupt service routine that increments count when called.
 */
void s4741858_reg_joystick_pb_isr() {
	static int ptime = 0;
	static int pressed = 0;

	//DEBOUNCING!
	if (((HAL_GetTick() - ptime) > 100) && (!pressed)) {
    
		pressed = 1;
    	count++;
    	// increment counter

		// FreeRTOS Functio
		#ifdef FreeRTOS

		BaseType_t xHigherPriorityTaskWoken;
		xHigherPriorityTaskWoken = pdFALSE;

		if (s4741858SemaphoreJoystickSig != NULL) {	// Check if semaphore exists 
			xSemaphoreGiveFromISR( s4741858SemaphoreJoystickSig, &xHigherPriorityTaskWoken );		// Give PB Semaphore from ISR
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
 * @brief Activated from interrupt (joystick press), acts to 
 * implicitly call count isr, deals with register bits.
 */
void EXTI3_IRQHandler(void) {

	NVIC_ClearPendingIRQ(EXTI3_IRQn);

	// PR: Pending register
	if ((EXTI->PR & EXTI_PR_PR3) == EXTI_PR_PR3) {

		// cleared by writing a 1 to this bit
		EXTI->PR |= EXTI_PR_PR3;	//Clear interrupt flag.

		s4741858_reg_joystick_pb_isr();   // Callback for A3
	}
}

/**
 * @brief Get function which returns the value of 
 * file static.
 * @return joystick press count static int. 
 */
int s4741858_reg_joystick_press_get() {
    return count;
}

/**
 * @brief Set function which reset count static
 */
void s4741858_reg_joystick_press_reset() {
    count = 0;
}

/* FreeRTOS CODE-----------------------------------------*/

#ifdef FreeRTOS

/**
 * @brief Get function which returns the value of 
 * file static.
 * @return joystick press count static int. 
 */
extern void s4741858_tsk_joystick_init() {

	// Start the joystick control task
  	xTaskCreate( (void *) &s4741858TaskJoystickControl, (const signed char *) "T_JOYSTICK", JOYSTICKTASK_STACK_SIZE, NULL, JOYSTICKTASK_PRIORITY, NULL );

}

/**
 * @brief FreeRTOS task to signal joystick press
 * 
 * Need to worry about enter critical if priority higher??
 */
void s4741858TaskJoystickControl( void ) {

	// Init the relevant hardware - Joystick PB
	s4741858_reg_joystick_pb_init();

	// Create semaphore (declared in .h file)
	
	s4741858SemaphoreJoystickSig = xSemaphoreCreateBinary();

	for (;;) {
		// Wait for interrupt - handled

	}
}

#endif
