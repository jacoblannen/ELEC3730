#include "AUDIO.h"
#include "altera_up_avalon_audio_regs_dgz.h"
#include "altera_up_avalon_audio_dgz.h"
#include <math.h>
#include "spi_sd.h"
#include "newtypes.h"

int main(void){
	euint8 buf[512]={NULL};
	euint8* ptr;
	euint32 addr[4]={NULL};
	euint32 size[4]={NULL};
	euint32 sectors_per_cluster;
	euint32 sectors_per_FAT;
	euint32 root_dir_addr;
	int i;

	if(if_initInterface()){
		return(-1);
	}
	sd_readSector(0,buf);

	for(i=0;i<4;i++){
		ptr = buf + 446 + i*16;
		addr[i] = extract_little(ptr,8,4);
		size[i] = extract_little(ptr,12,4);
		printf("Address: %d\tSize: %d\n",addr[i],size[i]);
	}

	sd_readSector(addr[0],buf);

	printf("Boot Sector of Partition 1:\n");

	for(i=0;i<512;i++){
		printf("%x\t%x\n",i,buf[i]);
	}

	sectors_per_cluster = extract_little(buf,0x0d,1);
	sectors_per_FAT = extract_little(buf,0x24,4);
	root_dir_addr = extract_little(buf,0x2c,4);

	printf("Sectors per cluster: %d\nSectors per FAT: %d\nCluster number of root dir: %d\n",sectors_per_cluster,sectors_per_FAT,root_dir_addr);

    return(0);
}

int extract_little(char* str, int offset, int n){
	euint8 temp;																										//Buffer to hold value in current byte before it is modified for output
	int power;																										//Used to hold output of pow() function. Used to set signifigance of current byte
	int output = 0;																									//Output integer
	int i;																											//Loop counter

	if(0>n||n>sizeof(int)){																							//Error check input values
		printf("Invalid value n. Please ensure n is between 0 and %d.\n",sizeof(int));
		output=-1;
	}else if(0>str){
		printf("Invalid pointer value. Please ensure str points to a valid memory address.\n");
		output=-1;
	}else if(0>offset){
		printf("Invalid offset value. Please use positive offset.\n");
		output=-1;
	}else{
		for(i = 0; i < n; i++){																						//Loop through expected number of bytes
			temp = *(str+offset);																					//Move current byte to buffer
			power=(int)pow((float)256,i);																			//Generate bias of byte
			output+= temp*power;																					//Apply bias and add to output
			offset++;																								//Increment offset to look at next byte of data
		}
	}
	return(output);																									//Return output
}
