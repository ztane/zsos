#ifndef __BLOCK_HH_INCLUDED__
#define __BLOCK_HH_INCLUDED__

#include "fs/filelike.hh"

class SectorHandle;
class BlockDevice : public FileLike {
protected:
	uint32_t sector_size;
public:
	bool isSeekable() const {
		return true;
	}

	uint32_t getSectorSize()   const {
		return sector_size;
	}
	virtual uint32_t getTotalSectors() const = 0;
	virtual ErrnoCode acquire_sector(uint64_t number, SectorHandle& handle);
};

class SectorHandle {
private:
	BlockDevice *dev;
	uint8_t *data;
	uint64_t sectornumber;
	uint32_t sec_size;
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

#endif

