/*
 * ass2.h
 *
 *  Created on: 01/04/2015
 *      Author: c3162100
 */

#ifndef ASS2_H_
#define ASS2_H_

#include "alt_types.h"

#define lcd_write_cmd(base, data)                     IOWR(base, 0, data)
#define lcd_read_cmd(base)                            IORD(base, 1)
#define lcd_write_data(base, data)                    IOWR(base, 2, data)
#define lcd_read_data(base)                           IORD(base, 3)
#define ERR_NO_1 -1
#define ERR_NO_2 -2

typedef struct{
		alt_8* com_string;
		alt_32 (*com_fun)(alt_8 argc, alt_8* argv[]);
	}command;

alt_32 do_ledr(alt_8 no_args, alt_8* arg_strings[]);
alt_32 do_add(alt_8 no_args, alt_8* arg_strings[]);
alt_32 do_switch(alt_8 no_args, alt_8* arg_strings[]);

void LCD_Init();
void LCD_Show_Text(char* Text);
void LCD_Line2();

char string_parser(char* inp, char* array_of_words[]);

#endif /* ASS2_H_ */
