#ifndef __FAT_H__
#define __FAT_H__

#include <inttypes.h>
#include <stdlib.h>
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

class FatFileSystem;

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
	uint32_t cluster_offset_mask;
	uint32_t cluster_bits;

	int fat_size;
	
	char oem_name[9];

public:
	FatInfo() {
	}
	
	bool initialize(BlockDevice& dev);
	void print_info();

	friend class FatFileSystem;
};

typedef uint32_t fat_off_t;
typedef uint32_t cluster_t;

class FatFileSystem
{
	FatInfo info;
public:
	FatFileSystem(BlockDevice& dev) {
		info.initialize(dev);
		info.print_info();
	}

	size_t    read(cluster_t&, fat_off_t&, size_t, void*);
	cluster_t cluster_seek_fwd(cluster_t current, 
		fat_off_t current_offset, fat_off_t wanted_pos);
};

class FatFile {
	void      *cache;
	fat_off_t  cache_start;
	fat_off_t  cache_end;
	fat_off_t  position;
	fat_off_t  file_size;
	cluster_t  file_start;
	cluster_t  current_cluster;
	FatFileSystem& filesys;
public:
	FatFile(FatFileSystem& system, cluster_t start);

	size_t read(size_t n_bytes, void *buffer);
	size_t setpos(size_t pos);
};

class DirBrowser {
	int x;
public:
	
};

#endif 
