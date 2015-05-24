/*
 * ass3.h
 *
 *  Created on: 20/05/2015
 *      Author: c3162100
 */

#ifndef ASS3_H_
#define ASS3_H_

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
#include "spi_sd.h"
#include "newtypes.h"
#include "io.h"

typedef struct{
	euint8 filename[12];
	euint8 fileAttr;
	euint32 fileSize;
	euint32 fileStart;
}dir_ent;

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

typedef struct{
	dir_ent fdata;
	euint32 fptr;
	euint32 fcursor;
}SD_FILE;

euint32 current_dir;

euint16 sect_size;
euint8 clust_size;
euint32 fat_size;
euint32 root_dir;
euint32 fat_addr;
euint32 zeroth_clust;
euint32 clust_bytes;
euint32 max_ent;

dir_ent *dir_list;

alt_up_audio_dev*  audio_dev;

euint32 sd_boot(void);
void sd_readCluster(euint32,euint8*);
void sd_openDir(euint32);
dir_ent sd_getDir(euint8*);
euint32 sd_nextClust(euint32);
void sd_viewDir(void);
void sd_fOpen(SD_FILE*,dir_ent);
euint32 sd_fileRead(SD_FILE *file,euint8* buf, euint32 n);

wav_fmt get_format(SD_FILE file);
void set_format(wav_fmt format);
void playFile(SD_FILE file);

int extract_little(euint8*,euint32,euint32);

euint32 do_open(euint8 no_args, euint8* arg_strings[]);
euint32 do_play(euint8 no_args, euint8* arg_strings[]);
euint32 do_help(euint8 no_args, euint8* arg_strings[]);

char string_parser(char* inp, char* array_of_words[]);								// Prototypes for string parser function

#endif /* ASS3_H_ */
