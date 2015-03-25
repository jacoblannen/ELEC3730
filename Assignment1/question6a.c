#include <stdio.h>
#include <conio.h>
#include "ass1.h"

int keyboard_codes(void){
	char c;
	
	while(1){
		c = getch();
		printf("%x\n",c);
	}
		
	return(0);
}
