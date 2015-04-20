#include <stdio.h>
#include <fcntl.h>														//Included for write flags
#include <unistd.h>														//Included for write operations
#include <string.h>														//Included for strlen()
#include "efs.h"														//Included for EFSL functions
#include "ls.h"

int main()
{
	EmbeddedFileSystem efsl;											//Create file system
	DirList list;														//Create directory list
	esint8 ret;															//Error check variable
	int uart_write=open("/dev/uart_0",O_WRONLY);						//Open UART with a write variable
	char disp_string[100];												//String to be displayed on PuTTY

	write(uart_write,"Initialising SD card...\r\n",25);					//Display initialisation status in PuTTY
	ret = efs_init(&efsl, "/dev/sda");									//Initialise SD card and display whether successful
	if(ret==0){
		write(uart_write,"SD card correctly initialised.\r\n",32);
	}else{
		write(uart_write,"Could not initialise.\r\n",23);
		return(0);
	}

	ls_openDir(&list, &efsl.myFs, "/");									//Open root directory

	while(ls_getNext(&list)==0){										//Loop for whole directory
		if(list.currentEntry.Attribute & 0x10){							//Display directory names
			sprintf(disp_string,"Directory: %s\n",list.currentEntry.FileName);
			write(uart_write,disp_string,strlen(disp_string));
		}else{															//Display file names/sizes
			sprintf(disp_string,"File: %s\t Filesize: %d\n",list.currentEntry.FileName,list.currentEntry.FileSize);
			write(uart_write,disp_string,strlen(disp_string));
		}
	}

	fs_unmount(&efs.myFs);												//Unmount directory

	return (0);
}
