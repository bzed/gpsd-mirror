/*
 * Simulate ANSI/POSIX conformance on platforms that don't have it
 *
 * This file is Copyright (c) 2010 by the GPSD project
 * BSD terms apply: see the file COPYING in the distribution root for details.
 */
#include <time.h>
#include <sys/time.h>

#include "compiler.h"

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#ifndef HAVE_CLOCK_GETTIME
int clock_gettime(clockid_t clk_id UNUSED, struct timespec *tp)
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    tp->tv_sec = mts.tv_sec;
    tp->tv_nsec = mts.tv_nsec;
#else
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
	return -1;
    tp->ts_sec = tv.tv_sec;
    tp->ts_nsec = tv.tv_usec * 1000;
    /* paranoid programming */
    if (1000000 <= (tv)->tv_usec) {
	(tv)->tv_usec -= 1000000;
	(tv)->tv_sec++;
    } else if (0 > (tv)->tv_usec) {
	(tv)->tv_usec += 1000000;
	(tv)->tv_sec--;
    }
    return 0;
#endif /* __MACH__ */
}
#endif /* HAVE_CLOCK_GETTIME */

/* end */
