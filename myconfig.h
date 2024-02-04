/** 
 **************************************************************
 * @file mylib/myconfig.h
 * @author flynn kelly - s4741858
 * @date 28042023
 * @brief Essentially a variable storing file for different 
 * machine configuraitons on ASC system.
 *
 ***************************************************************
 **/
#include <stdio.h>
#include <stdlib.h>

#define MYRADIOCHAN 58


uint8_t myradiotxaddr[5] = {0x30, 0x10, 0x00, 0x00, 0x58};


//XENON TEST MONITOR ADDRESS

// #define MYRADIOCHAN 40
// uint8_t myradiotxaddr[5] = {0x12, 0x34, 0x56, 0x78, 0x90};
