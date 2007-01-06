#ifndef INIT_HH
#define INIT_HH 1


namespace init {

class Init {
private:
	Init *next;
	friend void setup(Init *ptr);
	friend void run();
public:
	virtual ~Init();
	virtual int init() = 0;
};

extern void setup(Init *ptr);
extern void run();

};


#endif
