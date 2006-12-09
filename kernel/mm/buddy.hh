#ifndef __BUDDY_HH_INCLUDED__
#define __BUDDY_HH_INCLUDED__

typedef int pageaddr_t;

class BuddyZone {
private:
	pageaddr_t __get_free_pages(int n);
public:	
	BuddyZone(pageaddr_t start, pageaddr_t end);
	pageaddr_t get_free_page() {
		return __get_free_pages(1);
	}
	pageaddr_t get_free_pages(unsigned int n) {
		switch (n) {
			case 1:
			case 2:
			case 4:
			case 8:
			case 16:
			case 32:
			case 64:
			case 128:
			case 256:
			case 512:
				return __get_free_pages(n)
		}
		return -1;
	}
}

#endif
