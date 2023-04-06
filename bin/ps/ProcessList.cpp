#include <Types.h>
#include <Macros.h>
#include <stdio.h>
#include <unistd.h>
#include <ProcessClient.h>
#include "ProcessList.h"

ProcessList::ProcessList(int argc, char **argv)
    : POSIXApplication(argc, argv)
{
    parser().setDescription("Output system process list");
    parser().registerFlag('l', "PRIORITY", "Outputs a list of priorities. 1-5, 3 being the default.");//adding the -l function
}

ProcessList::Result ProcessList::exec()
{
    const ProcessClient process;
    String out;

    if(arguments().get("PRIORITY")) {
        out << "ID  PARENT  PRIORITY USER GROUP STATUS     CMD\r\n";
    } else {
        // Printing the header
        out << "ID  PARENT  USER GROUP STATUS     CMD\r\n";
    }
    
    // Loops through each processes
    for (ProcessID pid = 0; pid < ProcessClient::MaximumProcesses; pid++)
    {
        ProcessClient::Info info;

        const ProcessClient::Result result = process.processInfo(pid, info);
        if (result == ProcessClient::Success)
        {
            DEBUG("PID " << pid << " state = " << *info.textState);

            // Output a line
            if(arguments().get("PRIORITY")) {
                char line[128];
                snprintf(line, sizeof(line),
                        "%3d %7d %8d %4d %5d %10s %32s\r\n",
                        pid, info.kernelState.parent, info.kernelState.priority,
                        0, 0, *info.textState, *info.command);
                out << line;
            } 
            else {
                char line[128];
                snprintf(line, sizeof(line),
                        "%3d %7d %4d %5d %10s %32s\r\n",
                        pid, info.kernelState.parent,
                        0, 0, *info.textState, *info.command);
                out << line;
            }
        }
    }

    write(1, *out, out.length());
    return Success;
}
