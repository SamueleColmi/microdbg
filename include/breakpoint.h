#ifndef BREAKPOINT_H_
#define BREAKPOINT_H_

#include <cstdint>

#include <sys/types.h>

using namespace std;

class Breakpoint
{
public:
	Breakpoint(pid_t pid, intptr_t addr);
	Breakpoint() = default;

	void disable();
	void enable();

	intptr_t get_address();
	bool is_enabled();

private:
	uint8_t saved_instruction;
	intptr_t addr;
	bool enabled;
	pid_t pid;
};

#endif	/* BREAKPOINT_H_ */
