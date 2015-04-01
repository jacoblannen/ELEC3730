/* Jacob Lannen
 * 3162100
 * Assignment 1 Question 1
 * 1/4/2015
 */

#include <stdio.h>
#include <fcntl.h>														//Included for read/write flags
#include <unistd.h>														//Included for read/write operations
#include <string.h>														//Included for strlen()
#include "ass2.h"

int main()
{
	int uart_read=open("/dev/uart_0",O_RDONLY);							//Open UART with a read variable
	int uart_write=open("/dev/uart_0",O_WRONLY);						//Open UART with a write variable
	char buffer[100];													//Buffer to receive/transmit UART data
	char cmd_string[100];												//String received from UART at carriage return
	char* out_array[50];												//Array of strings used in string_parser()
	int i=0;															//Loop counter
	int arg_count=0;													//Number of input arguments/words received from command string

	while(buffer[0]!=0xd){												//Loop until "enter" keystroke
		read(uart_read,buffer,1);										//Read from UART
		write(uart_write,buffer,1);										//Write to UART
		printf("%c",buffer[0]);											//Display on console
		cmd_string[i]=buffer[0];										//Write input to cmd_string
		i++;
	}

	cmd_string[i-1]=0;													//Null terminate command string
	arg_count = string_parser(cmd_string,out_array);					//Use string_parser() to separate arguments of cmd_string

	for(i=0;i<arg_count;i++){											//Write all arguments to console/PuTTY
		write(uart_write,"\r\n",2);
		write(uart_write,out_array[i],strlen(out_array[i]));
		printf("\n%s",out_array[i]);
	}

  return (0);
}
