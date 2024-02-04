 /** 
 **************************************************************
 * @file mylib/s4741858_rgb.h
 * @author flynn kelly - s4741858
 * @date 05032023
 * @brief RGB header file with both colour creation and 
 * PWM capabilities (hardware logic circuit), importantly
 * includes macro functions.
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

#include "board.h"
#include "processor_hal.h"

/* Colour Functions ---------------------------------------------------------*/
#define S4741858_REG_RGB_BLACK() s4741858_reg_rgb_colour(0x00)
#define S4741858_REG_RGB_WHITE() s4741858_reg_rgb_colour(0x07)
#define S4741858_REG_RGB_RED() s4741858_reg_rgb_colour(0x04)
#define S4741858_REG_RGB_GREEN() s4741858_reg_rgb_colour(0x02)
#define S4741858_REG_RGB_BLUE() s4741858_reg_rgb_colour(0x01)
#define S4741858_REG_RGB_YELLOW() s4741858_reg_rgb_colour(0x06)
#define S4741858_REG_RGB_CYAN() s4741858_reg_rgb_colour(0x03)
#define S4741858_REG_RGB_MAGENTA() s4741858_reg_rgb_colour(0x05)

/* PWM Control Variables ---------------------------------------------------------*/
#define TIMER_COUNTER_FREQ  100000  //CLOCK COUNTER Frequency
#define PWM_PERIOD_SECONDS  0.01    // PERIOD OF PWM (1/Freq) - 100Hz is desired                
#define PWM_PERIOD_TICKS    1000    // Number of ticks per period
#define TICK_DUTYCYCLE(value) (uint16_t)(value*PWM_PERIOD_TICKS/100) //Mapping a percentage to duty cycle
#define PWM_DUTYCYCLE_CHANGE(value) 			TIM1->CCR1=(uint16_t)value //Changes CCR1 Value accordingly


void s4741858_reg_rgb_init();
void s4741858_reg_rgb_colour(unsigned char rgb_mask);
void s4741858_reg_rgb_brightness_write(int level);
int s4741858_reg_rgb_brightness_read();
