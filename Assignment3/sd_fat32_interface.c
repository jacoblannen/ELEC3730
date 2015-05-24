#include "ass3.h"

euint32 sd_boot(void){
	euint8 sectBuf[512]={NULL};
	euint16 resv_sect;
	euint8 no_fat;
	euint32 check = 0;

	if(if_initInterface()){
		printf("SD card could not be mounted\n");
		return(-1);
	}

	sd_readSector(0,sectBuf);

	check = extract_little(sectBuf,450,1);

	if(check!=0x0b&&check!=0x0c){
		printf("SD card format incorrect.\n");
		return(-2);
	}

	check = extract_little(sectBuf,454,4);

	sd_readSector(check,sectBuf);

	sect_size = extract_little(sectBuf,0x0b,2);
	clust_size = extract_little(sectBuf,0x0d,1);
	resv_sect = extract_little(sectBuf,0x0e,2);
	no_fat = extract_little(sectBuf,0x10,1);
	fat_size = extract_little(sectBuf,0x24,4);
	root_dir = extract_little(sectBuf,0x2c,4);

	fat_addr = check+resv_sect;
	zeroth_clust = fat_addr + (no_fat*fat_size)- (2*clust_size);
	clust_bytes = sect_size*clust_size;
	max_ent = (clust_bytes)/32;

	dir_list = (dir_ent *) malloc(max_ent*sizeof(dir_ent));

	return(0);
}

void sd_readCluster(euint32 addr, euint8* buf){
	euint32 i = 0;

	for(i=0;i<clust_size;i++){
		sd_readSector(zeroth_clust+(addr*clust_size)+i,buf+(i*sect_size));
	}

	return;
}

void sd_openDir(euint32 dir_addr){
	euint32 i = 0, j = 0;
	euint8 *clustBuf;
	euint8 temp_dirEnt[32];
	euint8 check = 1;

	current_dir = dir_addr;
	clustBuf = (euint8 *) malloc(clust_bytes);

	while(dir_addr < 0x0fffffff){
//		printf("test\n");
		sd_readCluster(dir_addr,clustBuf);
//		printf("test2\n");
		while(i<max_ent&&check!=0){
			for(j=i*32;j<(i+1)*32;j++){
				temp_dirEnt[j%32]=clustBuf[j];
			}
			strncpy(dir_list[i].filename,temp_dirEnt,11);
			dir_list[i].filename[11] = NULL;
			dir_list[i].fileSize = extract_little(temp_dirEnt,0x1c,4);
			dir_list[i].fileStart = (extract_little(temp_dirEnt,0x14,2)>>16)+extract_little(temp_dirEnt,0x1a,2);
			dir_list[i].fileAttr = temp_dirEnt[0x0b];

			check = dir_list[i].filename[0];
			i++;
		}

		i=0;

		dir_addr = sd_nextClust(dir_addr);

	}

	return;
}

euint32 sd_nextClust(euint32 addr){
	euint8 *sectBuf;

	sectBuf = (euint8 *) malloc(sect_size);

	sd_readSector(fat_addr+(addr/(sect_size/4)),sectBuf);
	addr = extract_little(sectBuf,4*(addr%(sect_size/4)),4);

	return(addr);
}

void sd_viewDir(void){
	int uart_write = open("/dev/uart_0",O_WRONLY);								//Open UART with a write variable
	char disp_string[128]={NULL};												//String used to display data over UART
	euint32 i = 0, j = 0;
	euint32 dir_addr = current_dir;
	euint8 *clustBuf;
	euint8 temp_dirEnt[32];
	euint8 check = 1;

	clustBuf = (euint8 *) malloc(clust_bytes);

	while(dir_addr < 0x0fffffff){
		sd_readCluster(dir_addr,clustBuf);
		while(i<max_ent&&check!=0){
			for(j=i*32;j<(i+1)*32;j++){
				temp_dirEnt[j%32]=clustBuf[j];
			}
			strncpy(dir_list[i].filename,temp_dirEnt,11);
			dir_list[i].filename[11] = NULL;
			dir_list[i].fileSize = extract_little(temp_dirEnt,0x1c,4);
			dir_list[i].fileStart = (extract_little(temp_dirEnt,0x14,2)<<16)+extract_little(temp_dirEnt,0x1a,2);
			dir_list[i].fileAttr = temp_dirEnt[0x0b];

			check = dir_list[i].filename[0];
			i++;
		}

		i=0;

		while(dir_list[i].filename[0]&&i<max_ent){
			if((dir_list[i].fileAttr!=8)&&(dir_list[i].fileAttr!=0x0f)&&(dir_list[i].filename[0]!=0xe5)){
				if(dir_list[i].fileAttr&0x10){
					sprintf(disp_string,"\r\n%s\t<DIR>\tStart:%d",dir_list[i].filename,dir_list[i].fileStart);
					write(uart_write,disp_string,strlen(disp_string));
				}else{
					sprintf(disp_string,"\r\n%s\t<%u Bytes>\tStart:%d",dir_list[i].filename,dir_list[i].fileSize,dir_list[i].fileStart);
					write(uart_write,disp_string,strlen(disp_string));
				}
			}
			i++;
		}

		i=0;

		dir_addr = sd_nextClust(dir_addr);

	}

	return;
}

