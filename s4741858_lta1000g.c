 /** 
 **************************************************************
 * @file mylib/s4741858_lta1000g.c
 * @author flynn kelly - s4741858
 * @date 27022023
 * @brief LED Bar Driver Functions
 * @ref sourcelib/examples/gpio for register assignments.
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4741858_reg_lta1000g_init() - initialises all the registers
 * in output mode for the LED bar.
 * s4741858_reg_lta1000g_write() - writes the input int to
 * the LED bar in binary.
 *************************************************************** 
 */

#include "s4741858_lta1000g.h"

/**
 * @brief Initialise all the registers connected to periphery,
 * which in the case of LED bar is all output pins.
 * 
 */
void s4741858_reg_lta1000g_init() {

    // Enable the GPIO Clock for all relvant output ports (A, B & C)
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();

    // Initialise each of the pins as OUTPUT - starting with C6
    GPIOC->MODER &= ~(0x03 << (6 * 2));  //clear bits
    GPIOC->MODER |= (0x01 << (6 * 2));   //Set for push pull

    GPIOC->OSPEEDR &= ~(0x03<<(6 * 2));
	GPIOC->OSPEEDR |=   0x02<<(6 * 2);  // Set for Fast speed

    GPIOC->OTYPER &= ~(0x01 << 6);       //Clear Bit for Push/Pull Output
    
    GPIOC->PUPDR &= ~(0x03 << (6 * 2));   //Clear Bits
    GPIOC->PUPDR |= ((0x01) << (6 * 2));  //Set for Pull down output

    // B15
    GPIOB->MODER &= ~(0x03 << (15 * 2));  
    GPIOB->MODER |= (0x01 << (15 * 2));
    GPIOB->OSPEEDR &= ~(0x03<<(15 * 2));
	GPIOB->OSPEEDR |=   0x02<<(15 * 2);  
    GPIOB->OTYPER &= ~(0x01 << 15);       
    GPIOB->PUPDR &= ~(0x03 << (15 * 2));   
    GPIOB->PUPDR |= ((0x01) << (15 * 2));  
    
    // B13
    GPIOB->MODER &= ~(0x03 << (13 * 2));  
    GPIOB->MODER |= (0x01 << (13 * 2));
    GPIOB->OSPEEDR &= ~(0x03<<(13 * 2));
	GPIOB->OSPEEDR |=   0x02<<(13 * 2);  
    GPIOB->OTYPER &= ~(0x01 << 13);       
    GPIOB->PUPDR &= ~(0x03 << (13 * 2));   
    GPIOB->PUPDR |= ((0x01) << (13 * 2));

    // B12
    GPIOB->MODER &= ~(0x03 << (12 * 2));  
    GPIOB->MODER |= (0x01 << (12 * 2));
    GPIOB->OSPEEDR &= ~(0x03<<(12 * 2));
	GPIOB->OSPEEDR |=   0x02<<(12 * 2);  
    GPIOB->OTYPER &= ~(0x01 << 12);       
    GPIOB->PUPDR &= ~(0x03 << (12 * 2));   
    GPIOB->PUPDR |= ((0x01) << (12 * 2));

    // A15
    GPIOA->MODER &= ~(0x03 << (15 * 2));  
    GPIOA->MODER |= (0x01 << (15 * 2));
    GPIOA->OSPEEDR &= ~(0x03<<(15 * 2));
	GPIOA->OSPEEDR |=   0x02<<(15 * 2);  
    GPIOA->OTYPER &= ~(0x01 << 15);       
    GPIOA->PUPDR &= ~(0x03 << (15 * 2));   
    GPIOA->PUPDR |= ((0x01) << (15 * 2));

    // C7
    GPIOC->MODER &= ~(0x03 << (7 * 2));  
    GPIOC->MODER |= (0x01 << (7 * 2));
    GPIOC->OSPEEDR &= ~(0x03<<(7 * 2));
	GPIOC->OSPEEDR |=   0x02<<(7 * 2);  
    GPIOC->OTYPER &= ~(0x01 << 7);       
    GPIOC->PUPDR &= ~(0x03 << (7 * 2));   
    GPIOC->PUPDR |= ((0x01) << (7 * 2));

    // B5
    GPIOB->MODER &= ~(0x03 << (5 * 2));  
    GPIOB->MODER |= (0x01 << (5 * 2));
    GPIOB->OSPEEDR &= ~(0x03<<(5 * 2));
	GPIOB->OSPEEDR |=   0x02<<(5 * 2);  
    GPIOB->OTYPER &= ~(0x01 << 5);       
    GPIOB->PUPDR &= ~(0x03 << (5 * 2));   
    GPIOB->PUPDR |= ((0x01) << (5 * 2));

    // B3
    GPIOB->MODER &= ~(0x03 << (3 * 2));  
    GPIOB->MODER |= (0x01 << (3 * 2));
    GPIOB->OSPEEDR &= ~(0x03<<(3 * 2));
	GPIOB->OSPEEDR |=   0x02<<(3 * 2);  
    GPIOB->OTYPER &= ~(0x01 << 3);       
    GPIOB->PUPDR &= ~(0x03 << (3 * 2));   
    GPIOB->PUPDR |= ((0x01) << (3 * 2));

    // A4
    GPIOA->MODER &= ~(0x03 << (4 * 2));  
    GPIOA->MODER |= (0x01 << (4 * 2));
    GPIOA->OSPEEDR &= ~(0x03<<(4 * 2));
	GPIOA->OSPEEDR |=   0x02<<(4 * 2);  
    GPIOA->OTYPER &= ~(0x01 << 4);       
    GPIOA->PUPDR &= ~(0x03 << (4 * 2));   
    GPIOA->PUPDR |= ((0x01) << (4 * 2));

    // B4
    GPIOB->MODER &= ~(0x03 << (4 * 2));  
    GPIOB->MODER |= (0x01 << (4 * 2));
    GPIOB->OSPEEDR &= ~(0x03<<(4 * 2));
	GPIOB->OSPEEDR |=   0x02<<(4 * 2);  
    GPIOB->OTYPER &= ~(0x01 << 4);       
    GPIOB->PUPDR &= ~(0x03 << (4 * 2));   
    GPIOB->PUPDR |= ((0x01) << (4 * 2));

}

