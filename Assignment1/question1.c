/* Jacob Lannen
 * 3162100
 * Assignment 1 question 1 - Two functions to return an int from a specified memory address assuming a certain endianness
 * 27/3/2015
 */

#include <stdio.h>
#include <math.h>																									//Included for pow() function

int extract_little(char* str, int offset, int n){
	int temp;																										//Buffer to hold value in current byte before it is modified for output
	int power;																										//Used to hold output of pow() function. Used to set signifigance of current byte
	int output = 0;																									//Output integer
	int i;																											//Loop counter
	
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

int extract_big(char* str, int offset, int n){																		//Function runs the same as the above
	int temp;
	int power;
	int output = 0;
	int i;
	
	if(0>n||n>sizeof(int)){
		printf("Invalid value n. Please ensure n is between 0 and %d.\n",sizeof(int));
		output=-1;
	}else if(0>str){
		printf("Invalid pointer value. Please ensure str points to a valid memory address.\n");
		output=-1;
	}else if(0>offset){
		printf("Invalid offset value. Please use positive offset.\n");
	}else{
		for(i = 0; i < n; i++){
			temp = *(str+offset);
			power=(int)pow((float)256,n-1-i);
			output+= temp*power;
			offset++;
		}
	}
	return(output);
}