dir_ent sd_getDir(euint8* dir_name){
	euint32 i = 0, j = 0, ent_count = 0;
	euint32 dir_addr = current_dir;
	euint8 *clustBuf;
	euint8 temp_dirEnt[32];
	euint8 check = 1;
	dir_ent root_ent;
	dir_ent blank_ent;

	root_ent.fileAttr=0x10;
	root_ent.fileStart=root_dir;

	blank_ent.fileStart=0;

	clustBuf = (euint8 *) malloc(clust_bytes);

	if(strlen(dir_name)<11){
		strncat(dir_name,"          ",11-strlen(dir_name));
	}

	for(i=0;i<strlen(dir_name);i++){
		dir_name[i]=toupper(dir_name[i]);
	}

	while(dir_addr <= 0x0fffffef && dir_addr >=2){
		sd_readCluster(dir_addr,clustBuf);
		while(i<max_ent&&check!=0){
			for(j=i*32;j<(i+1)*32;j++){
				temp_dirEnt[j%32]=clustBuf[j];
			}
			strncpy(dir_list[i].filename,temp_dirEnt,11);
			dir_list[i].filename[11] = NULL;
			dir_list[i].fileSize = extract_little(temp_dirEnt,0x1c,4);
			dir_list[i].fileStart = (extract_little(temp_dirEnt,0x14,2)<<16)+extract_little(temp_dirEnt,0x1a,2);
			dir_list[i].fileAttr = temp_dirEnt[0x0b];

			check = dir_list[i].filename[0];
			i++;
		}

		ent_count = i;

		for(i=0;i<ent_count;i++){
			if(strcmp(dir_list[i].filename,dir_name)==0){
				return(dir_list[i]);
			}else if(strcmp("/          ",dir_name)==0){
				return(root_ent);
			}else{
				dir_addr = sd_nextClust(dir_addr);
			}
		}

		i=0;

	}

	return(blank_ent);
}

void sd_fOpen(SD_FILE *output, dir_ent file){

	output->fdata.fileAttr = file.fileAttr;
	output->fdata.fileSize = file.fileSize;
	output->fdata.fileStart = file.fileStart;
	strcpy(output->fdata.filename,file.filename);
	output->fptr=0;
	output->fcursor=0;

	return;
}

euint32 sd_fileRead(SD_FILE *file,euint8* buf, euint32 n){
	euint32 count = 0;
	euint32 clust_to_read = 0;
	euint32 sect_to_read = 0;
	euint32 bytes_to_read = 0;
	euint32 i = 0, j = 0, k = 0,flag = 0;
	euint8 *clustBuf;

	memset(buf,NULL,n);

	clustBuf = (euint8 *) malloc(clust_bytes);

	sd_readCluster(file->fdata.fileStart,clustBuf);

	if(file->fcursor==file->fdata.fileSize){
					return(count);
	}

	clust_to_read = n/clust_bytes+1;
	bytes_to_read = n%clust_bytes;

	for(j=0;j<clust_to_read;j++){
		if(j!=clust_to_read-1){
			for(i=0;i<clust_bytes;i++){
				if((file->fcursor%clust_bytes)!=0||file->fcursor==0||flag){
					buf[(j*clust_bytes)+i] = clustBuf[i+file->fptr];
					file->fcursor++;
					flag=0;
					count++;
					if(count == n){
						file->fptr+=count;
						free(clustBuf);
						return(count);
					}
					if(file->fcursor == file->fdata.fileSize){
						free(clustBuf);
						return(count);
					}
				}else{
					(file->fdata.fileStart) = sd_nextClust(file->fdata.fileStart);
					sd_readCluster(file->fdata.fileStart,clustBuf);
					flag=1;
					i--;
					file->fptr=0;
				}
			}
		}else{
			for(i=0;i<bytes_to_read;i++){
				if((file->fcursor%clust_bytes)!=0||file->fcursor==0||flag){
					buf[(j*clust_bytes)+i] = clustBuf[i+file->fptr];
					file->fcursor++;
					flag=0;
					count++;
					if(count == n){
						file->fptr+=count;
						free(clustBuf);
						return(count);
					}
					if(file->fcursor == file->fdata.fileSize){
						free(clustBuf);
						return(count);
					}
				}else{
					(file->fdata.fileStart) = sd_nextClust(file->fdata.fileStart);
					sd_readCluster(file->fdata.fileStart,clustBuf);
					flag=1;
					i--;
					file->fptr=0;
				}
			}
		}
	}
}
