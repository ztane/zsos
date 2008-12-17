#ifndef _IDE_REQUEST_
#define _IDE_REQUEST_ 1

#include <cstddef>

namespace ide {

struct ide_request_t {
	enum { READ, WRITE } type;
	int drive;
	unsigned long long block;
	size_t count;
	void *data;
};

};

#endif
