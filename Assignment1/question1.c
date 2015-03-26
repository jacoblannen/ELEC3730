#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int extract_little(char* str, int offset, int n){
	int temp;
	int power;
	int output = 0;
	int i;
	
	if(n>sizeof(int)){
		printf("String larger than maximum size for int (%d bytes)",sizeof(int));
		output=-1;
	}else{
		for(i = 0; i < n; i++){
			temp = *(str+offset);
			power=(int)pow((float)256,i);
			output+= temp*power;
			offset++;
		}
	}
	return(output);
}

int extract_big(char* str, int offset, int n){
	int temp;
	int power;
	int output = 0;
	int i;
	
	if(n>sizeof(int)){
		printf("String larger than maximum size for int (%d bytes)",sizeof(int));
		output=-1;
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
