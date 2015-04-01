/* Jacob Lannen
 * 3162100
 * Assignment 1 Question 1
 * 1/4/2015
 */

#include <stdio.h>
#include <fcntl.h>														//Included for read/write flags
#include <unistd.h>														//Included for read/write operations
#include <stdlib.h>														//Included for malloc()
#include <string.h>														//Included for strcpy() and strlen()

char string_parser(char* inp, char* array_of_words[]);					//Function prototype (may be moved to a header file)

int main()
{
	int uart_read=open("/dev/uart_0",O_RDONLY);							//Open UART with a read variable
	int uart_write=open("/dev/uart_0",O_WRONLY);						//Open UART with a write variable
	char buffer[100];													//Buffer to receive/transmit UART data
	char cmd_string[100];												//String received from UART at carriage return
	char* out_array[50];												//Array of strings used in string_parser()
	int i=0;															//Loop counter
	int arg_count=0;													//Number of input arguments/words received from command string

	while(buffer[0]!=0xd){												//Loop until "enter" keystroke
		read(uart_read,buffer,1);										//Read from UART
		write(uart_write,buffer,1);										//Write to UART
		printf("%c",buffer[0]);											//Display on console
		cmd_string[i]=buffer[0];										//Write input to cmd_string
		i++;
	}

	cmd_string[i-1]=0;													//Null terminate command string
	arg_count = string_parser(cmd_string,out_array);					//Use string_parser() to separate arguments of cmd_string

	for(i=0;i<arg_count;i++){											//Write all arguments to console/PuTTY
		write(uart_write,"\r\n",2);
		write(uart_write,out_array[i],strlen(out_array[i]));
		printf("\n%s",out_array[i]);
	}

  return (0);
}

char string_parser(char* inp, char* array_of_words[]){
	char buffer[50];													//Buffer string used to retrieve single word at a time to be placed into array
	int i=0;															//Loop counters
	int j=0;
	int k=0;

	if(inp<0||array_of_words<0){										//Check pointers contain valid memory addresses
		printf("Please ensure pointers point to valid memory address.\n");
		return(0);
	}

	while(inp[i]!=0){													//While loop till end of input string (if input is NULL the loop ends)
		if(inp[i]!=' '){												//If current char in input string is not a space move it into the buffer and increment indicies
			buffer[k] = inp[i];
			i++;
			k++;
		}else{
			buffer[k]='\0';												//If input char is a space null terminate buffer string
			array_of_words[j] = malloc(k);								//Initialise string to required length and move buffer in.
			strcpy(array_of_words[j],buffer);

			if(i>0){													//Increment word count ignoring leading space
				j++;
			}
			while(inp[i]==' '){											//Check for multiple spaces
				i++;
			}

			k=0;														//Reset buffer index for new word
		}
	}

	if(i!=0){															//After end of input string is detected check that it wasn't empty
		if(inp[i-1]!=' '){												//Ensure last char was not a space
			buffer[k]='\0';												//Move final buffer into array and increment word count
			array_of_words[j] = malloc(k);
			strcpy(array_of_words[j],buffer);
			j++;
		}
	}else{																//Display error if string is empty
		printf("ERROR: Input string contains starts with NULL.\n");
	}

	return(j);															//Return wordcount
}
