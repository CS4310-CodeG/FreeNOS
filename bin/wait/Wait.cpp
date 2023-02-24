#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Wait.h"
#include "sys/wait.h"

Wait::Wait(int argc, char **argv)
    : POSIXApplication(argc, argv)
{
    parser().setDescription("Stop executing for some time");
    parser().registerPositional("PROCESS_ID", "Stop executing for the given number of seconds");
}

Wait::~Wait()
{
}

Wait::Result Wait::exec()
{
    ProcessID pid;
    
    if ((pid = atoi(arguments().get("PROCESS_ID"))) <= 3) {
	ERROR("wait: `" << arguments().get("PROCESS_ID") << "': not a pid or valid job spec");
        return InvalidArgument;
    }
    
    if (waitpid(pid, 0, 0) == (pid_t) -1) {
    	ERROR("wait: pid " << arguments().get("PROCESS_ID") << " is not a child of this shell");
        return IOError;
    }

    return Success;
}
