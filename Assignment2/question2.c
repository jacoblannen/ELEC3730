/* Jacob Lannen
 * 3162100
 * Assignment 1 Question 2
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
	int err_check=0;
	const command commandList[]={
			{"ledr",do_ledr},
			{"add",do_add},
			{"switch",do_switch},
			{NULL,NULL}
	};

	LCD_Init();
	buffer[0]=0;
	write(uart_write,">> ",3);

	while(1){
		while(buffer[0]!=0xd){												//Loop until "enter" keystroke
			read(uart_read,buffer,1);										//Read from UART
			write(uart_write,buffer,1);										//Write to UART
			printf("%c",buffer[0]);											//Display on console
			if(buffer[0]==0x7F){
				if(i!=0){
					i--;
				}
				cmd_string[i]=NULL;
			}else{
				cmd_string[i]=buffer[0];										//Write input to cmd_string
				i++;
			}
		}
		printf("\n");

		cmd_string[i-1]=NULL;													//Null terminate command string
		arg_count = string_parser(cmd_string,out_array);					//Use string_parser() to separate arguments of cmd_string
		if(arg_count==0){
			write(uart_write,"ERROR: Input string starts with NULL.",37);
		}

		write(uart_write,"\r\n>> ",5);

		for(i=0; commandList[i].com_string!=NULL; i++){
			if(strcmp(commandList[i].com_string, out_array[0])==0){
				err_check = commandList[i].com_fun(arg_count,out_array);
				switch(err_check){
					case ERR_NO_1:
						write(uart_write,"ERROR: ledr requires a single argument between 0 and 262143.\r\n",62);
						break;
					case ERR_NO_2:
						write(uart_write,"ERROR: Sum too large to be displayed.",37);
						break;
				}
			}
		}
		buffer[0]=0;
		i=0;
	}


  return (0);
}

alt_32 do_ledr(alt_8 no_args, alt_8* arg_strings[]){
	int arg=0;

	arg = atoi(arg_strings[1]);

	if(no_args!=2||arg<0||arg>262143){
		return(ERR_NO_1);
	}else{
		IOWR(LED_RED_BASE, 0, arg&0x3ffff);
	}

	return(0);
}

alt_32 do_add(alt_8 no_args, alt_8* arg_strings[]){
	int i=1;
	long long sum=0;
	char disp_str[17];

	LCD_Init();

	for(i=1;i<no_args;i++){
		sum+= (atoll(arg_strings[i]));
		printf("\n%lld",sum);
	}

	if(sum>9999999999999999){
		return(ERR_NO_2);
	}

	sprintf(disp_str,"%lld",sum);

	LCD_Show_Text(disp_str);

	return(0);
}

alt_32 do_switch(alt_8 no_args, alt_8* arg_strings[]){
	int input = IORD(SWITCH_PIO_BASE,0);
	int i = 0;
	int temp=0;
	static alt_u8  Map[] = {
			63, 6, 91, 79, 102, 109, 125, 7,
	        127, 111, 119, 124, 57, 94, 121, 113
	};

	for(i=0;i<5;i++){
		temp = (input>>(i*4))&0xf;
		IOWR(SEG7_DISPLAY_BASE,i,Map[temp]);
	}
	return(0);
}
