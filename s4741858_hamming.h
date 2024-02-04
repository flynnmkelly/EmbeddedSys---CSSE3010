 /** 
 **************************************************************
 * @file mylib/s4741858_hamming.h
 * @author flynn kelly - s4741858
 * @date 17032023
 * @brief Software functions for number processing, involving
 * (7,4) block code with hamming.
 *************************************************************** 
  * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4741858_lib_hamming_byte_decoder() - takes byte of input and 
 * returns half byte with decoded data bits
 * s4741858_lib_hamming_parity_error() - returns parity error,
 * due to 7,4 cannot be corrected just flagged (return 1)
 * s4741858_lib_hamming_byte_encoder() - takes one byte of input
 * and returns two bytes uint16_t of encoded output
 *************************************************************** 
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"

/* .c File Functions -----------------------------------------*/
extern unsigned char s4741858_lib_hamming_byte_decoder(unsigned char value);
extern int s4741858_lib_hamming_parity_error(unsigned char value);
extern uint16_t s4741858_lib_hamming_byte_encoder(unsigned char value);
