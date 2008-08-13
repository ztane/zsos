#ifndef __FAT_H__
#define __FAT_H__

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef uint32_t fat_off_t;
typedef uint32_t cluster_t;

class SectorHandle;
class BlockDevice {
	FILE *fp;
	int filedes;
	uint32_t total_size;
	uint32_t total_secs;
	uint32_t sec_size;
public:
	BlockDevice(const char *filename);

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

	bool     read(void *buffer, uint32_t offset, uint32_t number);
	bool     acquire_sector(uint32_t sectornumber, SectorHandle& handle);
};

class SectorHandle {
private:
	BlockDevice *dev;
	uint8_t *data;
	uint32_t sectornumber;
	uint16_t sec_size;
public:
	SectorHandle() : dev(0), data(0), sectornumber(0) {
	}
	
	uint32_t sector_size() {
		return sec_size;
	}

	void init(BlockDevice *_dev, uint8_t *_data, uint32_t num) {
		dev = _dev;
		data = _data;
		sectornumber = num;
		sec_size = _dev->getSectorSize();
	}

	SectorHandle(BlockDevice *_dev, uint8_t *_data, uint32_t _number) {
		init(_dev, _data, _number);
	}

	uint8_t *get_data() const {
		return data;
	}

	uint32_t get_sector_number() const {
		return sectornumber;
	}

	void release() {
		dev = NULL;
		delete[] data;
	}
};

class FatFileSystem;

class FatInfo {
public:
	uint32_t bytes_per_sector;

	uint32_t sector_bits;
	uint32_t sector_offset_mask;

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

	uint32_t fat_start_sector;

	uint32_t root_dir_start;
	uint32_t data_area_start;

	cluster_t fat32_root_dir_start;

	int fat_size;

	char oem_name[9];

public:
	FatInfo() { }

	bool initialize(BlockDevice& dev);
	void print_info() const;

	uint32_t fat12_16_root_dir_start() const {
		return root_dir_start;
	}

	uint32_t fat12_16_root_dir_end() const {
		return data_area_start;
	}

	uint32_t  cluster_to_sector(cluster_t clust) const {
		uint32_t rv = data_area_start;
		return rv + (clust - 2) * secs_per_cluster;
	}

	cluster_t sector_to_cluster(uint32_t sec) const {
		sec -= data_area_start;
		return sec / secs_per_cluster + 2;
	}

	friend class FatFileSystem;
	friend class FileAllocationTable;
	friend class FileAllocationTable32;
};

class FatOrdinaryFile {
protected:
	uint32_t length;
	FatFileSystem *filesystem;
	cluster_t first_cluster;
public:
        FatOrdinaryFile(FatFileSystem *fs, cluster_t _cluster, uint32_t _length) {
		length = _length;
		filesystem = fs;
		first_cluster = _cluster;
	}

	bool get_first_sector(SectorHandle& handle);
	bool get_next_sector(uint32_t current_sector, SectorHandle& handle);
};

class FatDirectoryFile {
public:
	virtual bool get_first_sector(SectorHandle& handle) = 0;
	virtual bool get_next_sector(uint32_t current_sector, SectorHandle& handle) = 0;
};

class FatFixedRootDirFile : public FatDirectoryFile {
protected:
	FatFileSystem *filesys;
        FatFixedRootDirFile(FatFileSystem *fs) : filesys(fs) { }
public:
	virtual bool get_first_sector(SectorHandle& handle);
	virtual bool get_next_sector(uint32_t current_sector, SectorHandle& handle);
	friend class FatFileSystem;
};

class FatDirEntry;
class FatDirPointer {
private:
	SectorHandle current_sector;
	uint32_t in_sector_pointer;
	FatDirectoryFile *directory;
public:
	FatDirPointer() : in_sector_pointer(0) {
	}

	void rewind() {
		current_sector.release();
		directory->get_first_sector(current_sector);
		in_sector_pointer = 0;
	}

	void open(FatDirectoryFile *dir) {
		directory = dir;
		rewind();
	}

	bool get_next_entry(FatDirEntry& entry);
	void close();
};

class FatDirectoryFile;
class FileAllocationTable;

class FatFileSystem
{
protected:
	FatInfo info;
        FileAllocationTable *t;
	BlockDevice& device;
	FatDirectoryFile *root_dir;
	FileAllocationTable *fat_reader;

	void create_fat_reader();
public:
        const FatInfo& get_info() const {
		return info;
	}
	
	FatFileSystem(BlockDevice& dev) : device(dev) {
		info.initialize(dev);
		info.print_info();

		if (info.fat_size != 32) {
			root_dir = new FatFixedRootDirFile(this);
		}
		create_fat_reader();
	}

	FileAllocationTable& get_fat_reader() const {
		return *fat_reader;
	}

	BlockDevice& get_device() const {
		return device;
	}

	FatDirectoryFile *get_root_directory() const {
		return root_dir;
	}

	size_t    read(cluster_t&, fat_off_t&, size_t, void*);
	cluster_t cluster_seek_forward(cluster_t current, 
		fat_off_t new_offset, fat_off_t current_offset);

	friend class FileAllocationTable;
	friend class FileAllocationTable32;
	friend class FatDir;
};

