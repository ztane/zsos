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

bool FatInfo::initialize(BlockDevice& dev)
{
	bool fat16_32 = false;
	uint32_t tmp;

	SectorHandle bpb;

	if (! dev.acquire_sector(0, bpb)) {
		fprintf(stderr, "Failed to read first sector\n");
		return false;
	}

	uint8_t *data = bpb.get_data();
	if (U16(OMAGIC) != MAGIC) {
		fprintf(stderr, "bytes per sector INVLID MAKING\n");
		goto error_exit;
	}

	bytes_per_sector = U16(OBPS);
	switch (bytes_per_sector) {
		case 512:
			sector_bits = 9;
			sector_offset_mask = 0x1FF;
			break;

		case 1024:
			sector_bits = 10;
			sector_offset_mask = 0x3FF;
			break;

		case 2048:
			sector_bits = 11;
			sector_offset_mask = 0x7FF;
			break;

		case 4096:
			sector_bits = 12;
			sector_offset_mask = 0xFFF;
			break;

		default:
			fprintf(stderr, "bytes per sector not a power of 2\n");
			goto error_exit;
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
			goto error_exit;
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
		goto error_exit;
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

	fat_start_sector = total_reserved_sectors;
	root_dir_start   = total_reserved_sectors + num_fats * sectors_per_fat;
	data_area_start  = root_dir_start +
		(num_root_entries * DIRENTRY_SIZE + bytes_per_sector - 1)
		/ bytes_per_sector;

	// Layout: total_reserved_sectors - 1 (BPB = 0) | FAT1, FAT2... | root dir (12, 16) | data
	n_clusters = (total_sectors - data_area_start) / secs_per_cluster;

	if (n_clusters < FAT12LIMIT) {
		fat_size = 12;
	}
	else if (n_clusters < FAT16LIMIT) {
		fat_size = 16;
	}
	else {
		fat_size = 32;
	}

	tmp = secs_per_cluster * bytes_per_sector;
	if (tmp == 0) {
		goto error_exit;
	}

	cluster_bits = 0;
	cluster_offset_mask = tmp - 1;

	while (! (tmp & 1)) {
		tmp >>= 1;
		cluster_bits ++;
	}

	bpb.release();
	return true;

error_exit:
	bpb.release();
	return false;
}

void FatInfo::print_info() const {
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
        cout << "Fat start:        " << fat_start_sector << endl;
	cout << "Root dir start:   " << root_dir_start << endl;
        cout << "Data area start:  " << data_area_start << endl;
	printf("Cluster offset bits: %d - cluster offset mask %08X\n", 
		cluster_bits, cluster_offset_mask);
	printf("Sector offset bits: %d - sector offset mask %08X\n", 
		sector_bits, sector_offset_mask);

	cout << endl << endl;
}


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

BlockDevice::BlockDevice(const char *filename)
{
	struct stat s;
	
	fp = fopen(filename, "r");
	if (! fp) {
		goto error;
        }

	filedes = fileno(fp);
	if (fstat(filedes, &s) != 0)
		goto error;

	uint32_t nblocks;
	if (s.st_mode & S_IFBLK) {
		cout << "Block device opened..." << endl;

		ioctl(filedes, BLKGETSIZE, &nblocks);

	        setSectorSize(512);
	        setTotalSectors(nblocks);
	}
	else {
		nblocks = s.st_size;
		cout << "Regular file opened..." << endl;
	        setSectorSize(512);
	        setTotalSectors(nblocks / 512);
        }
	return;
error:
	perror("BlockDevice construction failed");
	exit(1);
}

bool BlockDevice::read(void *buffer, uint32_t _offset, uint32_t number)
{
	off_t offset = _offset * sec_size;

	fseek(fp, offset, SEEK_SET);
	uint32_t rc = fread(buffer, sec_size, number, fp);
	return rc == number;
}


bool BlockDevice::acquire_sector(uint32_t number, SectorHandle& handle) {
	uint8_t *buffer = new uint8_t[sec_size];
	if (! read(buffer, number, 1)) {
		delete[] buffer;
		return false;
	}

	handle.init(this, buffer, number);
	return true;
}

void FatFileSystem::create_fat_reader() {
	fat_reader = 0;
        if (info.fat_size == 32) {
		fat_reader = new FileAllocationTable32(*this);
	}
        else if (info.fat_size == 16) {
		fat_reader = new FileAllocationTable16(*this);
	}
}

bool FatFixedRootDirFile::get_first_sector(SectorHandle& handle) {
	uint32_t first_sector = filesys->get_info().fat12_16_root_dir_start();
	return filesys->get_device().acquire_sector(first_sector, handle);
}

bool FatFixedRootDirFile::get_next_sector(uint32_t current_sector, SectorHandle& handle) {
	uint32_t next_sector = current_sector + 1;

	if (next_sector == filesys->get_info().fat12_16_root_dir_end()) {
		return false;
	}

	return filesys->get_device().acquire_sector(next_sector, handle);
}

bool FatDirPointer::get_next_entry(FatDirEntry& e) {
	while (1) {
		if (in_sector_pointer >= current_sector.sector_size()) {
			if (! directory->get_next_sector(current_sector.get_sector_number(), current_sector)) {
				return false;
			}

			in_sector_pointer = 0;
		}

		e.read(current_sector.get_data() + in_sector_pointer);
		in_sector_pointer += DIRENTRY_SIZE;
		if (! e.is_empty()) {
			return true;
		}

		// last in dir
		if (e.is_last())
			return false;

		// get next entry
	};
}

void FatDirPointer::close() {
	current_sector.release();
}

class DirEntry {
public:
	cluster_t containing_directory;
	cluster_t first_cluster;
	uint32_t  filesize;
	char      filename[260];
};

class LFNScanner {
private:
	uint16_t long_filename[260];
	uint16_t lfn_pos;
	uint16_t lfn_seq_no;
	bool     lfn_started;
	uint8_t  cksum;
	FatDirPointer *pointer;
	FatDirEntry tmpentry;

public:
	LFNScanner(FatDirPointer *ptr) {
		pointer = ptr;
		lfn_started = false;
		rewind();
	}

	void copy_lfn_chars() {
		memcpy(long_filename + lfn_pos,      tmpentry.lfn_entry.name_chars_1, 10);
		memcpy(long_filename + lfn_pos + 5,  tmpentry.lfn_entry.name_chars_2, 12);
		memcpy(long_filename + lfn_pos + 11, tmpentry.lfn_entry.name_chars_3, 4);
	}

	void update_lfn() {
		if (tmpentry.lfn_entry.sequence & 0x40) {
			cksum = tmpentry.lfn_entry.cksum;

			// take other bits than 0x40
			lfn_seq_no = tmpentry.lfn_entry.sequence & (0xFF - 0x40);

			if (lfn_seq_no > 20) {
				lfn_started = false;
				return;
			}

			lfn_seq_no --;
			lfn_pos = lfn_seq_no * 13;
			lfn_started = true;
		}
		else if (! lfn_started
				|| tmpentry.lfn_entry.cksum != cksum)
		{
			lfn_started = false;
			return;
		}
		else {
			uint32_t new_seq_no = tmpentry.lfn_entry.sequence;
                        if (new_seq_no != lfn_seq_no || lfn_seq_no == 0)
			{
				lfn_started = false;
				return;
			}
			lfn_seq_no --;
			lfn_pos -= 13;
		}

		copy_lfn_chars();
	}

	bool checksum_match() {
		uint32_t sum;
		uint32_t i;
		for (sum = i = 0; i < 11; i++) {
			sum = (((sum & 1) << 7)
				| ((sum & 0xfe) >> 1)) + tmpentry.direntry.filename[i];
		}
		return sum == cksum;
	}

	bool get_next_entry(DirEntry& e) {
		while (1) {
			if (! pointer->get_next_entry(tmpentry))
				return false;

			if (tmpentry.is_lfn_part()) {
				update_lfn();
			}
			else {
				if (lfn_started && lfn_seq_no == 0 && checksum_match()) {
					for (int i = 0; i < sizeof(long_filename) / 2; i++) {
						e.filename[i] = (char)(uint8_t)(long_filename[i]);
					}
				}
				else {
					tmpentry.get_filename(e.filename);
				}

				e.first_cluster = tmpentry.get_first_cluster();
				e.filesize = tmpentry.direntry.file_size;
				return true;
			}
		}
	}

	bool rewind() {
		long_filename[0] = 0;
		lfn_pos = 0;
		cksum = 0;
		lfn_started = 0;
		pointer->rewind();
	}
};


bool FatOrdinaryFile::get_first_sector(SectorHandle& handle) {
        if (length == 0) {
                return false;
        }

	fprintf(stderr, "sector: %d\n", filesystem->get_info().cluster_to_sector(first_cluster));
        return filesystem->get_device().acquire_sector(
                filesystem->get_info().cluster_to_sector(first_cluster), handle);
}

bool FatOrdinaryFile::get_next_sector(uint32_t current_sector, SectorHandle& handle) {
        const FatInfo& fi = filesystem->get_info();

        uint32_t in_cluster =
                (current_sector - fi.data_area_start)
                &  fi.cluster_offset_mask
                >> fi.sector_bits;

        /* inside the same cluster? */
        if (in_cluster + 1 >= fi.secs_per_cluster) {
                cluster_t cluster = fi.sector_to_cluster(current_sector);
                cluster = filesystem->get_fat_reader().get_next_cluster(cluster);

                if (cluster == 0xFFFFFFFF) {
                        return false;
                }

                return filesystem->get_device().acquire_sector
                        (fi.cluster_to_sector(cluster), handle);
        }
        else {
                return filesystem->get_device().acquire_sector
                        (current_sector + 1, handle);
        }
}



int main() {
	FILE *fp;

	BlockDevice dev("vittu.img");
	FatFileSystem f(dev);
	FatDirectoryFile *root = f.get_root_directory();

	FatDirPointer p;
	p.open(root);

	fprintf(stderr, "root dir contents\n");

	DirEntry e;
	LFNScanner lfns = LFNScanner(&p);
	while (lfns.get_next_entry(e)) {
		fprintf(stderr, "------------------\n");
		fprintf(stderr, "filename: %s\n", e.filename);

		fprintf(stderr, "filesize: %d\n", e.filesize);

	        FatOrdinaryFile fil = FatOrdinaryFile(&f, e.first_cluster, e.filesize);
	
		SectorHandle sh;
		bool res = fil.get_first_sector(sh);
		while (res) {
			uint8_t *data = sh.get_data();
			fwrite(data, sh.sector_size(), 1, stderr);
			res = fil.get_next_sector(sh.get_sector_number(), sh);
		}
		sh.release();
		fprintf(stderr, "<end>\n");
	}
	p.close();
}
