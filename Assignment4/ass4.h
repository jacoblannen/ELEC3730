/*
 * ass4.h
 *
 *  Created on: 17/06/2015
 *      Author: c3162100
 */

#ifndef ASS4_H_
#define ASS4_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>																//Included for read/write flags
#include <unistd.h>																//Included for read/write operations
#include <ctype.h>
#include "AUDIO.h"
#include "altera_up_avalon_audio_regs_dgz.h"
#include "altera_up_avalon_audio_dgz.h"
#include "altera_avalon_uart_regs.h"
#include "io.h"
#include "includes.h"
#include "efs.h"																//Included for EFSL functions
#include "ls.h"																	//Included for EFSL functions
#include "sys/alt_irq.h"   // For interupt handlling
#include "system.h"       // system-wide standard macros

typedef struct{																		// Define command structure
		euint8* com_string;
		euint32 (*com_fun)(euint8 argc, euint8* argv[]);
}command;

typedef struct{
	euint32 file_size;
	euint32 fmt_ck_size;
	euint32 dat_ck_size;
	euint16 format_tag;
	euint16 channels;
	euint32 sample_rate;
	euint32 data_rate;
	euint16 block_allign;
	euint16 bit_rate;
}wav_fmt;

static alt_u8  Map[] = {												//Lookup table of values for 7-seg display
		63, 6, 91, 79, 102, 109, 125, 7,
        127, 111, 119, 124, 57, 94, 121, 113
};

alt_up_audio_dev*  audio_dev;

void audio_irq_init();
void audio_irq_handler(void* context);

wav_fmt get_format(FILE* file);
void set_format(wav_fmt format);
void playFile(FILE* file);


int extract_little(euint8*,euint32,euint32);

euint32 do_del(euint8 no_args, euint8* arg_strings[]);
euint32 do_copy(euint8 no_args, euint8* arg_strings[]);
euint32 do_newdir(euint8 no_args, euint8* arg_strings[]);
euint32 do_open(euint8 no_args, euint8* arg_strings[]);
euint32 do_play(euint8 no_args, euint8* arg_strings[]);
euint32 do_new_plist(euint8 no_args, euint8* arg_strings[]);
euint32 do_add_plist(euint8 no_args, euint8* arg_strings[]);
euint32 do_play_plist(euint8 no_args, euint8* arg_strings[]);
euint32 do_help(euint8 no_args, euint8* arg_strings[]);

char string_parser(char* inp, char* array_of_words[]);								// Prototypes for string parser function

//EFSL Global Variables
EmbeddedFileSystem efsl;														//File system pointer
DirList list;																	//List pointer

FILE nowPlaying;
FILE currentPlist;
char plist[30][128];

/* Definition of Task Stacks */
#define TASK_STACKSIZE 4096
OS_STK task1_stk[TASK_STACKSIZE];
OS_STK task2_stk[TASK_STACKSIZE];
OS_STK task3_stk[TASK_STACKSIZE];

OS_EVENT* PlaySem;
OS_EVENT* AudioSem;

/* Definition of Task Priorities */

#define TASK1_PRIORITY 5
#define TASK2_PRIORITY 6
#define TASK3_PRIORITY 7


#endif /* ASS4_H_ */