class FileAllocationTable
{
protected:
	FatFileSystem& fs;
public:
	FileAllocationTable(FatFileSystem& filesystem) : fs(filesystem) { }
	virtual cluster_t get_next_cluster(cluster_t current) = 0;
};

class FileAllocationTable16 : public FileAllocationTable
{
public:
	FileAllocationTable16(FatFileSystem& filesystem) : FileAllocationTable(filesystem) { }

	cluster_t get_next_cluster(cluster_t current) {
                if (current < 2 || current >= fs.get_info().n_clusters + 2) {
			return (cluster_t)0xFFFFFFFF;
		}

		uint32_t offset     = (current << 1);
		uint32_t fat_sector = offset >> fs.get_info().sector_bits;

		fat_sector += fs.get_info().fat_start_sector;

		SectorHandle sh;
		fs.get_device().acquire_sector(fat_sector, sh);
		uint16_t *ptr = (uint16_t *)sh.get_data();
		offset  &= fs.get_info().sector_offset_mask;
		offset >>= 1;
		cluster_t cluster = ptr[offset];
		sh.release();

                if (cluster < 2 || cluster >= fs.get_info().n_clusters + 2) {
			return (cluster_t)0xFFFFFFFF;
		}

		return cluster;
	}
};

class FileAllocationTable32 : public FileAllocationTable
{
public:
	FileAllocationTable32(FatFileSystem& filesystem) : FileAllocationTable(filesystem) { }

	cluster_t get_next_cluster(cluster_t current) {
                if (current < 2 || current >= fs.get_info().n_clusters + 2) {
			return (cluster_t)0xFFFFFFFF;
		}

		uint32_t offset     = (current << 2);
		uint32_t fat_sector = offset >> fs.get_info().sector_bits;
		fat_sector += fs.get_info().fat_start_sector;

		SectorHandle sh;
		fs.get_device().acquire_sector(fat_sector, sh);
		uint32_t *ptr = (uint32_t *)sh.get_data();
		offset  &= fs.get_info().sector_offset_mask;
		offset >>= 2;
		cluster_t cluster = ptr[offset] & 0x0FFFFFFF;
		sh.release();

                if (cluster < 2 || cluster >= fs.get_info().n_clusters + 2) {
			return (cluster_t)0xFFFFFFFF;
		}

		// 28 bits only!
		return cluster;
	}
};

class FatInode {
};

/* not MSB compliant */
class FatDirEntry {
public:
	union {
		uint8_t contents[32];
		struct {
			char     filename[11];
                        uint8_t  attributes;
			uint8_t  _reserved_1;
                        uint8_t  fine_ctime;
			uint16_t create_time;
			uint16_t create_date;
			uint16_t access_date;
			uint16_t hcluster;
			uint16_t modify_time;
			uint16_t modify_date;
			uint16_t lcluster;
			uint32_t file_size;
 		} direntry;
		struct {
			uint8_t  sequence;
			uint8_t  name_chars_1[10];
			uint8_t  attributes;
			uint8_t  _reserved_1;
			uint8_t  cksum;
			uint8_t  name_chars_2[12];
			uint16_t _reserved_2;
			uint8_t  name_chars_3[4];
		} lfn_entry;
	};

	static const int ATTR_READ_ONLY = 0x01;
	static const int ATTR_HIDDEN    = 0x02;
	static const int ATTR_SYSTEM    = 0x04;
	static const int ATTR_VOL_LABEL = 0x08;
	static const int ATTR_DIRECTORY = 0x10;
	static const int ATTR_ARCHIVE   = 0x20;

	FatDirEntry() {
        }

	FatDirEntry(const uint8_t *data) {
		read(data);
        }

	void read(const uint8_t *data) {
		memcpy(contents, data, 32);
	}

	int get_attributes() {
		return direntry.attributes;
	}

	uint32_t get_file_size() {
		return direntry.file_size;
	}

	cluster_t get_first_cluster() {
		return (((cluster_t)direntry.hcluster) << 16) + direntry.lcluster;
	}

	bool is_empty() {
		uint8_t ch = direntry.filename[0];
		return (ch == 0xE5 || ch == 0);
	}

        bool is_last() {
		uint8_t ch = direntry.filename[0];
		return ch == 0;
	}

        bool is_lfn_part() {
		return direntry.attributes == (
			ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOL_LABEL | ATTR_READ_ONLY
		);
	}

	char *get_filename(char buf[256]) {
		int i;
		char *tmp = buf;
		for (i = 0; i < 8; i++) {
			char ch = direntry.filename[i];
			if (ch >= 'A' && ch <= 'Z') {
				ch += 32;
			}
			else if (ch == 5) {
				ch = 0xE5;
			}
			else if (ch == ' ') {
				break;
			}
			*tmp = ch;
			tmp ++;
		}
		char *extpoint = tmp;
		*extpoint = '.';
		for (i = 8; i < 11; i++) {
			char ch = direntry.filename[i];
			if (ch >= 'A' && ch <= 'Z') {
				ch += 32;
			}
			else if (ch == ' ') {
				break;
			}
			*(++tmp) = ch;
		}
		if (tmp == extpoint) {
			*tmp = 0;
		}
		else {
			*(tmp + 1) = 0;
		}

		return buf;
	}
};

class DirBrowser {
	int x;
public:
	
};

#endif 
