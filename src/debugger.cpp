#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "linenoise.h"

#include "debugger.h"

using namespace std;

Debugger::Debugger(const string &program_name, pid_t child_pid)
{
	program = program_name;
	child = child_pid;
}

void Debugger::run()
{
	TARGET_STATUS state;

	while (true) {
		state = get_target_status();

		switch (state) {
		case TARGET_STATUS::STOPPED:
			get_cmd();
			break;
		case TARGET_STATUS::EXITED:
		case TARGET_STATUS::UNDEF:
		case TARGET_STATUS::ERROR:
		default:
			goto terminate;
		}
	}

terminate:
	cout << "Terminating microdbg, bye!" << endl;
	exit(EXIT_SUCCESS);
}

TARGET_STATUS Debugger::get_target_status()
{
	int status;

	if (waitpid(child, &status, 0) == -1) {
		cerr << "Error! microdbg failed to retrieve target status: -" << errno << " (" << strerror(errno) << ")" << endl;
		return TARGET_STATUS::ERROR;
	}

	if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
		return TARGET_STATUS::STOPPED;
	}

	if (WIFEXITED(status)) {
		cout << "Target program exited with status " << WEXITSTATUS(status) << endl;
		return TARGET_STATUS::EXITED;
	} else {
		cerr << "Error! Target program is in undefined state" << endl;
		return TARGET_STATUS::UNDEF;
	}
}

void Debugger::get_cmd()
{
	char *line;

	line = linenoise("microdbg> ");
	if (line != nullptr) {
		handle_cmd(line);
		linenoiseHistoryAdd(line);
		linenoiseFree(line);
	}
}

void Debugger::handle_cmd(const string &line)
{
	vector<string> args;

	/**
	 * args[0] == command
	 * args[1] == <address>
	 *
	 * TODO:  add check for valid address format, assuming 0xDEADBEEF or DEADBEEF
	 */
	args = parse_cmd(line);

	if (is_prefix(args[0], "continue")) {
		continue_execution();
	} else if (is_prefix(args[0], "breakpoint")) {
		set_breakpoint_at(stol(args[1], 0, 16));
		get_cmd();	// TO REMOVE
	} else {
		cerr << "Error! Unknown command \"" << args[0] << "\"" << endl;
		get_cmd();
	}
}

vector<string> Debugger::parse_cmd(const string &str)
{
	stringstream ss {str};
	vector<string> out;
	string token;

	while (getline(ss, token, ' ')) {
		out.push_back(token);
	}

	if (out.size() != 2)
		out.push_back("0x0");

	return out;
}

bool Debugger::is_prefix(const string &cmd, const string &of)
{
	if (cmd.size() > of.size())
		return false;

	return equal(cmd.begin(), cmd.end(), of.begin());
}

void Debugger::continue_execution()
{
	ptrace(PTRACE_CONT, child, nullptr, nullptr);
}

void Debugger::set_breakpoint_at(intptr_t addr)
{
	cout << "Setting breakpoint at: 0x" << hex << addr << endl;

	breakpoints.emplace(piecewise_construct, forward_as_tuple(addr), forward_as_tuple(child, addr));
	breakpoints[addr].enable();
}
