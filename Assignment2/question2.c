/* Jacob Lannen
 * 3162100
 * Assignment 1 Question 1
 * 1/4/2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>														//Included for read/write flags
#include <unistd.h>														//Included for read/write operations
#include <string.h>
#include "ass2.h"														//Included for function prototypes
#include "alt_types.h"
#include "system.h"
#include "io.h"

alt_32 do_ledr(alt_8 no_args, alt_8* arg_strings[]);
alt_32 do_add(alt_8 no_args, alt_8* arg_strings[]);
alt_32 do_switch(alt_8 no_args, alt_8* arg_strings[]);

typedef struct{
		alt_8* com_string;
		alt_32 (*com_fun)(alt_8 argc, alt_8* argv[]);
	}command;

int main()
{
	int uart_read=open("/dev/uart_0",O_RDONLY);							//Open UART with a read variable
	int uart_write=open("/dev/uart_0",O_WRONLY);						//Open UART with a write variable
	char buffer[100];													//Buffer to receive/transmit UART data
	char cmd_string[100];												//String received from UART at carriage return
	char* out_array[50];												//Array of strings used in string_parser()
	int i=0;															//Loop counter
	int arg_count=0;													//Number of input arguments/words received from command string
	int err_check=0;
	const command commandList[]={
			{"ledr",do_ledr},
			{"add",do_add},
			{"switch",do_switch},
			{NULL,NULL}
	};

	buffer[0]=0;

	while(1){
		while(buffer[0]!=0xd){												//Loop until "enter" keystroke
			read(uart_read,buffer,1);										//Read from UART
			write(uart_write,buffer,1);										//Write to UART
			printf("%c",buffer[0]);											//Display on console
			cmd_string[i]=buffer[0];										//Write input to cmd_string
			i++;
		}
		printf("\n");

		cmd_string[i-1]=0;													//Null terminate command string
		arg_count = string_parser(cmd_string,out_array);					//Use string_parser() to separate arguments of cmd_string
		if(arg_count==0){
			write(uart_write,"ERROR: Input string starts with NULL.",37);
		}

		write(uart_write,"\r\n",2);

		for(i=0; commandList[i].com_string!=NULL; i++){
			if(strcmp(commandList[i].com_string, out_array[0])==0){
				err_check = commandList[i].com_fun(arg_count,out_array);
				if(err_check==-1){
					write(uart_write,"ERROR: ledr requires a single argument between 0 and 262143.\r\n",62);
				}
			}
		}
		buffer[0]=0;
		i=0;
	}


  return (0);
}

alt_32 do_ledr(alt_8 no_args, alt_8* arg_strings[]){
	long arg=0;
	char *ptr=0;

	arg = strtol(arg_strings[1],ptr,10);

	if(no_args!=2||arg<0||arg>262143){
		return(-1);
	}else{
		IOWR(LED_RED_BASE, 0, arg&0x3ffff);
	}

	return(0);
}

alt_32 do_add(alt_8 no_args, alt_8* arg_strings[]){
	printf("add\n");
	return(0);
}
alt_32 do_switch(alt_8 no_args, alt_8* arg_strings[]){
	int temp = IORD(SWITCH_PIO_BASE,0);
	int i = 0;
	char string[6]={0,0,0,0,0,0};
	static alt_u8  Map[] = {
			63, 6, 91, 79, 102, 109, 125, 7,
	        127, 111, 119, 124, 57, 94, 121, 113
	};

	sprintf(string, "%0x", temp);
	printf("%s",string);
	for(i=0;i<5;i++){
		IOWR(SEG7_DISPLAY_BASE,i,Map[(string[5-i]-'0')]);
		printf("%d\n",(string[5-i]-'0'));
	}
	return(0);
}
