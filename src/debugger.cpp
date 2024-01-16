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
	int status;

	while (true) {
		if (waitpid(child, &status, 0) == -1) {
			cerr << "Error! microgdb failed to retrieve target \
				status: -" << errno << " (" << strerror(errno)
				<< ")" << endl;
			exit(EXIT_FAILURE);
		} else if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
			get_cmd();
		} else if (WIFEXITED(status)) {
			cout << "Target program exited with status " <<
				WEXITSTATUS(status) << ".\nTerminating microgdb."
				<< endl;
			exit(EXIT_SUCCESS);
		}
	}
}

void Debugger::get_cmd()
{
	char *line;

	line = linenoise("microgdb> ");
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

	args = split(line);
	cmd = args[0];

	if (is_prefix(cmd, "continue"))
		continue_execution();
	else
		cerr << "Error! Unknown command " << cmd << endl;
}

void Debugger::continue_execution()
{
	ptrace(PTRACE_CONT, child, nullptr, nullptr);
}

bool Debugger::is_prefix(const string &cmd, const string &of)
{
	if (cmd.size() > of.size())
		return false;

	return equal(cmd.begin(), cmd.end(), of.begin());
}

vector<string> Debugger::split(const string &str)
{
	stringstream ss {str};
	vector<string> out;
	string token;

	while (getline(ss, token, ' ')) {
		out.push_back(token);
	}

	return out;
}

