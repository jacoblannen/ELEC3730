/* Jacob Lannen
 * 3162100
 * Assignment 1 question 2 - Program to read an int n and n doubles from a binary file
 * 27/3/2015
 */

#include <stdio.h>
#include <stdlib.h>

FILE *fp;														//Pointer to input file

int main(void)
{
	int	count;													//Variable used to store the number of doubles to be read
	int	size_check=0;											//Variable used to ensure data file size is correct
	double *arr;												//Pointer to dynamic array of doubles to be used to store data
	int i;														//Loop counter
	char address[50];											//String used to retrieve input file location from user
	
	printf("Enter file path/name:\t");							//Get input file from user and open in binary read mode
	gets(address);
	fp = fopen(address,"rb");

	if(fp == 0)													//Error check fopen() and tell user if file failed to be opened/found
		printf("\nFile could not be opened. Please check that path is correct.\n\n");
	else{
		fread(&count,sizeof(int),1,fp);							//Read int from file
		printf("\nNumber of floats in file: %i\n\n",count);

		arr =malloc(count*sizeof(double));						//malloc() array of correct size for data and read in data
		fread(arr,sizeof(double),count,fp);
		
		fseek(fp,0,SEEK_END);									//Error check size of file
		size_check = ftell(fp);
		
		if(size_check != ((count*8)+4)){						//If file size in not expected size display error
			printf("\nFile size does not match indicated size.\n\n");
		}else{													//If no error print array of doubles
			for(i = 0; i<count;i++){
				printf("%lf\n",arr[i]);
			}
		}
		
		free(arr);												//Free memory used for arr
		fclose(fp);												//Close input file
	}
	return(0);
}

