/* Jacob Lannen
 * 3162100
 * Assignment 1 question 5 - Function to split a string into an array of words. Takes input of string pointer 
 * 							 and array pointer and outputs the number of words in string
 * 27/3/2015
 */
 
#include <stdio.h>
#include <stdlib.h>														//Included for malloc()
#include <string.h>														//Included for strcpy()

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
