#ifndef __XBEE_INTERNAL_H
#define __XBEE_INTERNAL_H

/*
  libxbee - a C library to aid the use of Digi's Series 1 XBee modules
            running in API mode (AP=2).

  Copyright (C) 2009  Attie Grande (attie@attie.co.uk)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <pthread.h>
#include <semaphore.h>

#include "xbee.h"
#include "ll.h"

/* ######################################################################### */

struct xbee_device {
	char *path;
	int fd;
	FILE *f;
	int baudrate;
	int ready;
};

struct xbee {
	int running;
	struct xbee_device device;
	struct xbee_mode *mode;
	
	struct ll_head txList; /* data is struct bufData containing 'Frame Data' (no start delim, length or checksum) */
	pthread_t txThread;
	sem_t txSem;
	int txRunning;
	
	pthread_t rxThread;
	int rxRunning;
};

/* ######################################################################### */

struct xbee_conAddress {
	unsigned char addr16_enabled;
	unsigned char addr16[2];
	
	unsigned char addr64_enabled;
	unsigned char addr64[8];
	
	unsigned char frameID_enabled;
	unsigned char frameID;
};

struct xbee_con {
	struct xbee_conType *conType;
	
	struct xbee_conAddress address;
	
	int rxPackets;
	int txPackets;
	
	struct ll_head rxList; /* data is struct xbee_pkt */
};

struct bufData {
	int len;
	unsigned char buf[1];
};

/* functions are given:
    xbee      the 'master' libxbee struct
    handler   is the handler (used to get packet ID, conType, and potentially recursive calling)
    buf       is a double pointer so that:
                Rx functions may take charge of the packet (setting *buf = NULL will prevent libxbee from free'ing buf)
                Tx functions may return the packet
    con       is used to identify the destination address
                Rx is ONLY used for the address, returns the addressing info to _xbee_rxHandlerThread() so it can be added to the correct connection
                Tx is a valid pointer
		pkt				is used to convey the packet information
								Rx allows the handler to return the populated packet struct (is ** so that realloc may be called)
								Tx allows the user to 'hand write' a packet
*/
struct xbee_pktHandler;
typedef int(*xbee_pktHandlerFunc)(struct xbee *xbee,
                                  struct xbee_pktHandler *handler,
                                  struct bufData **buf,
                                  struct xbee_con *con,
                                  struct xbee_pkt **pkt);

/* ADD_HANDLER(packetID, dataStarts, functionName) */
#define ADD_HANDLER(a, b) \
	{ (a), (#b), (b), NULL, NULL }
#define ADD_HANDLER_TERMINATOR() \
	{ 0, NULL, NULL, NULL, NULL }

/* a NULL handler indicates the end of the list */
struct xbee_pktHandler  {
	unsigned char id;
	char *handlerName; /* used for debug output, identifies handler function */
	xbee_pktHandlerFunc handler;
	void *rxData; /* used by listen thread (listen.c) */
	struct xbee_conType *conType;
};

/* ADD_TYPE_RXTX(rxID, txID, name) */
#define ADD_TYPE_RXTX(a, b, c) \
	{ 1, (a), 1, (b), (c),  NULL }
#define ADD_TYPE_RX(a, b) \
	{ 1, (a), 0,  0 , (b),  NULL }
#define ADD_TYPE_TX(a, b) \
	{ 0,  0,  1, (a), (b),  NULL }
#define ADD_TYPE_TERMINATOR() \
	{ 0,  0,  0,  0 , NULL, NULL }

/* a NULL name indicates the end of the list */
struct xbee_conType {
	unsigned char rxEnabled;
	unsigned char rxID;
	unsigned char txEnabled;
	unsigned char txID;
	char *name;
	void *data;
	struct ll_head conList; /* data is struct xbee_con */
};

struct xbee_mode {
	struct xbee_pktHandler *pktHandlers;
	struct xbee_conType *conTypes;
	char *name;
};

/* ######################################################################### */

#define xbee_threadStart(a, b, c) _xbee_threadStart((a), (b), (void*(*)(void*))(c), (#c))
int _xbee_threadStart(struct xbee *xbee, pthread_t *thread, void*(*startFunction)(void*), char *startFuncName);

#endif /* __XBEE_INTERNAL_H */

