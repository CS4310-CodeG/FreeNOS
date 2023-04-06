#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <String.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include "SlowCalc.h"

SlowCalc::SlowCalc(int argc, char **argv)
    : POSIXApplication(argc, argv)
{
    parser().setDescription("Alters the scheduling priority of a running");
    parser().registerPositional("PRIORITY", "Priority level to be used");
    parser().registerPositional("PROCESS_ID", "Process to have its scheduling priority changed");
    parser().registerFlag('n', "priority", "Specify the scheduling priority to be used for the process");
}

SlowCalc::~SlowCalc()
{  
}

SlowCalc::Result SlowCalc::exec()
{
    int n, i, j, k;
    double l;

    // Read max number
    n = atoi(arguments().get("NUMBER"));

    // Slow it down
    for (i = 1; i < n; i++)
        for (j = 1; j < n; j++)
            for (k = 1; k < n; k++)
                l = sqrt(i+j+k)+i+j+k;
                while (l>0) {
                    l--;
                }

    // Done
    return Success;
}
