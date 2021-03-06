#include "fat.hh"
#include "kernel/drivers/block.hh"
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

ErrnoCode FatInfo::initialize(BlockDevice& dev)
{
	bool fat16_32 = false;
	uint32_t tmp;

	SectorHandle bpb;
	ErrnoCode rc;

	if ((rc = dev.acquire_sector(0, bpb)) != NOERROR) {
		printk("Failed to read first sector\n");
		return rc;
	}

	uint8_t *data = bpb.get_data();
	if (U16(OMAGIC) != MAGIC) {
		printk("bytes per sector INVLID MAKING\n");
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
			printk("bytes per sector not a power of 2\n");
			goto error_exit;
	}

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
			printk("secs per cluster not a power of 2\n");
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
		printk("Total sectors on BPB > size of partition\n");
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
	return NOERROR;

error_exit:
	bpb.release();
	return 	EINVAL;
}

void FatInfo::print_info() const {
	kout << "Bytes per sector: " << bytes_per_sector << endl;
	kout << "Secs per cluster: " << secs_per_cluster << endl;
	kout << "Total reserved:   " << total_reserved_sectors << endl;
	kout << "Number of FATs:   " << num_fats << endl;
	kout << "Root entries:     " << num_root_entries << endl;
	kout << "Total sectors:    " << total_sectors << endl;
	kout << "Media type:       " << media_type << endl;
	kout << "Sectors per fat:  " << sectors_per_fat << endl;
	kout << "Sectors:          " << num_sectors << endl;
	kout << "Heads:            " << num_heads << endl;
	kout << "Hidden sectors:   " << hidden_sectors << endl;
	kout << "Num clusters:     " << n_clusters << endl;
	kout << "File system type: FAT" << fat_size << endl;
        kout << "Fat start:        " << fat_start_sector << endl;
	kout << "Root dir start:   " << root_dir_start << endl;
        kout << "Data area start:  " << data_area_start << endl;
	printk("Cluster offset bits: %d - cluster offset mask %08X\n", 
		cluster_bits, cluster_offset_mask);
	printk("Sector offset bits: %d - sector offset mask %08X\n", 
		sector_bits, sector_offset_mask);

	kout << endl << endl;
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
					for (size_t i = 0; i < sizeof(long_filename) / 2; i++) {
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

	void rewind() {
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
