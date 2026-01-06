#ifndef NET_H
#define NET_H

#define MSG_SIZE 10

#include <stddef.h>

struct WorldInfo;   // Forward declaration of MPI world info.

// Pass a message using Remote Memory Access.
int rma(float* buf, size_t buf_size, struct WorldInfo* info, double* execution_time);

#endif // NET_H
