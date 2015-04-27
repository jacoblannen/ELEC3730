#include <stdio.h>
#include <fcntl.h>														//Included for read/write flags
#include <unistd.h>														//Included for read/write operations
#include <string.h>
#include "efs.h"
#include "ls.h"

int main()
{
	EmbeddedFileSystem efsl;
	DirList list;
	esint8 ret;
	int uart_read=open("/dev/uart_0",O_RDONLY);							//Open UART with a read variable
	int uart_write=open("/dev/uart_0",O_WRONLY);						//Open UART with a write variable
	char buffer[100];													//Buffer to receive/transmit UART data
	char disp_string[100];
	int i=0;															//Counter used to check number of files in directory

	write(uart_write,"Initialising SD card...\r\n",25);
	ret = efs_init(&efsl, "/dev/sda");
	if(ret==0){
		write(uart_write,"SD card correctly initialised.\r\n\n",33);
	}else{
		write(uart_write,"Could not initialise.\r\n\n",24);
		return(0);
	}

	ls_openDir(&list, &efsl.myFs, "/");

	while(ls_getNext(&list)==0){
		if(list.currentEntry.Attribute & 0x10){
			sprintf(disp_string,"Dir:\t %s\n\r",list.currentEntry.FileName);
			write(uart_write,disp_string,strlen(disp_string));
		}else{
			sprintf(disp_string,"File:\t %s\t Size: %d bytes\n\r",list.currentEntry.FileName,list.currentEntry.FileSize);
			write(uart_write,disp_string,strlen(disp_string));
		}
		i++;
	}

	if(i==0){
		write(uart_write,"Root directory empty.\r\n",23);
	}

	fs_umount(&efsl.myFs);

	return (0);
}
