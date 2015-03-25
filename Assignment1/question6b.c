#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	int uart_read=open("/dev/uart_0",O_RDONLY);
	int uart_write=open("/dev/uart_0",O_WRONLY);
	char buffer[100];

	while(buffer[0]!=0xd&&buffer[0]!=0x1b){
		read(uart_read,buffer,1);
		write(uart_write,buffer,1);
		printf("%c",buffer[0]);
	}

  return 0;
}
