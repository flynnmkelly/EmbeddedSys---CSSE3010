 /** 
 **************************************************************
 * @file mylib/s4741858_rgb.c
 * @author flynn kelly - s4741858
 * @date 05032023
 * @brief RGB driver functions with both colour creation and 
 * PWM capabilities (hardware logic circuit).
 *
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4741858_reg_rgb_init() - initialises all the GPIO ports to
 * appropriate values and initialises PWM mode for D6.
 * s4741858_reg_rgb_colour() - utilises bitmask to set colour
 * combinations of RGB
 * s4741858_reg_rgb_brightness_write() - sets CCR register to
 * achieve desired brightness (through PWM)
 * s4741858_reg_rgb_brightness_read() - reads the CCR register
 * value to determine and return the brightness level
 *************************************************************** 
 **/


#include "s4741858_rgb.h"

/**
 * @brief Sets RGB LED to desired colour according to bit
 * combination; eg rgb_mask 0x01 = colour green
 * @param rgb_mask Bit mask with according to 
 * <RED> <GREEN> <BLUE>
 */
void s4741858_reg_rgb_colour(unsigned char rgb_mask) {
    // 0th bit -> blue
    int blueBit = (rgb_mask & (1 << 0)) >> 0;
    if (blueBit == 1){
        GPIOE->ODR |= (0x01 << 13);
    }
    else {
        GPIOE->ODR &= ~(0x01 << 13);
    }

    int greenBit = (rgb_mask & (1 << 1)) >> 1;
    if (greenBit == 1){
        GPIOF->ODR |= (0x01 << 14);
    }
    else {
        GPIOF->ODR &= ~(0x01 << 14);
    }

    int redBit = (rgb_mask & (1 << 2)) >> 2;
    if (redBit == 1){
        GPIOE->ODR |= (0x01 << 11);
    }
    else {
        GPIOE->ODR &= ~(0x01 << 11);
    }
    
}

/**
 * @brief Sets CCR register value through the use
 * of macro functions to desired PWM duty cycle.
 * @param level percent PWM duty cycle desired.
 *  
 */
void s4741858_reg_rgb_brightness_write(int level) {

    int dutyCyclePercent = level / 2;

    PWM_DUTYCYCLE_CHANGE(TICK_DUTYCYCLE(dutyCyclePercent));
}

/**
 * @brief Read the PWM duty cycle percentage (brightness level)
 * directly from the CCR register: the function factors in
 * spec sheet maximum pulse width.
 * @return integer brightness percent - to be outputted to 
 * the LED bar.
 */
int s4741858_reg_rgb_brightness_read(void) {

    int regValue = TIM1->CCR1; // access register where PWM duty cycle determined
    int brightnessPercent = regValue / 5; //scales accordingly to established duty cycles

    return brightnessPercent;
}

/**
 * @brief Initialise all GPIO output ports for each
 * of the colours. Also places board D6 in alternate
 * function (PWM) mode.
 * 
 */
void s4741858_reg_rgb_init() {
    // Enable the GPIO Clock for Port E and F - output
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();

    // Initialise regular output ports (red/blue/green)
    // PORT E PIN 11 - Red (Board D5)
    GPIOE->MODER &= ~(0x03 << (11 * 2));  //clear bits
    GPIOE->MODER |= (0x01 << (11 * 2));   //Set for push pull
    GPIOE->OSPEEDR &= ~(0x03<<(11 * 2));
	GPIOE->OSPEEDR |=   0x02<<(11 * 2);  // Set for Fast speed
    GPIOE->OTYPER &= ~(0x01 << 11);       //Clear Bit for Push/Pull Output
    GPIOE->PUPDR &= ~(0x03 << (11 * 2));   //Clear Bits
    GPIOE->PUPDR |= ((0x01) << (11 * 2));  //Set for Pull down output

    //PORT F PIN 14 - Green (Board D4)
    GPIOF->MODER &= ~(0x03 << (14 * 2));  
    GPIOF->MODER |= (0x01 << (14 * 2));
    GPIOF->OSPEEDR &= ~(0x03<<(14 * 2));
	GPIOF->OSPEEDR |=   0x02<<(14 * 2);  
    GPIOF->OTYPER &= ~(0x01 << 14);       
    GPIOF->PUPDR &= ~(0x03 << (14 * 2));   
    GPIOF->PUPDR |= ((0x01) << (14 * 2)); 

    //PORT E PIN 13 - Blue (Board D3)
    GPIOE->MODER &= ~(0x03 << (13 * 2));  
    GPIOE->MODER |= (0x01 << (13 * 2));
    GPIOE->OSPEEDR &= ~(0x03<<(13 * 2));
	GPIOE->OSPEEDR |=   0x02<<(13 * 2);  
    GPIOE->OTYPER &= ~(0x01 << 13);       
    GPIOE->PUPDR &= ~(0x03 << (13 * 2));   
    GPIOE->PUPDR |= ((0x01) << (13 * 2)); 

    // Brightness Control Register (PWM) - PORT E PIN 9 (Board D6)
    GPIOE->OSPEEDR |= (GPIO_SPEED_FAST << (9 * 2));		//Set fast speed.
	GPIOE->PUPDR &= ~(0x03 << (9 * 2));				//Clear bits for no push/pull
	GPIOE->MODER &= ~(0x03 << (9 * 2));				//Clear bits
	GPIOE->MODER |= (GPIO_MODE_AF_PP << (9 * 2));  	//Set Alternate Function Push Pull Mode
	

    // NEED TO CHECK AFR REGISTERS!
	GPIOE->AFR[1] &= (0x0F << ((9-8) * 4));			//Clear Alternate Function for pin (higher ARF register)
	GPIOE->AFR[1] |= (GPIO_AF1_TIM1 << ((9-8) * 4));	//Set Alternate Function for pin (higher ARF register) 
    
    // Timer 3 clock enable
	__TIM1_CLK_ENABLE();

	/* Compute the prescaler value
	   Set the clock prescaler to 100kHz (COUNT FREQUENCYß)
	   SystemCoreClock is the system clock frequency */
	TIM1->PSC = ((SystemCoreClock) / TIMER_COUNTER_FREQ) - 1;

	// Counting direction: 0 = up-counting, 1 = down-counting (Timer Control Register 1)
	TIM1->CR1 &= ~TIM_CR1_DIR; 

	TIM1->ARR = (PWM_PERIOD_SECONDS*TIMER_COUNTER_FREQ) - 1;
	TIM1->CCR1 = TICK_DUTYCYCLE(25);	//INITIALISATION VALUE - equivalent to 2.5ms p/w

	TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M); 	// Clear OC1M (Channel 1) 
	TIM1->CCMR1 |= (0x6 << 4); 		// Enable PWM Mode 1, upcounting, on Channel 1
	TIM1->CCMR1 |= (TIM_CCMR1_OC1PE); 	// Enable output preload bit for channel 1
	
	TIM1->CR1  |= (TIM_CR1_ARPE); 	// Set Auto-Reload Preload Enable 
	TIM1->CCER |= TIM_CCER_CC1E; 	// Set CC2E Bit
	TIM1->CCER &= ~TIM_CCER_CC1NE; 	// Clear CC2NE Bit for active high output
	
	/* Set Main Output Enable (MOE) bit
	   Set Off-State Selection for Run mode (OSSR) bit
	   Set Off-State Selection for Idle mode (OSSI) bit */
	TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI; 

	TIM1->CR1 |= TIM_CR1_CEN;	// Enable the counter
}



