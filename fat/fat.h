#ifndef __FAT_H__
#define __FAT_H__

#include <inttypes.h>

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
	
	bool initialize(void *data);
	void print_info();
};

#endif 
