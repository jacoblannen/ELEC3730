/* Jacob Lannen
 * 3162100
 * Assignment 1 question 4 - Program to apply an FIR filter to a WAV file using coefficients from a binary data file, and output to a new WAV file.
 * 27/3/2015
 */

#include <stdio.h>
#include <stdlib.h>

FILE *fp;														//File pointer for input files
FILE *fwr;														//File pointer for output file

int main(int argc, char *argv[]){
	char code[5]={0,0,0,0,0};									//String used for checking and writing format codes in files, initialised with null termination
	char address[50];											//String used to retrieve input/output file names from user
	int file_size;												//File size retrieved from header of input file. Used to error check file.
	int fmt_ck_size;											//Size of input file format chunk
	int dat_ck_size;											//Size of input file data chunk. Used in filter loop.
	short format_tag;											//Format tag of input file. Used to check for file error.

	short channels;												//Formatting info of wave files
	int sample_rate;
	int data_rate;
	short block_allign;
	short bit_rate;

	long int size_check;										//Variable used when checking file size matches format.
	char *buffer;												//Pointer to dynamically allocated array of bytes used in filters circular buffer.
	int	no_of_coeff;											//Variable containing number of coefficients found in data file.
	double *arr_of_coeff;										//Pointer to dynamically allocated array of coefficients retrieved from data file.
	char write_byte;											//Variable containing byte to be written to output file.
	double temp;												//Variable used to scale outputof filter back to an appropriate size for storage in single byte.

	int i;														//Variables used in loops/array references
	int j;
	int p=0;
	
	
	printf("Enter data file path/name:\t");						//Retrieve data file address from user
	gets(address);
	
	fp = fopen(address,"rb");									//Open data file in binary read mode

	if(fp == 0){												//Error check fopen to ensure file has opened correcly, ending program if file cannot be opened
		printf("\nFile could not be opened. Please check that path is correct.\n\n");
		return(0);
	}else{														
		fread(&no_of_coeff,sizeof(int),1,fp);					//Read number of coefficients to be found in file
		arr_of_coeff =malloc(no_of_coeff*sizeof(double));		//Allocate space in heap for array of coefficients and read in from file
		fread(arr_of_coeff,sizeof(double),no_of_coeff,fp);
		
		fseek(fp,0,SEEK_END);									//Check file size by comparing address of final byte of file to expected file size
		size_check = ftell(fp);
		
		if(size_check != ((no_of_coeff*8)+4)){					//If file size incorrect display error and end program
			printf("\nFile size does not match indicated size.\n\n");
			fclose(fp);
			return(0);
		}
		
		fclose(fp);												//Close data file
	}
	
	buffer = malloc(no_of_coeff*sizeof(char));					//Allocate space in heap for filter input buffer array, and initialise to zero
	for(i=0;i<no_of_coeff;i++){
		buffer[i]=0;
	}
	
	printf("Enter wave file path/name:\t");						//Retrieve wave file from user and open in binary read mode
	gets(address);
	fp = fopen(address,"rb");

	printf("Enter output wave file path/name:\t");				//Generate output file from user input
	gets(address);
	fwr = fopen(address,"wb");

	if(fp == 0){												//Error check for opening wav file. Display error and end program if file not found.
		printf("\nFile could not be opened. Please check that path is correct.\n\n");
		fclose(fwr);
		return(0);
	}else{
		//fread(&code,sizeof(char),4,fp);							//Read header info from input file and write tonew output file
		//fwrite(&code,sizeof(char),4,fwr);
		fcopy(&code,sizeof(char),4,fp,fwr);
		
		fread(&file_size,sizeof(int),1,fp);
		fwrite(&file_size,sizeof(int),1,fwr);
		file_size+=8;											//Remove offset from file size
		
		fseek(fp,0,SEEK_END);									//Check size of file. If size does not match the format, display error and end program.
		size_check=ftell(fp);
		if(size_check!=(file_size)){
			printf("File size does not match data.\n");
			fclose(fp);
			fclose(fwr);
			return(0);
		}else{													//Return file pointer to previous position, then continue to read/write
			fseek(fp,8,SEEK_SET);
			
			fread(&code,sizeof(char),4,fp);
			fwrite(&code,sizeof(char),4,fwr);
			fread(&code,sizeof(char),4,fp);
			fwrite(&code,sizeof(char),4,fwr);
			fread(&fmt_ck_size,4,1,fp);
			fwrite(&fmt_ck_size,4,1,fwr);
			
			fread(&format_tag,2,1,fp);
			if(format_tag!=0x0001){								//Ensure wav file is PCM format as this program doesnt read non-PCM files
				printf("Format ID not valid for this program.\n");
				fclose(fp);
				fclose(fwr);
				return(0);
			}
			fwrite(&format_tag,2,1,fwr);
		
			fread(&channels,2,1,fp);
			fwrite(&channels,2,1,fwr);
			fread(&sample_rate,4,1,fp);
			fwrite(&sample_rate,4,1,fwr);
			fread(&data_rate,4,1,fp);
			fwrite(&data_rate,4,1,fwr);
			fread(&block_allign,2,1,fp);
			fwrite(&block_allign,2,1,fwr);
			fread(&bit_rate,2,1,fp);
			fwrite(&bit_rate,2,1,fwr);
			fread(&code,sizeof(char),4,fp);
			fwrite(&code,sizeof(char),4,fwr);
			fread(&dat_ck_size,4,1,fp);
			fwrite(&dat_ck_size,4,1,fwr);
						
			for(i=0;i<dat_ck_size;i++){							//Read data chunk and use circular buffer to apply filter
				fread(&buffer[p],1,1,fp);
				temp=0;
				for(j=0;j<no_of_coeff;j++){
					temp+= arr_of_coeff[j]*buffer[(p-j)%no_of_coeff];
				}
				write_byte=(temp/(no_of_coeff));
				fwrite(&write_byte,1,1,fwr);					//Write filtered data to output file
				p=(p+1)%no_of_coeff;
			}
		}
	}
		
		
	free(arr_of_coeff);											//Free malloced space and close files
	free(buffer);
	fclose(fp);
	fclose(fwr);
	
	return(0);
}

int fcopy(void* address, int element_size, int no_of_elements, FILE* in_file, FILE* out_file){
	fread(address,element_size,no_of_elements,in_file);
	fwrite(address,element_size,no_of_elements,out_file);
	
	return(0);
}
