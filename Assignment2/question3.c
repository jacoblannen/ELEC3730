/* Jacob Lannen
 * 3162100
 * Assignment 2 question 3
 * 21/4/2015
 */
#include <stdio.h>
#include <fcntl.h>														//Included for read/write flags
#include <unistd.h>														//Included for read/write operations
#include <string.h>														//Included for string functions
#include "efs.h"														//Included for EFSL functions
#include "ls.h"															//Included for EFSL functions

int main()
{
	EmbeddedFileSystem efsl;											//Initialise file system pointer
	DirList list;														//Initialise directory list pointer
	esint8 ret;															//Initialise error check variable
	int uart_read=open("/dev/uart_0",O_RDONLY);							//Open UART with a read variable
	int uart_write=open("/dev/uart_0",O_WRONLY);						//Open UART with a write variable
	char buffer[128];													//Buffer to receive/transmit UART data
	char disp_string[128];												//String used to display info over UART
	int i=0;															//Counter used to check number of files in directory

	write(uart_write,"Initialising SD card...\r\n",25);					//Initialise SD card
	ret = efs_init(&efsl, "/dev/sda");
	if(ret==0){
		write(uart_write,"SD card correctly initialised.\r\n\n",33);
	}else{																//Display error message if SD card cannot be initialised
		write(uart_write,"Could not initialise.\r\n\n",24);
		return(0);
	}

	ls_openDir(&list, &efsl.myFs, "/");									//Open root directory

	while(ls_getNext(&list)==0){										//List directory contents
		if(list.currentEntry.Attribute & 0x10){
			sprintf(disp_string,"Dir:\t %s\n\r",list.currentEntry.FileName);
			write(uart_write,disp_string,strlen(disp_string));
		}else{
			sprintf(disp_string,"File:\t %s\t Size: %d bytes\n\r",list.currentEntry.FileName,list.currentEntry.FileSize);
			write(uart_write,disp_string,strlen(disp_string));
		}
		i++;
	}

	if(i==0){															//Display message if directory empty
		write(uart_write,"Root directory empty.\r\n",23);
	}

	fs_umount(&efsl.myFs);												//Unmount/save SD card

	return (0);
}
