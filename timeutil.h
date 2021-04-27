#ifndef __TIMEUTIL__
#define __TIMEUTIL__
#include <stdio.h>
#include <sys/time.h>

typedef unsigned long long ms_t;

ms_t gettom();
ms_t ftoms(float);

#endif
