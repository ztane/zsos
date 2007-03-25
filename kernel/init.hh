// -*- C++ -*-

#ifndef INIT_INC
#define INIT_INC 1

class Init {
public:
	enum Time {
		EARLY,
		LATE,
		LIST_MAX
	};

	virtual ~Init();
	virtual int init() = 0;

	static void run(Time when);
protected:
	Init(Time when = EARLY);
private:
	Init *next;
};

#endif
