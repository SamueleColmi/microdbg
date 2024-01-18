#include <cerrno>
#include <cstring>
#include <iostream>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>

#include "debugger.h"

using namespace std;

static void execute_target(const string &program_name)
{
	/**
	 * PTRACE_TRACEME request: the child process asks the OS kernel to let
	 * its parent trace it.
	 * NOTICE: All subsequent calls to exec() by this (child) process will
	 * cause a SIGTRAP to be sent to it, giving the parent a chance to gain
	 * control before the new program begins execution.
	 */
	if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == -1) {
		cerr << "Error! Initialization of ptrace failed: -" << errno << " (" <<	strerror(errno) << ")" << endl;
		goto exit_failure;
	}

	/**
	 * The call to execl() will cause the OS to stop the child process
	 * though a SIGTRAP signal and, hence, a SIGSTOP signal is sent to its
	 * parent.
	 */
	if (execl(program_name.c_str(), program_name.c_str(), nullptr) == -1) {
		cerr << "Error! Execution of " << program_name << " failed: -" << errno << " (" << strerror(errno) << ")" << endl;
		goto exit_failure;
	}

	return;

exit_failure:
	exit(EXIT_FAILURE);
}

static void execute_debugger(const string &program_name, pid_t pid)
{
	Debugger dbg {program_name, pid};
	dbg.run();
}

int main(int argc, char *argv[])
{
	pid_t pid;

	if (argc != 2) {
		cerr << "Error! Wrong number of arguments\nUsage: ./microdbg <program-name-full-path>" << endl;
		goto exit_failure;
	}

	pid = fork();
	if (pid == 0) {
		/* child */
		execute_target(argv[1]);
	} else if (pid > 0) {
		/* parent */
		execute_debugger(argv[1], pid);
	} else {
		/* fork() failed */
		cerr << "Error! Fork of child process failed: -" << errno << " (" << strerror(errno) << ")" << endl;
		goto exit_failure;
	}

	return 0;

exit_failure:
	exit(EXIT_FAILURE);
}
