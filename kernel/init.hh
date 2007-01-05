#ifndef INIT_HH
#define INIT_HH 1


namespace init {

class Init {
public:
	virtual ~Init();
	virtual int init() = 0;
};


extern void setup(Init *ptr);
extern void run();

};


#endif
