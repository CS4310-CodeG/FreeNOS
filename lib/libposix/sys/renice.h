#ifndef __LIBPOSIX_RENICE_H
#define __LIBPOSIX_RENICE_H

#include <Macros.h>
#include "types.h"

/**
 * @brief Change the scheduling priority of a running process.
 *
 * @param pid Process ID of child to change priority.
 * @param priority Priority level to set process to.
 * @param stat_loc Points to an integer for storing the exit status.
 * @param options Optional flags.
 *
 * @return Process ID of the child on success or -1 on error
 */
extern C pid_t renicepid(pid_t pid, int priority, int *stat_loc, int options);

/**
 * @}
 * @}
 */

#endif /* __LIBPOSIX_RENICE_H */
