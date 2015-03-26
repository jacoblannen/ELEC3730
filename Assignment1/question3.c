/* Jacob Lannen
 * 3162100
 * Assignment 1 question 3 - Program to open a WAV file and retrieve and display its format info
 * 27/3/2015
 */

#include <stdio.h>
#include <stdlib.h>

FILE *fp;

int main(void){
	char address[50];														//String used to retrieve file location from user
	char code[5]={0,0,0,0,0};												//Various format variables
	int file_size;
	int fmt_ck_size;
	int dat_ck_size;
	short format_tag;
	short channels;
	int sample_rate;
	int data_rate;
	short block_allign;
	short bit_rate;
	long int test;
	
	printf("Enter file path/name:\t");										//Retrieve file from user
	gets(address);
	
	fp = fopen(address,"rb");												//Open file in binary read mode and perform error check

	if(fp == 0)
		printf("\nFile could not be opened. Please check that path is correct.\n\n");
	else{
		fread(&code,sizeof(char),4,fp);										//Read and display format info
		printf("Chunk ID: %s\n",code);
		
		fread(&file_size,sizeof(int),1,fp);
		printf("File size: %d bytes.\n",file_size+8);
		
		fseek(fp,0,SEEK_END);												//Check file size matches format, else display error and terminate
		test=ftell(fp);
		if(test!=(file_size+8)){
			printf("File size does not match data.\n");
			fclose(fp);
			return(0);
		}else{
			fseek(fp,8,SEEK_SET);
			
			fread(&code,sizeof(char),4,fp);
			printf("WAVE ID: %s\n",code);
			
			fread(&code,sizeof(char),4,fp);
			printf("Chunk ID: %s\n",code);
			
			fread(&fmt_ck_size,4,1,fp);
			printf("Chunk size: %d bytes.\n",fmt_ck_size);
			
			fread(&format_tag,2,1,fp);										//Check format tag is valid, else display error and terminate
			if(format_tag==0x0001){
				printf("Format ID: PCM\n");
			}else if(format_tag==0x0003){
				printf("Format ID: IEEE float\n");
			}else if(format_tag==0x0006){
				printf("Format ID: 8-bit ITU-T G.711 A-law\n");
			}else if(format_tag==0x0007){
				printf("Format ID: 8-bit ITU-T G.711 µ-law\n");
			}else if(format_tag==0xfffe){
				printf("Format ID: Extensible\n");
			}else{
				printf("Format ID invalid.\n");
				fclose(fp);
				return(0);
			}
		
			fread(&channels,2,1,fp);
			printf("Number of channels: %d\n",channels);
		
			fread(&sample_rate,4,1,fp);
			printf("Sampling rate: %d samples per second\n",sample_rate);
		
			fread(&data_rate,4,1,fp);
			printf("Data rate: %d bytes per second\n",data_rate);
		
			fread(&block_allign,2,1,fp);
			printf("Block allignment: %d byte(s) per block\n",block_allign);
		
			fread(&bit_rate,2,1,fp);
			printf("Bits per sample: %d\n",bit_rate);
		}
	}
		
		
	
	fclose(fp);																//Close file
	
	return(0);
}
