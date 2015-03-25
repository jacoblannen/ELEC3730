#include <stdio.h>
#include <stdlib.h>
#include "ass1.h"

FILE *fp;

int get_wav_format(void){
	char code[5]={0,0,0,0,0};
	char address[50];
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
	
	printf("Enter file path/name:\t");
	gets(address);
	
	fp = fopen(address,"r");

	if(fp == 0)
		printf("\nFile could not be opened. Please check that path is correct.\n\n");
	else{
		fread(&code,sizeof(char),4,fp);
		printf("Chunk ID: %s\n",code);
		
		fread(&file_size,sizeof(int),1,fp);
		printf("File size: %d bytes.\n",file_size);
		
		fseek(fp,0,SEEK_END);
		test=ftell(fp);
		if(test!=(file_size+8)){
			printf("File size does not match data.\n");
		}else{
			fseek(fp,8,SEEK_SET);
			
			fread(&code,sizeof(char),4,fp);
			printf("WAVE ID: %s\n",code);
			
			fread(&code,sizeof(char),4,fp);
			printf("Chunk ID: %s\n",code);
			
			fread(&fmt_ck_size,4,1,fp);
			printf("Chunk size: %d bytes.\n",fmt_ck_size);
			
			fread(&format_tag,2,1,fp);
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
		
		
	
	fclose(fp);
	
	return(0);
}