/**
 * @brief Sets 1 or 0 to LED bar segment by writing to output GPIO pin
 * @param segment The segment of LED bar (0 - 9)
 * @param value Whether segment is high (1) or low (0)s
 */
void lta1000g_seg_set(int segment, unsigned char value) {
    
    unsigned char segmentValue = (value > 0) ? 1 : 0; //use ternary operator to set 1 or 0
    
    switch(segment) { // use switch statement for each of the segments
        case 0:
            if (segmentValue == 1){
                GPIOC->ODR |= (0x01 << 6);
            }
            else {
                GPIOC->ODR &= ~(0x01 << 6);
            }
        case 1:
            if (segmentValue == 1){
                GPIOB->ODR |= (0x01 << 15);
            }
            else {
                GPIOB->ODR &= ~(0x01 << 15);
            }
        case 2:
            if (segmentValue == 1){
                GPIOB->ODR |= (0x01 << 13);
            }
            else {
                GPIOB->ODR &= ~(0x01 << 13);
            }
        case 3:
            if (segmentValue == 1){
                GPIOB->ODR |= (0x01 << 12);
            }
            else {
                GPIOB->ODR &= ~(0x01 << 12);
            }
        case 4:
            if (segmentValue == 1){
                GPIOA->ODR |= (0x01 << 15);
            }
            else {
                GPIOA->ODR &= ~(0x01 << 15);
            }
        case 5:
            if (segmentValue == 1){
                GPIOC->ODR |= (0x01 << 7);
            }
            else {
                GPIOC->ODR &= ~(0x01 << 7);
            }
        case 6:
            if (segmentValue == 1){
                GPIOB->ODR |= (0x01 << 5);
            }
            else {
                GPIOB->ODR &= ~(0x01 << 5);
            }
        case 7:
            if (segmentValue == 1){
                GPIOB->ODR |= (0x01 << 3);
            }
            else {
                GPIOB->ODR &= ~(0x01 << 3);
            }
        case 8:
            if (segmentValue == 1){
                GPIOA->ODR |= (0x01 << 4);
            }
            else {
                GPIOA->ODR &= ~(0x01 << 4);
            }
        case 9:
            if (segmentValue == 1){
                GPIOB->ODR |= (0x01 << 4);
            }
            else {
                GPIOB->ODR &= ~(0x01 << 4);
            }      
    }
}

/**
 * @brief Writes integer values in binary to the LED bar through
 * bit iteration and declaration of set_segment function.
 * @param value number to be displayed in binary
 */
void s4741858_reg_lta1000g_write(unsigned short value) {

    for (int i = 0; i < 10; i++) {
        lta1000g_seg_set(i, (value >> i) & 1);
    }
}

/**
 * @brief Conditional bit inverting depending on the mask used
 * @param value number to be inverted
 * @param mask number to mask the orginal value
 */
void s4741858_reg_lta1000g_invert(unsigned short value, unsigned short mask) {
    unsigned short out = value ^ mask;
    s4741858_reg_lta1000g_write(out);
}


