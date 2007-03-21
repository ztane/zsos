#ifndef SOFTIRQ_H_INCLUDED
#define SOFTIRQ_H_INCLUDED

const int numSoftIrqVectors = 32;

bool registerSoftIrq(int vector, void (*routine)(int vector));
bool unregisterSoftIrq(int vector);
bool triggerSoftIrq(int vector);
void initSoftIrq();

#endif
