/*
 * question2.c
 *
 *  Created on: 20/05/2015
 *      Author: c3162100
 */

#include "ass3.h"

int main(void){
	int uart_read = open("/dev/uart_0",O_RDONLY);								//Open UART with a read variable
	int uart_write = open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	char buffer[128] = {NULL};													//Buffer to receive/transmit UART data
	char cmd_string[128] = {NULL};												//String received from UART at carriage return
	char* out_array[64];														//Array of strings used in string_parser()
	int i = 0;																	//Counter used to check number of files in directory
	int arg_count = 0;															//Number of input arguments/words received from command string
	int cmd_flag = 0;																//Flag set if command found in list
	euint32 err_check = 0;
	const command commandList[]={												//Array of command line arguments
			{"open",do_open},
			{"play",do_play},
			{"help",do_help},
			{NULL,NULL}
	};

	write(uart_write,"Initialising SD card...\r\n",25);							//Initialise SD card

	err_check = sd_boot();
	if(err_check==0){
			write(uart_write,"SD card correctly initialised.\r\n",33);
	}else{																		//Display appropriate error if SD card cannot be initialised
			write(uart_write,"Could not initialise.\r\n",24);
			return(0);
	}

	if(!AUDIO_Init()) {
		printf("Unable to initialize audio codec\n");
		return(0);
    }
    audio_dev = alt_up_audio_open_dev(AUDIO_NAME);

	write(uart_write,"Opening root directory...\r\n\n",28);
	sd_openDir(root_dir);
	sd_viewDir();

	write(uart_write,"\r\n\n>> ",6);

	while(1){
		i=0;																	//Reset index
		while(buffer[0]!=0xd){													//Loop until "enter" keystroke
			read(uart_read,buffer,1);											//Read from UART
			write(uart_write,buffer,1);											//Write to UART
			if(buffer[0]==0x7F){												//Check for backspace
				if(i!=0){														//If not first char of string decrement index and replace previous entry with null
					i--;
				}
				cmd_string[i]=NULL;
			}else{
				cmd_string[i]=buffer[0];										//Write input to cmd_string
				i++;
			}
		}

		cmd_string[i-1]=NULL;													//Null terminate command string
		arg_count = string_parser(cmd_string,out_array);						//Use string_parser() to separate arguments of cmd_string
		if(arg_count==0){
			write(uart_write,"\r\nERROR: Input string starts with NULL.\r\n>> ",44);
		}
		for(i=0;i<strlen(out_array[0]);i++){
			out_array[0][i]=tolower(out_array[0][i]);
		}

		cmd_flag=0;																//Reset command flag

		for(i=0; commandList[i].com_string!=NULL; i++){							//Compare input to command strings
			if(strcmp(commandList[i].com_string, out_array[0])==0){
				commandList[i].com_fun(arg_count,out_array);					//Run function with command line arguments
				cmd_flag=1;
			}
		}

		if(cmd_flag==0){
			write(uart_write,"\r\nERROR: Invalid command. Use 'help' command to view list of valid commands.",76);
		}

		write(uart_write,"\r\n\n>> ",6);

		buffer[0]=0;															//Clear buffer
	}

    return(0);
}

euint32 do_open(euint8 no_args, euint8* arg_strings[]){							//Function to open directory and list it's contents
	dir_ent addr;
	euint8 attr;
	euint32 start;
	int uart_write = open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable

	addr = sd_getDir(arg_strings[1]);

	start = addr.fileStart;
	attr = addr.fileAttr;

	if((start == 0)||(attr&0x10)==0){
		write(uart_write,"\r\nERROR: Directory not found.\r",30);
		return(0);
	}


	sd_openDir(start);
	sd_viewDir();

	return(0);
}

euint32 do_play(euint8 no_args, euint8* arg_strings[]){
	SD_FILE file;
	dir_ent addr;
	euint8 name[9] = {NULL};
	euint8 ext[4] = {NULL};
	euint8 filename[12] = {NULL};
	euint8 *chp;
	euint32 i = 0;
	euint8 buf[1025] = {NULL};

	int uart_write = open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable

    alt_up_audio_reset_audio_core(audio_dev);

	chp = strtok(arg_strings[1],".");

	while(chp){
		if(i==0){
			strcpy(name,chp);
			i++;
		}else{
			strcpy(ext,chp);
		}
		chp = strtok(NULL,".");
	}
	if(strcmp(ext,"wav")){
		write(uart_write,"\r\nERROR: Only .wav files can be played.\r",40);
		return(0);
	}
	if(strlen(name)<8){
		strncat(name,"       ",8-strlen(name));
	}
	strcpy(filename,name);
	strcat(filename,ext);

	addr = sd_getDir(filename);

	sd_fOpen(&file, addr);

	if((addr.fileStart == 0)||(addr.fileAttr&0x10)!=0){
		write(uart_write,"\r\nERROR: File could not be opened.\r",35);
		return(0);
	}

	playFile(file);

	return(0);
}

euint32 do_help(euint8 no_args, euint8* arg_strings[]){							//Function to display help for available functions
	int uart_write = open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable

	write(uart_write,"\r\n",2);
	write(uart_write,"\topen\t\t<dir>\t\t\t\t\tOpen specified directory and list contents.\r\n",62);
	write(uart_write,"\tplay\t\t<file>\t\t\t\t\tPlay WAV file using DE2 audio codec.\r\n",56);
	write(uart_write,"\thelp\t\t\t\t\t\t\tDisplay this message.\r\n",35);

	return(0);
}
