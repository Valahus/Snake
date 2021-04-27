#include "timeutil.h"

struct timeval tv;

ms_t gettom(){
	gettimeofday(&tv, NULL);
	return (ms_t)(tv.tv_sec) * 1000 + (ms_t)(tv.tv_usec) / 1000;
}

ms_t ftoms(float f){
  return (ms_t)(f*1000);
}
