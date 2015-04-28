/* Jacob Lannen
 * 3162100
 * Assignment 2 Question 2
 * 1/4/2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>														//Included for read/write flags
#include <unistd.h>														//Included for read/write operations
#include <string.h>
#include "alt_types.h"
#include "system.h"
#include "io.h"
#include "ass2.h"														//Included for function prototypes

int main()
{
	int uart_read=open("/dev/uart_0",O_RDONLY);							//Open UART with a read variable
	int uart_write=open("/dev/uart_0",O_WRONLY);						//Open UART with a write variable
	char buffer[100];													//Buffer to receive/transmit UART data
	char cmd_string[100];												//String received from UART at carriage return
	char* out_array[50];												//Array of strings used in string_parser()
	int i=0;															//Loop counter
	int arg_count=0;													//Number of input arguments/words received from command string
	int err_check=0;													//Error check variable
	const command commandList[]={										//Array of command line arguments
			{"ledr",do_ledr},
			{"add",do_add},
			{"switch",do_switch},
			{NULL,NULL}
	};

	LCD_Init();															//Initialise LCD
	buffer[0]=0;														//Initialise UART input buffer
	write(uart_write,">> ",3);											//Display command line prompt

	while(1){
		while(buffer[0]!=0xd){												//Loop until "enter" keystroke
			read(uart_read,buffer,1);										//Read from UART
			write(uart_write,buffer,1);										//Write to UART
			printf("%c",buffer[0]);											//Display on console
			if(buffer[0]==0x7F){											//Check for backspace
				if(i!=0){													//If not first char of string decrement index and replace previous entry with null
					i--;
				}
				cmd_string[i]=NULL;
			}else{
				cmd_string[i]=buffer[0];									//Write input to cmd_string
				i++;
			}
		}

		cmd_string[i-1]=NULL;												//Null terminate command string
		arg_count = string_parser(cmd_string,out_array);					//Use string_parser() to separate arguments of cmd_string
		if(arg_count==0){
			write(uart_write,"ERROR: Input string starts with NULL.\r\n>> ",42);
		}

		for(i=0; commandList[i].com_string!=NULL; i++){						//Compare input to command strings
			if(strcmp(commandList[i].com_string, out_array[0])==0){
				err_check = commandList[i].com_fun(arg_count,out_array);	//Run function with command line arguments
				switch(err_check){											//Display appropriate error messages for returned error values
					case ERR_NO_1:
						write(uart_write,"ERROR: ledr requires a single argument between 0 and 262143.\r\n>> ",65);
						break;
					case ERR_NO_2:
						write(uart_write,"ERROR: Sum too large to be displayed.\r\n>> ",42);
						break;
				}
			}
		}
		buffer[0]=0;														//Clear buffer
		i=0;																//Reset index
	}

	return (0);
}

alt_32 do_ledr(alt_8 no_args, alt_8* arg_strings[]){						//Function to take command line argument and display a number in binary on the red LEDs
	int arg=0;																//Input argument

	arg = atoi(arg_strings[1]);												//Take input from string

	if(no_args!=2||arg<0||arg>262143){										//Check input within appropriate range
		return(ERR_NO_1);
	}else{
		IOWR(LED_RED_BASE, 0, arg&0x3ffff);									//Display on LEDs
	}

	return(0);
}

alt_32 do_add(alt_8 no_args, alt_8* arg_strings[]){							//Function to take arbitrary number of command line arguments and add them displaying sum on LCD
	int i;																	//Index counter
	long long sum=0;														//64-bit int used to sum inputs
	char disp_str[17];														//Output string

	LCD_Init();																//Clear LCD

	for(i=1;i<no_args;i++){													//Sum input arguments
		sum+= (atoll(arg_strings[i]));
	}

	if(sum>LCD_LIMIT){														//Check sum can be displayed on LCD
		return(ERR_NO_2);
	}

	sprintf(disp_str,"%lld",sum);											//Move value into display string

	LCD_Show_Text(disp_str);												//Display result

	return(0);
}

alt_32 do_switch(alt_8 no_args, alt_8* arg_strings[]){						//Function to take the status of the switches on dev board and display as a hex value on 7-seg display
	int input = IORD(SWITCH_PIO_BASE,0);									//Check status of switches
	int i = 0;																//Initialise counter
	int temp=0;																//Initialise temp variable used in bitmask
	static alt_u8  Map[] = {												//Lookup table of values for 7-seg display
			63, 6, 91, 79, 102, 109, 125, 7,
	        127, 111, 119, 124, 57, 94, 121, 113
	};

	for(i=0;i<5;i++){														//Mask each set of four switches and display on appropriate 7-seg display
		temp = (input>>(i*4))&0xf;
		IOWR(SEG7_DISPLAY_BASE,i,Map[temp]);
	}
	return(0);
}
