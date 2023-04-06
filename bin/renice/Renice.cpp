#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Renice.h"
#include "sys/renice.h"

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
    // to be added
}
