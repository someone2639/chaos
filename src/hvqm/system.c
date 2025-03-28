/*
 *  N64-HVQM2 library  Sample program
 *
 *  FILE : system.c (boot program/system utility)
 *
 *  Copyright (C) 1998,1999 NINTENDO Co.,Ltd.
 *
 */

/* 1998-12-15 */

#include <ultra64.h>
#include "system.h"

/***********************************************************************
 * Main thread
 ***********************************************************************/
OSThread hvqmThread;
static u64 hvqmThreadStack[STACKSIZE / 8];

/***********************************************************************
 *
 * mainproc - Main thread procedure
 *
 ***********************************************************************/
static void mainproc(void *arg) {
    /* To main function */
    Main(arg);

    /* To idle state */
    osSetThreadPri(0, 0);
    for (;;)
        ;
    /* NOT REACHED */
}

void createHvqmThread(void *arg) {
    bzero(&hvqmThread, sizeof(OSThread));
    osCreateThread(&hvqmThread, 7, mainproc, arg, hvqmThreadStack + STACKSIZE / 8, 11);
}

