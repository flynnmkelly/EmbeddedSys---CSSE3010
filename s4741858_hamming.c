 /** 
 **************************************************************
 * @file mylib/s4741858_hamming.c
 * @author flynn kelly - s4741858
 * @date 17032023
 * @brief Software functions for number processing, involving
 * (7,4) block code with hamming. SOURCE - sourcelib hamming file
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
#include "s4741858_hamming.h"


/**
  * Implement Hamming Code + parity checking - FROM EXAMPLES
  * Hamming code is based on the following generator and parity check matrices
  * G = [ 0 1 1 | 1 0 0 0 ;
  *       1 0 1 | 0 1 0 0 ;
  *       1 1 0 | 0 0 1 0 ;
  *       1 1 1 | 0 0 0 1 ;
  *
  * hence H =
  * [ 1 0 0 | 0 1 1 1 ;
  *   0 1 0 | 1 0 1 1 ;
  *   0 0 1 | 1 1 0 1 ];
  *
  * y = x * G, syn = H * y'
  *
  *
  * NOTE: !! is used to get 1 out of non zeros
  */
unsigned char hamming_hbyte_encoder(unsigned char value) {

	unsigned char d0, d1, d2, d3;
	unsigned char p0 = 0, h0, h1, h2;
	unsigned char z;
	uint16_t out;

	/* extract bits */
	d0 = !!(value & 0x1);
	d1 = !!(value & 0x2);
	d2 = !!(value & 0x4);
	d3 = !!(value & 0x8);

	/* calculate hamming parity bits */
	h0 = d1 ^ d2 ^ d3;
	h1 = d0 ^ d2 ^ d3;
	h2 = d0 ^ d1 ^ d3;

	/* generate out byte without parity bit P0 */
	out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
		(d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

	/* calculate even parity bit */
	for (z = 1; z<8; z++)
		p0 = p0 ^ !!(out & (1 << z));

	out |= p0;

	return(out);

}


/*
 * Implement Hamming Code on a full byte of input
 * This means that 16-bits out output is needed,
 * hence the unsigned short
 */
uint16_t s4741858_lib_hamming_byte_encoder(unsigned char value) {

	uint16_t out;

	/* first encode D0..D3 (first 4 bits),
	 * then D4..D7 (second 4 bits).
	 */
	out = hamming_hbyte_encoder(value & 0xF) |
		(hamming_hbyte_encoder(value >> 4) << 8);

	return(out);

}

/*
 * Decodes a full byte of incoming code into half byte of data.
 */
unsigned char s4741858_lib_hamming_byte_decoder(unsigned char value) {

    unsigned char returnValue;
	uint8_t d3, d2, d1, d0, h2, h1, h0, p0;

	//extract bits
	d3 = (value & 0x80) >> 7;
	d2 = (value & 0x40) >> 6;
	d1 = (value & 0x20) >> 5;
	d0 = (value & 0x10) >> 4;
	h2 = (value & 0x08) >> 3;
	h1 = (value & 0x04) >> 2;
	h0 = (value & 0x02) >> 1;
	p0 = (value & 0x01);

	//check whether there are errors
	uint8_t s0 = d0 ^ d1 ^ d2 ^ h0;
	uint8_t s1 = d0 ^ d1 ^ d3 ^ h1;
	uint8_t s2 = d0 ^ d2 ^ d3 ^ h2;

    //correct and detect SINGLE error
    switch(s0 | (s1 << 1) | (s2 << 2)) {
        case 0: break; // no errors detected - zero syndrome vec
        case 1: h0 = !h0; break;
		case 2: h1 = !h1; break;
		case 3: d1 = !d1; break;
		case 4: h2 = !h2; break;
		case 5: d2 = !d2; break;
		case 6: d3 = !d3; break;
		case 7: d0 = !d0; break;
    }

    // shift values into their respective places
    returnValue = (d3 << 3) | (d2 << 2) | (d1 << 1) | d0;

    return returnValue;
}

/*
 * Check for a parity error in the hamming process
 */
int s4741858_lib_hamming_parity_error(unsigned char value) {
    
    uint8_t d3, d2, d1, d0, h2, h1, h0, p0;

	//extract bits
	d3 = (value & 0x80) >> 7;
	d2 = (value & 0x40) >> 6;
	d1 = (value & 0x20) >> 5;
	d0 = (value & 0x10) >> 4;
	h2 = (value & 0x08) >> 3;
	h1 = (value & 0x04) >> 2;
	h0 = (value & 0x02) >> 1;
	p0 = (value & 0x01);

	//calculate even parity
	uint8_t parity = h0 ^ h1 ^ h2 ^ d0 ^ d1 ^ d2 ^ d3;

    if (parity == p0) { //check for overall parity error
        return 1;
    } else {
        return 0;
    }
}


