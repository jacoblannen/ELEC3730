#include <stdio.h>
#include <fcntl.h>														//Included for read/write flags
#include <unistd.h>														//Included for read/write operations
#include <string.h>
#include "efs.h"
#include "ls.h"
#include "ass2.h"														//Included for function prototypes

EmbeddedFileSystem efsl;
DirList list;
esint8 ret;

int main()
{
	int uart_read=open("/dev/uart_0",O_RDONLY);							//Open UART with a read variable
	int uart_write=open("/dev/uart_0",O_WRONLY);						//Open UART with a write variable
	char buffer[100];													//Buffer to receive/transmit UART data
	char cmd_string[100];												//String received from UART at carriage return
	char disp_string[100];
	char* out_array[50];												//Array of strings used in string_parser()
	int i=0;															//Counter used to check number of files in directory
	int arg_count=0;													//Number of input arguments/words received from command string
	const command commandList[]={										//Array of command line arguments
			{"del",do_del},
			{"open",do_open},
			{"copy",do_copy},
			{"newdir",do_newdir},
			{NULL,NULL}
	};

	write(uart_write,"Initialising SD card...\r\n",25);
	ret = efs_init(&efsl, "/dev/sda");
	if(ret==0){
		write(uart_write,"SD card correctly initialised.\r\n\n",33);
	}else{
		write(uart_write,"Could not initialise.\r\n\n",24);
		return(0);
	}

	ls_openDir(&list, &efsl.myFs, "/");

	write(uart_write,"Opening root directory...\r\n\n",28);

	while(ls_getNext(&list)==0){
		if(list.currentEntry.Attribute & 0x10){
			sprintf(disp_string,"\tDir:\t %s\n\r",list.currentEntry.FileName);
			write(uart_write,disp_string,strlen(disp_string));
		}else{
			sprintf(disp_string,"\tFile:\t %s\t Size: %d bytes\n\r",list.currentEntry.FileName,list.currentEntry.FileSize);
			write(uart_write,disp_string,strlen(disp_string));
		}
		i++;
	}

	if(i==0){
		write(uart_write,"Directory empty.\r\n",18);
	}

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
				commandList[i].com_fun(arg_count,out_array);				//Run function with command line arguments
			}
		}

		write(uart_write,"\r\n>> ",5);

		buffer[0]=0;														//Clear buffer
		i=0;																//Reset index
	}

	fs_umount(&efsl.myFs);

	return (0);
}

alt_32 do_del(alt_8 no_args, alt_8* arg_strings[]){
	int error_check=0;

	error_check = rmfile(&efsl.myFs,arg_strings[1]);

	return(0);
}

alt_32 do_copy(alt_8 no_args, alt_8* arg_strings[]){
/*	FILE *source_file;
	FILE *dest_file;
	char buffer[1];
	int error_check=0;

	strcat(arg_strings[3],arg_strings[1]);
	strcat(arg_strings[2],arg_strings[1]);
	printf("%s\n",arg_strings[3]);
	printf("%s\n",arg_strings[2]);

	error_check = file_fopen(source_file, &efsl.myFs, arg_strings[2], 'r');
	printf("%d\n",error_check);
	error_check = file_fopen(dest_file, &efsl.myFs, arg_strings[3], 'w');
	printf("%d\n",error_check);

	while(file_read(source_file,1,buffer)){
		file_write(dest_file,1,buffer);
	}

	//file_fclose(source_file);
	//file_fclose(dest_file);
*/	return(0);
}

alt_32 do_newdir(alt_8 no_args, alt_8* arg_strings[]){
	int error_check=0;

	error_check = makedir(&efsl.myFs,arg_strings[1]);

	return(0);
}

alt_32 do_open(alt_8 no_args, alt_8* arg_strings[]){
	int error_check=0;
	int uart_write=open("/dev/uart_0",O_WRONLY);						//Open UART with a write variable
	char disp_string[100];
	int i = 0;

	error_check = ls_openDir(&list,&efsl.myFs,arg_strings[1]);

	write(uart_write,"\r\n",2);

	while(ls_getNext(&list)==0){
		if(list.currentEntry.Attribute & 0x10){
			sprintf(disp_string,"\tDir:\t %s\n\r",list.currentEntry.FileName);
			write(uart_write,disp_string,strlen(disp_string));
		}else{
			sprintf(disp_string,"\tFile:\t %s\t Size: %d bytes\n\r",list.currentEntry.FileName,list.currentEntry.FileSize);
			write(uart_write,disp_string,strlen(disp_string));
		}
		i++;
	}

	if(i==0){
		write(uart_write,"Directory empty.\r\n",18);
	}
	return(0);
}
