#include "fat.h"
#include <stdio.h>
#include <iostream>

using namespace std;

#define U8(offset)  (data[offset])
#define U16(offset) ((uint16_t)((data[offset + 1] << 8) | data[offset]))
#define U32(offset) ((uint32_t)(((uint32_t)data[offset + 3] << 24) | \
	                        ((uint32_t)data[offset + 2] << 16) | \
	 			((uint32_t)data[offset + 1] << 8)  | \
				 (uint32_t)data[offset]))

#define MAGIC	    0xAA55

#define OBPS	    0x0B
#define OSPC 	    0x0D
#define ONRESERVED  0x0E
#define ONFATS      0x10
#define ONROOTENT   0x11
#define ONTOTSECS   0x13
#define OMEDIATYPE  0x15
#define ONSPFAT     0x16
#define ONSECS      0x18
#define ONHEADS     0x1A
#define ONHIDDEN    0x1C
#define ONTOTSECS2  0x20
#define ONSPFAT2    0x24

#define OMAGIC      0x1FE

#define FAT12LIMIT  4085
#define FAT16LIMIT  65525

#define DIRENTRY_SIZE 32

bool FatInfo::initialize(void *_data)
{
	bool fat16_32 = false;
	uint8_t *data = (uint8_t*)_data;

	printf("%04x %04x", U16(OMAGIC), MAGIC);
	if (U16(OMAGIC) != MAGIC)
		return false;

	bytes_per_sector = U16(OBPS);
	switch (bytes_per_sector) {
		case 512:
		case 1024:
		case 2048:
		case 4096:
			break;
		default:
			return false;
	}

	int tmp = secs_per_cluster = U8(OSPC);

	if (tmp == 0)
		return false;

	while (tmp) {
		if (tmp == 1) 
			break;

		else if (tmp & 1)
			return false;

		tmp >>= 1;
	}
		printf("Here");

	total_reserved_sectors = U16(ONRESERVED);
	
	num_fats = U8(ONFATS);
	num_root_entries = U16(ONROOTENT);	

	total_sectors = U16(ONTOTSECS);
	if (total_sectors == 0) {
		fat16_32 = true;
		total_sectors = U32(ONTOTSECS2);
	}
	
	media_type = U8(OMEDIATYPE);
	sectors_per_fat = U16(ONSPFAT);
	if (sectors_per_fat == 0) {
		sectors_per_fat = U32(ONSPFAT2);	
	}

	num_sectors = U16(ONSECS);
	num_heads = U16(ONHEADS);

	if (fat16_32) {
		hidden_sectors = U32(ONHIDDEN);
	}
	else {
		hidden_sectors = U16(ONHIDDEN);
	}

	n_clusters = total_sectors - 
		total_reserved_sectors - num_fats * sectors_per_fat - 
		num_root_entries / DIRENTRY_SIZE;
	
	n_clusters /= secs_per_cluster;
	return true;
}

void FatInfo::print_info() {
	cout << "Bytes per sector: " << bytes_per_sector << endl;
	cout << "Secs per cluster: " << secs_per_cluster << endl;
	cout << "Total reserved:   " << total_reserved_sectors << endl;
	cout << "Number of FATs:   " << num_fats << endl;
	cout << "Root entries:     " << num_root_entries << endl;
	cout << "Total sectors:    " << total_sectors << endl;
	cout << "Media type:       " << media_type << endl;
	cout << "Sectors per fat:  " << sectors_per_fat << endl;
	cout << "Sectors:          " << num_sectors << endl;
	cout << "Heads:            " << num_heads << endl;
	cout << "Hidden sectors:   " << hidden_sectors << endl;
	cout << "Num clusters:     " << n_clusters << endl;
}

int main() {
	FILE *fp;
	FatInfo f;	
	
	char sector[1024];

	fp = fopen("/dev/sda7", "r");
	if (! fp) {
		fprintf(stderr, "perms? partition exists?\n"); 	
		exit(1);
	}	

	fread(sector, 512, 1, fp);
	fclose(fp);
	
	
	f.initialize(sector);
	f.print_info();
}
