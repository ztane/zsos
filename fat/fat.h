#ifndef __FAT_H__
#define __FAT_H__

#include <inttypes.h>
#include <stdio.h>

class BlockDevice {
	FILE *fp;
	int filedes;
	uint32_t total_size;
	uint32_t total_secs;
	uint32_t sec_size;
public:
	BlockDevice(char *filename);

	uint32_t getSectorSize()   const {
		return sec_size;
	}

	uint32_t getTotalSectors() const {
		return total_secs;
	}

	void setSectorSize(uint32_t ns) {
		sec_size = ns;
	}

	void setTotalSectors(uint32_t ns) {
		total_secs = ns;
	}

	bool read(void *buffer, uint32_t offset, uint32_t number);
};

class FatInfo {
	uint32_t bytes_per_sector;
	uint32_t secs_per_cluster;
	uint32_t total_reserved_sectors;
	uint32_t num_fats;
	uint32_t num_root_entries;
	
	uint32_t total_sectors;
	uint32_t media_type;
	uint32_t sectors_per_fat;
	uint32_t num_sectors;
	uint32_t num_heads;
	uint32_t hidden_sectors;
	uint32_t n_clusters;

	int fat_size;
	
	char oem_name[9];

public:
	FatInfo() {
	}
	
	bool initialize(BlockDevice& dev);
	void print_info();
};

#endif 
