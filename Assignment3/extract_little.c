/*
 * extract_little.c
 *
 *  Created on: 20/05/2015
 *      Author: c3162100
 */
#include "ass3.h"

int extract_little(euint8* str, euint32 offset, euint32 n){
	euint8 temp;																										//Buffer to hold value in current byte before it is modified for output
	euint32 power;																										//Used to hold output of pow() function. Used to set signifigance of current byte
	int output = 0;																									//Output integer
	euint32 i;																											//Loop counter

	if(0>n||n>sizeof(int)){																							//Error check input values
		printf("Invalid value n. Please ensure n is between 0 and %d.\n",sizeof(int));
		output=-1;
	}else if(0>str){
		printf("Invalid pointer value. Please ensure str points to a valid memory address.\n");
		output=-1;
	}else if(0>offset){
		printf("Invalid offset value. Please use positive offset.\n");
		output=-1;
	}else{
		for(i = 0; i < n; i++){																						//Loop through expected number of bytes
			temp = *(str+offset);																					//Move current byte to buffer
			power=(int)pow((float)256,i);																			//Generate bias of byte
			output+= temp*power;																					//Apply bias and add to output
			offset++;																								//Increment offset to look at next byte of data
		}
	}
	return(output);																									//Return output
}
