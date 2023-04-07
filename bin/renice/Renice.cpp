#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Renice.h"
#include <ProcessClient.h>
#include <FreeNOS/User.h>
#include "sys/types.h"

pid_t renicepid(pid_t pid, int priority, int *stat_loc, int options)
{
    const ulong result = (ulong) ProcessCtl(pid, RenicePID, priority);

    switch ((const API::Result) (result & 0xffff))
    {
        case API::NotFound:
            errno = ESRCH;
            return (pid_t) -1;

        case API::Success:
            if (stat_loc)
            {
                *stat_loc = result >> 16;
            }
            return pid;

        default:
            errno = EIO;
            return (pid_t) -1;
    }
}

Renice::Renice(int argc, char **argv)
    : POSIXApplication(argc, argv)
{
    parser().setDescription("Alters the scheduling priority of a running");
    parser().registerPositional("PRIORITY", "Priority level to be used");
    parser().registerPositional("PROCESS_ID", "Process to have its scheduling priority changed");
    parser().registerFlag('n', "priority", "Specify the scheduling priority to be used for the process");
}

Renice::~Renice()
{  
}

Renice::Result Renice::exec()
{
    if (arguments().get("priority")) {
        const ProcessClient process;
        ProcessID pid = (atoi(arguments().get("PROCESS_ID")));
        int priority = (atoi(arguments().get("PRIORITY")));

        ProcessClient::Info info;
        const ProcessClient::Result result = process.processInfo(pid, info);

        // Check if the process exists    
        if (result != ProcessClient::Success) {
            ERROR("Process of ID '" << pid << "' not found")
            return InvalidArgument;
        }

        // Check if the new priority is valid
        if (priority < 1 || priority > 5) {
            ERROR("Unable to set priority for process " << pid)
            return InvalidArgument;
        }

        renicepid(pid, priority, 0, 0); // Set new priority to given process

        // Output
        printf("Process %d set to priority %d, from priority %d\n", pid, priority, info.kernelState.priority);
    }

    return Success;
}


