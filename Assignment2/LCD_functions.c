/*
 * LCD_functions.c
 *
 *  Created on: 17/04/2015
 *      Author: c3162100
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>  // for usec()
#include "alt_types.h"
#include "system.h"
#include "io.h"
#include "ass2.h"

void LCD_Init() {
  IOWR(LCD_16207_0_BASE,0,0x38);
  usleep(2000);
  IOWR(LCD_16207_0_BASE,0,0x0C);
  usleep(2000);
  IOWR(LCD_16207_0_BASE,0,0x01);
  usleep(2000);
  IOWR(LCD_16207_0_BASE,0,0x06);
  usleep(2000);
  IOWR(LCD_16207_0_BASE,0,0x80);
  usleep(2000);
}

void LCD_Line2() {
  lcd_write_cmd(LCD_16207_0_BASE,0xC0);
  usleep(2000);
}

void LCD_Show_Text(char* Text) {
  int i;
  for(i=0;i<strlen(Text);i++) {
    lcd_write_data(LCD_16207_0_BASE,Text[i]);
    usleep(2000);
  }
}
