/* Jacob Lannen
 * 3162100
 * Assignment 1 question 6b - Program to run on DE2 board. Collects data from UART, displays on console, and returns data back through UART
 * 27/3/2015
 */

#include <stdio.h>
#include <fcntl.h>									//Included for read/write flags
#include <unistd.h>									//Included for read/write operations

int main()
{
	int uart_read=open("/dev/uart_0",O_RDONLY);		//Open UART with a read variable
	int uart_write=open("/dev/uart_0",O_WRONLY);	//Open UART with a write variable
	char buffer[100];								//Buffer to recieve/transmit UART data
	char out[20];

	while(buffer[0]!=0xd&&buffer[0]!=0x1b){			//Loop until "esc" or "enter" keystroke
		read(uart_read,buffer,1);					//Read from UART
		printf("%c",buffer[0]);						//Display on console
		sprintf(out,"%c\t%x\r\n",buffer[0],buffer[0]);
		write(uart_write,out,strlen(out));					//Write to UART
	}

  return (0);
}
