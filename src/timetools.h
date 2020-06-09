#ifndef TIMETOOLS_H
#define TIMETOOLS_H

/* Header file for some tools to make conversions between different kinds
   of date and time formats. */

#include <iostream>
#include <string>
#include <time.h>
using namespace std;


/* Returns the timestamp to the nearest microsecond, as a double: */
double AccuTime(void);

/* sleeps sleeptime seconds, to a supposed nanosecond accuracy! */
void AccuSleep(double sleeptime);

#endif // TIMETOOLS_H
