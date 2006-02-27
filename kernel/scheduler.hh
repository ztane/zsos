#ifndef SCHEDULER_HH
#define SCHEDULER_HH

#include "tasking"

//  0 - 15: HISR (Highlevel Interrupt Service Routine)
// 16 - 31: Realtime tasks
// 32 - 47: Normal tasks
#define PRIV_LEVELS 48

// NOTE: turn into a class if requires complex operations!
typedef struct _process_dir
{
	Process *process;
	struct _process_dir *next;
	struct _process_dir *prev;
} process_dir;

class Scheduler
{
	public:
		Scheduler();
		~Scheduler();
		
		void schedule();
		void add_process(Process *process, unsigned int priv);
		
	private:
		Process *current;
		process_dir tasks[PRIV_LEVELS];
};

#endif
