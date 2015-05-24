/*
 * audio.c
 *
 *  Created on: 22/05/2015
 *      Author: c3162100
 */

#include "ass3.h"

wav_fmt get_format(SD_FILE file){
	wav_fmt output;
	euint32 i = 0;
	euint8 buf[44] = {NULL};
	euint8 code[5] = {NULL};

	sd_fileRead(&file,buf,44);

	for(i=0;i<4;i++){
		code[i] = extract_little(buf,i,1);
	}


	if(strcmp(code,"RIFF")){
		output.file_size=0;
		return(output);
	}

	output.file_size = extract_little(buf,4,4);
	printf("File size: %u bytes.\n",output.file_size+8);

	for(i=0;i<4;i++){
			code[i] = extract_little(buf,8+i,1);
	}

	if(strcmp(code,"WAVE")){
		output.file_size=0;
		return(output);
	}

	for(i=0;i<4;i++){
			code[i] = extract_little(buf,12+i,1);
	}

	if(strcmp(code,"fmt ")){
		output.file_size=0;
		return(output);
	}

	output.fmt_ck_size = extract_little(buf,16,4);
	printf("Format chunk size: %u bytes.\n",output.fmt_ck_size+8);

	output.format_tag = extract_little(buf,20,2);
	printf("format tag: %u\n",output.format_tag);

	output.channels = extract_little(buf,22,2);
	printf("channels: %u\n",output.channels);

	output.sample_rate = extract_little(buf,24,4);
	printf("Sample rate: %u\n",output.sample_rate);

	output.data_rate = extract_little(buf,28,4);
	printf("Byte rate: %u\n",output.data_rate);

	output.block_allign = extract_little(buf,32,2);
	printf("Block allign: %u\n",output.block_allign);

	output.bit_rate = extract_little(buf,34,2);
	printf("Bit rate: %u\n",output.bit_rate);

	output.dat_ck_size = extract_little(buf,40,4);
	printf("Data chunk size: %u bytes\n",output.dat_ck_size);

	return(output);
}

void set_format(wav_fmt format){

	switch(format.sample_rate){
    	case 8000:  AUDIO_SetSampleRate(RATE_ADC8K_DAC8K_USB); break;
    	case 32000: AUDIO_SetSampleRate(RATE_ADC32K_DAC32K_USB); break;
    	case 44100: AUDIO_SetSampleRate(RATE_ADC44K_DAC44K_USB); break;
    	case 48000: AUDIO_SetSampleRate(RATE_ADC48K_DAC48K_USB); break;
    	case 96000: AUDIO_SetSampleRate(RATE_ADC96K_DAC96K_USB); break;
    	default:  printf("Non-standard sampling rate\n"); return -1;
	}

	return;
}

void playFile(SD_FILE file){
	euint32 dat_read = 0;
	euint32 samp_count = 0;
	euint8 buf[16384] = {NULL};
	euint32 lBuf[2048] = {NULL};
	euint32 rBuf[2048] = {NULL};
	wav_fmt format;
	euint32 i = 0;
	euint32 switch_in = 0;
	int uart_write = open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable

	format = get_format(file);
	if(format.file_size==0){
		write(uart_write,"\r\nERROR: WAV file format not compatible.\r",41);
	}
	set_format(format);

	file.fptr=44;

	dat_read = sd_fileRead(&file,buf,format.block_allign*2048);

	while(dat_read&&switch_in==0){
		switch_in = IORD(SWITCH_PIO_BASE,0);
		samp_count = dat_read/format.block_allign;
		switch(format.bit_rate){
			case 8:
				for(i=0;i<dat_read;i++){
					buf[i]-=0x80;
				}
				audio_play_u8(buf,samp_count,format.channels,lBuf,rBuf,1);
				break;
			case 16:
				audio_play_u16(buf,samp_count,format.channels,lBuf,rBuf,1);
				break;
			case 24:
				audio_play_u24(buf,samp_count,format.channels,lBuf,rBuf,1);
				break;
			case 32:
				audio_play_u32(buf,samp_count,format.channels,lBuf,rBuf,1);
				break;
			default:
				printf("Non-standard sample size\n");
				return;
		}

//		for(i=0;i<256;i++){
//			printf("%d\t%d\n",lBuf[i],rBuf[i]);
//		}
		while(alt_up_audio_write_fifo_space(audio_dev,0)<samp_count);
		alt_up_audio_write_fifo(audio_dev,lBuf,samp_count,ALT_UP_AUDIO_LEFT);
		alt_up_audio_write_fifo(audio_dev,rBuf,samp_count,ALT_UP_AUDIO_RIGHT);
		dat_read = sd_fileRead(&file,buf,format.block_allign*2048);
	}

	return;
}
