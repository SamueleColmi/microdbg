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
	cout << "Enable" << endl;
}

void Breakpoint::disable()
{
	cout << "Disable" << endl;
}
