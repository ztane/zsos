#ifndef INIT_HH
#define INIT_HH 1


namespace init {

	const int EARLY = 0;
	const int LATE  = 1;

	const int NUM_LIST = 2;

	class Init;
	extern void run(int when);

	class Init {
	private:
		Init *next;
		friend void run(int);

	protected:
		Init(int when = EARLY);
	public:
		virtual ~Init();
		virtual int init() = 0;
	};

};


#endif
