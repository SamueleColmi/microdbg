#include <sys/ptrace.h>

#include "breakpoint.h"

using namespace std;

Breakpoint::Breakpoint(pid_t pid, intptr_t addr)
{
	this->pid = pid;
	this->addr = addr;

	saved_instruction = 0;
	enabled = false;
}

bool Breakpoint::is_enabled()
{
	return enabled;
}

intptr_t Breakpoint::get_address()
{
	return addr;
}


#include <iostream>
void Breakpoint::enable()
{
	uint64_t peek_data;
	uint64_t poke_data;
	uint64_t int3 = 0xcc;

	peek_data = ptrace(PTRACE_PEEKDATA, pid, addr, nullptr);
	saved_instruction = static_cast<uint8_t>(peek_data & 0xff);
	poke_data = ((peek_data & ~0xff) | int3);
	ptrace(PTRACE_POKEDATA, pid, addr, poke_data);

	enabled = true;
}

void Breakpoint::disable()
{
	uint64_t peek_data;
	uint64_t poke_data;

	peek_data = ptrace(PTRACE_PEEKDATA, pid, addr, nullptr);
	poke_data = ((peek_data & ~0xff) | saved_instruction);
	ptrace(PTRACE_POKEDATA, pid, addr, poke_data);

	enabled = false;
}
