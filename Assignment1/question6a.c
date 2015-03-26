/* Jacob Lannen
 * 3162100
 * Assignment 1 question 6a - Program to take keystroke and display hex code of character pressed
 * 27/3/2015
 */

#include <stdio.h>						//Included for printf()
#include <conio.h>						//Included for getch()

int main(void){
	char c;								//Char to be displayed
	
	while(c!=0xd&&c!=0x1b){				//Loop until "esc" or "enter" keystroke
		c = getch();
		printf("%x\n",c);				//Display char as hex value
	}
		
	return(0);
}
