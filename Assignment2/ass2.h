/* Jacob Lannen
 * 3162100
 * Header for Assignment 2
 * 1/4/2015
 */

#ifndef ASS2_H_
#define ASS2_H_

#include "alt_types.h"

#define lcd_write_cmd(base, data)                     IOWR(base, 0, data)			// Define LCD functions
#define lcd_read_cmd(base)                            IORD(base, 1)
#define lcd_write_data(base, data)                    IOWR(base, 2, data)
#define lcd_read_data(base)                           IORD(base, 3)

#define ERR_NO_1 -1																	// Define error codes
#define ERR_NO_2 -2
#define LCD_LIMIT 9999999999999999													// Define limit for LCD screen

typedef struct{																		// Define command structure
		alt_8* com_string;
		alt_32 (*com_fun)(alt_8 argc, alt_8* argv[]);
	}command;

alt_32 do_ledr(alt_8 no_args, alt_8* arg_strings[]);								// Prototypes for Q2 functions
alt_32 do_add(alt_8 no_args, alt_8* arg_strings[]);
alt_32 do_switch(alt_8 no_args, alt_8* arg_strings[]);
alt_32 do_help1(alt_8 no_args, alt_8* arg_strings[]);

alt_32 do_del(alt_8 no_args, alt_8* arg_strings[]);									// Prototypes for Q4 functions
alt_32 do_copy(alt_8 no_args, alt_8* arg_strings[]);
alt_32 do_newdir(alt_8 no_args, alt_8* arg_strings[]);
alt_32 do_open(alt_8 no_args, alt_8* arg_strings[]);
alt_32 do_help(alt_8 no_args, alt_8* arg_strings[]);

void LCD_Init();																	// Prototypes for LCD functions
void LCD_Show_Text(char* Text);
void LCD_Line2();

char string_parser(char* inp, char* array_of_words[]);								// Prototypes for string parser function

#endif /* ASS2_H_ */
