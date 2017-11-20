// Question 8 : write here the buggy function without errors
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

// For question 12:
#include <signal.h>
#include <errno.h>
#include <ptmx.h>

//#include "nmea.h"
#define KNOT_TO_KMH 1.852

int iteration = 0;

int knot_to_kmh_str(float not, size_t size, char * format, char * kmh_str)
{
    float kmh = KNOT_TO_KMH * not;
    snprintf(kmh_str, size, format, kmh);

#ifndef GPS_OK
    iteration++;
    if (iteration == 2)
    {
        puts("No Signal");
    }
#endif

    return kmh;
}

// Question 12 : write printf with a signal handler
// struct PTMX ptmx;

// void signals_handler(int signal_number)
// {
// 	/**
// 	 * Careful, it is awful!
// 	 */
// 	puts("Hey There!\n");
//     //printf("Signal catched.\n");
//     ptmxclose(&ptmx);
//     //exit(EXIT_SUCCESS);
// }


// int printf(const char *format, ...){
// 	// Used to avoid the program to stop with SIGINT
	 

// 	// signals handler
// 	struct sigaction action;
// 	action.sa_handler = signals_handler;
// 	sigemptyset(& (action.sa_mask));
// 	action.sa_flags = 0;
// 	sigaction(SIGINT, & action, NULL);
// 	//puts(format);
// 	puts("Warning! This may have no effect!\n");
// 	return EXIT_SUCCESS;
// }