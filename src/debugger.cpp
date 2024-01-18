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
	string cmd;

	args = parse_cmd(line);
	cmd = args[0];

	if (is_prefix(cmd, "continue")) {
		continue_execution();
	} else {
		cerr << "Error! Unknown command \"" << cmd << "\"" << endl;
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
