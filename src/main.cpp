#include <cerrno>
#include <cstring>
#include <iostream>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

static void execute_debuggee(const string &program_name)
{
	if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == -1) {
		cerr << "Error! Initialization of ptrace failed: -" << errno <<
			" (" <<	strerror(errno) << ")" << endl;

		goto exit;
	}

	if (execl(program_name.c_str(), program_name.c_str(), nullptr) == -1) {
		cerr << "Error! Execution of " << program_name << " failed: -" \
			<< errno << " (" << strerror(errno) << ")" << endl;

		goto exit;
	}

	return;

exit:
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	pid_t pid;

	if (argc != 2) {
		cerr << "Error! Wrong number of arguments\n \
			Usage: ./microdbg <program-name-full-path>" << endl;
		exit(EXIT_FAILURE);
	}

	pid = fork();
	if (pid == -1) {
		cerr << "Error! Fork of child process failed: -" << errno <<
			" (" << strerror(errno) << ")" << endl;
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
		/* child */
		execute_debuggee(argv[1]);
	} else {
		/* parent */
		int status;

		if (waitpid(pid, &status, 0) == -1)
			exit(EXIT_FAILURE);
		else if (WIFEXITED(status)) {
			cout << "Child process exited with status " <<
				WEXITSTATUS(status) << endl;
		}
	}


	return 0;
}
