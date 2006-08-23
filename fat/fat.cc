#include "fat.h"
#include <stdio.h>
#include <iostream>
#include <linux/fs.h>
#include <sys/ioctl.h>

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

void *allocateMemory(size_t size) {
	return malloc(size);
}

void freeMemory(void *ptr) {
	free(ptr);
}

bool FatInfo::initialize(BlockDevice& dev)
{
	bool fat16_32 = false;
	uint8_t *data = (uint8_t*)allocateMemory(512);

	fprintf(stderr, "muistit permit?\n");	
	dev.read(data, 0, 1);
	
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
			fprintf(stderr, "bytes per sector not a power of 2\n");
			freeMemory(data);
			return false;
	}

	dev.setSectorSize(bytes_per_sector);

	secs_per_cluster = U8(OSPC);
	switch (secs_per_cluster) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
		case 64:
		case 128:
			break;
		default:
			fprintf(stderr, "secs per cluster not a power of 2\n");
			freeMemory(data);
			return false;
	}

	total_reserved_sectors = U16(ONRESERVED);
	
	num_fats = U8(ONFATS);
	num_root_entries = U16(ONROOTENT);	

	total_sectors = U16(ONTOTSECS);
	if (total_sectors == 0) {
		fat16_32 = true;
		total_sectors = U32(ONTOTSECS2);
	}

	if (total_sectors > dev.getTotalSectors()) {
		fprintf(stderr, "Total sectors on BPB > size of partition\n");
		freeMemory(data);
		return false;
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
	
	if (n_clusters < FAT12LIMIT)
		fat_size = 12;
	else if (n_clusters < FAT16LIMIT)
		fat_size = 16;
	else
		fat_size = 32;

	int tmp = secs_per_cluster * bytes_per_sector;
	if (tmp == 0) {
		freeMemory(data);
		return false; 
	}

	cluster_bits = 0;
	cluster_offset_mask = tmp - 1;

	while (! (tmp & 1)) {
		tmp >>= 1;
		cluster_bits ++;		
	} 
	
	freeMemory(data);
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
	cout << "File system type: FAT" << fat_size << endl;
	cout << endl << endl;
	printf("Cluster offset bits: %d - cluster offset mask %08X", 
		cluster_bits, cluster_offset_mask);
}


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

BlockDevice::BlockDevice(char *filename)
{
	struct stat s;
	fp = fopen(filename, "r");
	if (! fp)
		return;

	filedes = fileno(fp);

	uint32_t nblocks;
	ioctl(filedes, BLKGETSIZE, &nblocks);

	setSectorSize(512);
	setTotalSectors(nblocks);
}

bool BlockDevice::read(void *buffer, uint32_t _offset, uint32_t number)
{
	off_t offset = _offset * sec_size;

	fseek(fp, offset, SEEK_SET);
	uint32_t rc = fread(buffer, sec_size, number, fp);
	return rc == number;
}

size_t FatFileSystem::read(cluster_t& current_cluster, 
	fat_off_t& position, size_t n_bytes, void *buffer)
{
	fat_off_t off      = position & info.cluster_offset_mask;
	fat_off_t end_diff = off + n_bytes - 1;
	
	cluster_t cluster_diff = end_diff >> info.cluster_bits;
	
		
}

FatFile::FatFile(FatFileSystem& system, cluster_t start) :
	filesys(system), file_start(start), current_cluster(start)
{
        cache_start = cache_end = position = 0;
	file_size = 1 << 20;
}

size_t FatFile::read(size_t n_bytes, void *buffer)
{
	return filesys.read(current_cluster, position, n_bytes, buffer);	
}

size_t FatFile::setpos(size_t pos) 
{
	if (position > file_size) {
		return (size_t)-1;
	} 
	
	//if (filesys.is_in_same_cluster(pos, position)) 
	//{
	//	position = pos;
	//	return position;
	//}

	cluster_t new_c;
	if (pos > position) {
		new_c = filesys.cluster_seek_fwd(current_cluster, position, pos);
	}
	else {
		new_c = filesys.cluster_seek_fwd(file_start, 0, pos);
	}

	if (new_c == (cluster_t)-1) {
		return (size_t)-1;
	}
	
	current_cluster = new_c;
	position = pos;
	return position;
}



int main() {
	FILE *fp;
	FatInfo f;	
	
	char sector[1024];
	BlockDevice dev("/dev/sdb3");
	f.initialize(dev);
	f.print_info();
}

