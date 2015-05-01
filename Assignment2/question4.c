/* Jacob Lannen
 * 3162100
 * Assignment 2 Question 4
 * 27/4/2015
 */
#include <stdio.h>
#include <fcntl.h>																//Included for read/write flags
#include <unistd.h>																//Included for read/write operations
#include <string.h>																//Included for string functions
#include "efs.h"																//Included for EFSL functions
#include "ls.h"																	//Included for EFSL functions
#include "ass2.h"																//Included for function prototypes

//Global Variables
EmbeddedFileSystem efsl;														//File system pointer
DirList list;																	//List pointer
char currentDir[128]={NULL};													//String containing address of last opened directory

int main()
{
	int uart_read=open("/dev/uart_0",O_RDONLY);									//Open UART with a read variable
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	char buffer[128]={NULL};													//Buffer to receive/transmit UART data
	char cmd_string[128]={NULL};												//String received from UART at carriage return
	char disp_string[128]={NULL};												//String used to display data over UART
	char* out_array[64];														//Array of strings used in string_parser()
	int i=0;																	//Counter used to check number of files in directory
	int arg_count=0;															//Number of input arguments/words received from command string
	esint8 ret;																	//Error check for efs_init()
	const command commandList[]={												//Array of command line arguments
			{"del",do_del},
			{"open",do_open},
			{"copy",do_copy},
			{"newdir",do_newdir},
			{"help",do_help},
			{NULL,NULL}
	};

	write(uart_write,"Initialising SD card...\r\n",25);							//Initialise SD card
	ret = efs_init(&efsl, "/dev/sda");
	if(ret==0){
		write(uart_write,"SD card correctly initialised.\r\n\n",33);
	}else{																		//Display appropriate error if SD card cannot be initialised
		write(uart_write,"Could not initialise.\r\n\n",24);
		return(0);
	}

	currentDir[0]='/';															//Initialise current directory to root and open
	write(uart_write,"Opening root directory...\r\n\n",28);
	ls_openDir(&list, &efsl.myFs, currentDir);

	while(ls_getNext(&list)==0){												//List contents of root directory
		if(list.currentEntry.Attribute & 0x10){
			sprintf(disp_string,"\tDir:\t %s\n\r",list.currentEntry.FileName);
			write(uart_write,disp_string,strlen(disp_string));
		}else{
			sprintf(disp_string,"\tFile:\t %s\t Size: %d bytes\n\r",list.currentEntry.FileName,list.currentEntry.FileSize);
			write(uart_write,disp_string,strlen(disp_string));
		}
		i++;
	}

	if(i==0){																	//If root directory empty display appropriate error
		write(uart_write,"Directory empty.\r\n",18);
	}

	write(uart_write,"\r\n>> ",5);

	while(1){
		i=0;																	//Reset index
		ls_openDir(&list, &efsl.myFs, currentDir);								//Re-mount directory
		while(buffer[0]!=0xd){													//Loop until "enter" keystroke
			read(uart_read,buffer,1);											//Read from UART
			write(uart_write,buffer,1);											//Write to UART
			printf("%c",buffer[0]);												//Display on console
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

		for(i=0; commandList[i].com_string!=NULL; i++){							//Compare input to command strings
			if(strcmp(commandList[i].com_string, out_array[0])==0){
				commandList[i].com_fun(arg_count,out_array);					//Run function with command line arguments
			}else{
				write(uart_write,"\r\nERROR: Invalid command. Use 'help' command to view list of valid commands.",76);
			}
		}

		write(uart_write,"\r\n>> ",5);

		buffer[0]=0;															//Clear buffer
	}

	return (0);
}

alt_32 do_del(alt_8 no_args, alt_8* arg_strings[]){								//Function to delete file off SD card
	int error_check=0;
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable

	error_check = rmfile(&efsl.myFs,arg_strings[1]);							//Delete file

	if(error_check){															//If file not deleted display error message
		write(uart_write,"\r\nERROR: File does not exist.\r\n",31);
	}

	fs_umount(&efsl.myFs);														//Unmount/save SD card
	return(0);
}

alt_32 do_copy(alt_8 no_args, alt_8* arg_strings[]){							//Function to copy file from one directory to another on SD card
	FILE source_file;															//Pointer to source file
	FILE dest_file;																//Pointer to new file
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	char buffer[64]={NULL};														//File read/write buffer
	int error_check=0;															//Error check variable

	strcat(arg_strings[3],arg_strings[1]);										//Create filepath strings from file name and directories
	strcat(arg_strings[2],arg_strings[1]);

	error_check = file_fopen(&source_file, &efsl.myFs, arg_strings[2], 'r');	//Open source file and display error if not found
	if(error_check==-1){
		write(uart_write,"\r\nERROR: Source file does not exist.\r\n",38);
	}else{
		error_check = file_fopen(&dest_file, &efsl.myFs, arg_strings[3], 'w');	//Create new file and display error if file of same name already exists
		if(error_check==-2){
			write(uart_write,"\r\nERROR: File of this name already exists in destination directory\r\n",68);
		}else{
			while(error_check=file_read(&source_file,64,buffer)){				//Copy file contents
				file_write(&dest_file,error_check,buffer);
			}
		}
	}

	file_fclose(&source_file);													//Close files
	file_fclose(&dest_file);
	fs_umount(&efsl.myFs);														//Unmount/save SD card
	return(0);
}

alt_32 do_newdir(alt_8 no_args, alt_8* arg_strings[]){							//Function to create new directory
	int error_check=0;															//Error check variable
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable

	error_check = makedir(&efsl.myFs,arg_strings[1]);							//Create directory

	if(error_check==-1){														//Display appropriate error messages
		write(uart_write,"\r\nERROR: Directory already exists.\r\n",36);
	}else if(error_check==-2){
		write(uart_write,"\r\nERROR: Parent directory does not exist.\r\n",43);
	}

	fs_umount(&efsl.myFs);														//Unmount/save SD card
	return(0);
}

alt_32 do_open(alt_8 no_args, alt_8* arg_strings[]){							//Function to open directory and list it's contents
	int error_check=0;															//Error check variable
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	char disp_string[128]={NULL};												//String used to display info via UART
	int i = 0;																	//Counter variable

	memset(currentDir,NULL,128);												//Clear currentDir
	strcpy(currentDir,arg_strings[1]);											//Set currentDir to new directory address

	error_check = ls_openDir(&list,&efsl.myFs,arg_strings[1]);					//Open new directory

	write(uart_write,"\r\n",2);

	if(0==error_check){															//List contents of directory
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
	}else{																		//If directory could not be opened display error message
		write(uart_write,"ERROR: Directory could not be opened.\r\n",39);
	}
	fs_umount(&efsl.myFs);														//Unmount/save SD card
	return(0);
}

alt_32 do_help(alt_8 no_args, alt_8* arg_strings[]){							//Function to display help for available functions
	int uart_write = open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable

	write(uart_write,"\r\n",2);
	write(uart_write,"\topen\t\t<dir>\t\t\t\t\tOpen specified directory and list contents.\r\n",62);
	write(uart_write,"\tnewdir\t\t<dir>\t\t\t\t\tCreate new directory with specified path.\r\n",62);
	write(uart_write,"\tcopy\t\t<filename> <src dir> <dest dir>\t\tCreate a copy of a file.\r\n",66);
	write(uart_write,"\tdel\t\t<filepath>\t\t\t\tDelete specified file.\r\n",44);
	write(uart_write,"\thelp\t\t\t\t\t\t\tDisplay this message.\r\n",35);

	return(0);
}
