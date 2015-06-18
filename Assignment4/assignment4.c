#include "ass4.h"

char currentDir[128]={NULL};													//String containing address of last opened directory

void task1(void* pdata)
{
	int uart_read=open("/dev/uart_0",O_NONBLOCK);									//Open UART with a read variable
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	char buffer[128]={NULL};													//Buffer to receive/transmit UART data
	char cmd_string[128]={NULL};												//String received from UART at carriage return
	char disp_string[128]={NULL};												//String used to display data over UART
	char* out_array[64];														//Array of strings used in string_parser()
	int i=0;																	//Counter used to check number of files in directory
	int arg_count=0;															//Number of input arguments/words received from command string
	int cmd_flag=0;																//Flag set if command found in list
	int err_check = 0;
	const command commandList[]={												//Array of command line arguments
			{"del",do_del},
			{"open",do_open},
			{"copy",do_copy},
			{"newdir",do_newdir},
			{"play",do_play},
			{"new",do_new_plist},
			{"add",do_add_plist},
			{"plist",do_play_plist},
			{"help",do_help},
			{NULL,NULL}
	};

	while (1)
	{
		while(buffer[0]!=0xd){												//Loop until "enter" keystroke
			OSTimeDly(10);
			if(read(uart_read,buffer,1)==1){										//Read from UART
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
		}

		cmd_string[i-1]=NULL;												//Null terminate command string
		arg_count = string_parser(cmd_string,out_array);					//Use string_parser() to separate arguments of cmd_string
		if(arg_count==0){
			write(uart_write,"ERROR: Input string starts with NULL.",37);
		}
		for(i=0;i<strlen(out_array[0]);i++){
					out_array[0][i]=tolower(out_array[0][i]);
		}

		cmd_flag=0;

		for(i=0; commandList[i].com_string!=NULL; i++){						//Compare input to command strings
			if(strcmp(commandList[i].com_string, out_array[0])==0){
				err_check = commandList[i].com_fun(arg_count,out_array);	//Run function with command line arguments
				cmd_flag=1;
			}
		}

		if(cmd_flag==0){
			write(uart_write,"\r\nERROR: Invalid command. Use 'help' command to view list of valid commands.",76);
		}

		write(uart_write,"\r\n>> ",5);

		buffer[0]=0;														//Clear buffer
		i=0;																//Reset index
	}
}
void task2(void* pdata)
{
	esint8 err;
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	while (1)
	{
		OSSemPend(PlaySem,0,&err);
		if(trackCount){
			if(currentTrack<trackCount){
				err = file_fopen(&nowPlaying, &efsl.myFs, plist[currentTrack], 'r');	//Open source file and display error if not found
				if(err==-1){
					write(uart_write,"\r\nERROR: File does not exist.\r\n",31);
				}else{
					playFile(&nowPlaying);
					OSSemPost(PlaySem);
					currentTrack++;
				}
			}else{
				currentTrack=0;
			}
		}else{
			playFile(&nowPlaying);
		}
	}
}

void task3(void* pdata)
{
	while (1)
	{
		OSTimeDly(10);
	}
}
/* The main function creates two task and starts multi-tasking */
int main(void)
{
	int uart_read=open("/dev/uart_0",O_RDONLY);									//Open UART with a read variable
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	char disp_string[128]={NULL};												//String used to display data over UART
	int i = 0;
	esint8 ret;																	//Error check for efs_init()

	currentTrack = 0;
	trackCount = 0;

	if(!AUDIO_Init()) {
		printf("Unable to initialize audio codec\n");
		return(0);
    }
    audio_dev = alt_up_audio_open_dev(AUDIO_NAME);

	write(uart_write,"Initialising SD card...\r\n",25);							//Initialise SD card
	ret = efs_init(&efsl, "/dev/sda");
	if(ret==0){
		write(uart_write,"SD card correctly initialised.\r\n\n",33);
	}else{																		//Display appropriate error if SD card cannot be initialised
		write(uart_write,"Could not initialise.\r\n\n",24);
		return(0);
	}

	audio_irq_init();

	currentDir[0] = '/';
	write(uart_write,"Opening root directory...\r\n\n",28);
	ls_openDir(&list, &efsl.myFs, currentDir);

	while(ls_getNext(&list)==0){												//List contents of root directory
		if(list.currentEntry.Attribute & 0x10){
			sprintf(disp_string,"\t%.11s\t<DIR>\n\r",list.currentEntry.FileName);
			write(uart_write,disp_string,strlen(disp_string));
		}else if((list.currentEntry.Attribute & 0x08) == 0){
			sprintf(disp_string,"\t%.11s\t<%d bytes>\n\r",list.currentEntry.FileName,list.currentEntry.FileSize);
			write(uart_write,disp_string,strlen(disp_string));
		}
		i++;
	}

	if(i==0){																	//If root directory empty display appropriate error
		write(uart_write,"Directory empty.\r\n",18);
	}

	write(uart_write,"\r\n>> ",5);

	OSInit();

	OSTaskCreate(task1, (void*)0, (void*) &task1_stk[TASK_STACKSIZE], TASK1_PRIORITY);
	OSTaskCreate(task2, (void*)0, (void*) &task2_stk[TASK_STACKSIZE], TASK2_PRIORITY);
	OSTaskCreate(task3, (void*)0, (void*) &task3_stk[TASK_STACKSIZE], TASK3_PRIORITY);

	PlaySem = OSSemCreate(0);
	AudioSem = OSSemCreate(1);
	OSStart();
	return 0;
}

euint32 do_del(euint8 no_args, euint8* arg_strings[]){								//Function to delete file off SD card
	int error_check=0;
	char tempString[128] = {NULL};
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable

	strcpy(tempString,currentDir);
	strcat(tempString,arg_strings[1]);
	error_check = rmfile(&efsl.myFs,tempString);							//Delete file

	if(error_check){															//If file not deleted display error message
		write(uart_write,"\r\nERROR: File does not exist.\r\n",31);
	}

	fs_umount(&efsl.myFs);														//Unmount/save SD card
	return(0);
}

euint32 do_copy(euint8 no_args, euint8* arg_strings[]){							//Function to copy file from one directory to another on SD card
	FILE source_file;															//Pointer to source file
	FILE dest_file;																//Pointer to new file
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	char buffer[64]={NULL};														//File read/write buffer
	char tempString[128] = {NULL};
	int error_check=0;															//Error check variable

	strcpy(tempString,currentDir);

	strcat(tempString,arg_strings[1]);										//Create filepath strings from file name and directories
	strcat(arg_strings[2],arg_strings[1]);

	error_check = file_fopen(&source_file, &efsl.myFs, tempString, 'r');	//Open source file and display error if not found
	if(error_check==-1){
		write(uart_write,"\r\nERROR: Source file does not exist.\r\n",38);
	}else{
		error_check = file_fopen(&dest_file, &efsl.myFs, arg_strings[2], 'w');	//Create new file and display error if file of same name already exists
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

euint32 do_newdir(euint8 no_args, euint8* arg_strings[]){							//Function to create new directory
	int error_check=0;															//Error check variable
	char tempString[128] = {NULL};
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable

	strcpy(tempString,currentDir);
	strcat(tempString,arg_strings[1]);
	error_check = makedir(&efsl.myFs,tempString);							//Create directory

	if(error_check==-1){														//Display appropriate error messages
		write(uart_write,"\r\nERROR: Directory already exists.\r\n",36);
	}else if(error_check==-2){
		write(uart_write,"\r\nERROR: Parent directory does not exist.\r\n",43);
	}

	fs_umount(&efsl.myFs);														//Unmount/save SD card
	return(0);
}

euint32 do_open(euint8 no_args, euint8* arg_strings[]){							//Function to open directory and list it's contents
	int error_check=0;															//Error check variable
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	char tempDir[128]={NULL};
	char disp_string[128]={NULL};												//String used to display info via UART
	int i = 0;																	//Counter variable

	strcpy(tempDir,currentDir);

	if(arg_strings[1][0]=='/'){
		memset(currentDir,NULL,128);												//Clear currentDir
		strcpy(currentDir,arg_strings[1]);
	}else{
		strcat(currentDir,arg_strings[1]);
		strcat(currentDir,"/");
	}

	error_check = ls_openDir(&list,&efsl.myFs,currentDir);					//Open new directory

	write(uart_write,"\r\n",2);

	if(0==error_check){															//List contents of directory
		while(ls_getNext(&list)==0){												//List contents of root directory
			if(list.currentEntry.Attribute & 0x10){
				sprintf(disp_string,"\t%.11s\t<DIR>\n\r",list.currentEntry.FileName);
				write(uart_write,disp_string,strlen(disp_string));
			}else if((list.currentEntry.Attribute & 0x08) == 0){
				sprintf(disp_string,"\t%.11s\t<%d bytes>\n\r",list.currentEntry.FileName,list.currentEntry.FileSize);
				write(uart_write,disp_string,strlen(disp_string));
			}
			i++;
		}
		if(i==0){
			write(uart_write,"Directory empty.\r\n",18);
		}
	}else{																		//If directory could not be opened display error message
		write(uart_write,"ERROR: Directory could not be opened.\r\n",39);
		strcpy(currentDir,tempDir);
	}

	fs_umount(&efsl.myFs);														//Unmount/save SD card
	return(0);
}

euint32 do_play(euint8 no_args, euint8* arg_strings[]){							//Function to copy file from one directory to another on SD card
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	wav_fmt format;
	char buffer[64]={NULL};														//File read/write buffer
	char tempString[128] = {NULL};
	int error_check=0;															//Error check variable

	strcpy(tempString,currentDir);

	strcat(tempString,arg_strings[1]);										//Create filepath strings from file name and directories

	currentTrack = 0;
	trackCount = 0;
	error_check = file_fopen(&nowPlaying, &efsl.myFs, tempString, 'r');	//Open source file and display error if not found
	if(error_check==-1){
		write(uart_write,"\r\nERROR: File does not exist.\r\n",31);
	}else{
		OSSemPost(PlaySem);
	}

	fs_umount(&efsl.myFs);														//Unmount/save SD card
	return(0);
}

euint32 do_new_plist(euint8 no_args, euint8* arg_strings[]){								//Function to delete file off SD card
	int error_check=0;
	char tempString[128] = {NULL};
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	FILE list;

	strcpy(tempString,"/plists/");
	strcat(tempString,arg_strings[1]);
	strcat(tempString,".lst");

	error_check = file_fopen(&list, &efsl.myFs, tempString, 'w');

	if(error_check==-2){
		write(uart_write,"\r\nERROR: Playlist already exists.\r\n",35);
		return(0);
	}

	file_fclose(&list);
	fs_umount(&efsl.myFs);														//Unmount/save SD card
	return(0);
}

euint32 do_add_plist(euint8 no_args, euint8* arg_strings[]){								//Function to delete file off SD card
	int error_check=0;
	char plistString[128] = {NULL};
	char fileString[128] = {NULL};
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	FILE list;
	FILE song;

	strcpy(fileString,currentDir);
	strcat(fileString,arg_strings[1]);
	strcat(fileString," ");

	strcpy(plistString,"/plists/");
	strcat(plistString,arg_strings[2]);
	strcat(plistString,".lst");

	error_check = file_fopen(&list, &efsl.myFs, plistString, 'a');
	file_write(&list,strlen(fileString),fileString);

	file_fclose(&list);
	fs_umount(&efsl.myFs);														//Unmount/save SD card
	return(0);
}

euint32 do_play_plist(euint8 no_args, euint8* arg_strings[]){							//Function to copy file from one directory to another on SD card
	int uart_write=open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	wav_fmt format;
	char buffer[1024]={NULL};														//File read/write buffer
	char plistString[128] = {NULL};
	char fileString[128] = {NULL};
	int error_check=0;															//Error check variable
	int i = 0;
	int j=0;

	strcpy(plistString,"/plists/");
	strcat(plistString,arg_strings[1]);
	strcat(plistString,".lst");

	error_check = file_fopen(&currentPlist, &efsl.myFs, plistString, 'r');	//Open source file and display error if not found
	if(error_check==-1){
		write(uart_write,"\r\nERROR: File does not exist.\r\n",31);
	}

	error_check = file_read(&currentPlist,1024,buffer);

	trackCount = string_parser(buffer,plist);

	currentTrack = 0;

	error_check = file_fopen(&nowPlaying, &efsl.myFs, plist[currentTrack], 'r');	//Open source file and display error if not found
	if(error_check==-1){
		write(uart_write,"\r\nERROR: File does not exist.\r\n",31);
	}else{
		OSSemPost(PlaySem);
	}

	fs_umount(&efsl.myFs);														//Unmount/save SD card
	return(0);
}

euint32 do_help(euint8 no_args, euint8* arg_strings[]){							//Function to display help for available functions
	int uart_write = open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable

	write(uart_write,"\r\n",2);
	write(uart_write,"\topen\t\t<dir>\t\t\t\t\tOpen specified directory and list contents.\r\n",62);
	write(uart_write,"\tnewdir\t\t<dir>\t\t\t\t\tCreate new directory with specified path.\r\n",62);
	write(uart_write,"\tcopy\t\t<file> <dest dir>\t\t\tCreate a copy of a file.\r\n",52);
	write(uart_write,"\tdel\t\t<file>\t\t\t\t\tDelete specified file.\r\n",40);
	write(uart_write,"\thelp\t\t\t\t\t\t\tDisplay this message.\r\n",35);

	return(0);
}

void audio_irq_init(){
	alt_irq_register(AUDIO_IRQ, NULL, audio_irq_handler);
	return;
}

void audio_irq_handler(void* context) {
	if(alt_up_audio_write_interrupt_pending(audio_dev)){
		alt_up_audio_disable_write_interrupt(audio_dev);
		OSSemPost(AudioSem);
	}
	return;
}
