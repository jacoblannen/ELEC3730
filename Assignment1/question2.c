#include <stdio.h>
#include <stdlib.h>
#include "ass1.h"

FILE *fp;

int get_data(void)
{
	int	count;
	int	size_check=0;
	double *arr;
	int i;
	char address[100];
	
	printf("Enter file path/name:\t");
	gets(address);
	
	fp = fopen(address,"r");

	if(fp == 0)
		printf("\nFile could not be opened. Please check that path is correct.\n\n");
	else{
		fread(&count,sizeof(int),1,fp);
		printf("\nNumber of floats in file: %i\n\n",count);

		arr =(double*)malloc(count*sizeof(double));
		
		fseek(fp,0,SEEK_END);
		size_check = ftell(fp);
		
		if(size_check != ((count*8)+4)){
			printf("\nFile size does not match indicated size.\n\n");
		}else{
			for(i = 0; i<count;i++){
				printf("%lf\n",arr[i]);
			}
		}
		
		
		free(arr);
		fclose(fp);
	}
	return(0);
}

