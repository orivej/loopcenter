#include <iostream>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <vector>
#include <math.h>
#include "timetools.h"
using namespace std;


/* Returns the timestamp to the nearest microsecond, as a double: */
double AccuTime(void) {
  struct timeval tm;
  struct timezone tz;
  tz.tz_minuteswest = 0;
  tz.tz_dsttime = 0;
  gettimeofday(&tm, &tz);

  return (tm.tv_sec + 0.000001 * tm.tv_usec);
}

/* sleeps sleeptime seconds, to a supposed nanosecond accuracy! */
void AccuSleep(double sleeptime) {

  struct timespec rqtp, rmtp;
  rqtp.tv_sec = (int) floor(sleeptime);
  rqtp.tv_nsec = (long int) ((sleeptime - floor(sleeptime)) * 1.0E9);
  nanosleep(&rqtp, &rmtp);
}


