#ifndef DEBUGGER_H_
#define DEBUGGER_H_

#include <string>
#include <vector>

#include <sys/types.h>

using namespace std;

enum class TARGET_STATUS
{
	STOPPED,
	EXITED,
	UNDEF,
	ERROR
};

class Debugger
{
public:
	Debugger(const string &program, pid_t pid);

	void run();

private:
	string program;
	pid_t child;

	bool is_prefix(const string &str, const string &of);
	vector<string> parse_cmd(const string &str);
	void handle_cmd(const string &line);
	TARGET_STATUS get_target_status();
	void continue_execution();
	void get_cmd();
};


#endif	/* DEBUGGER_H_ */
