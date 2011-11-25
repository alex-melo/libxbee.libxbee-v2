#ifndef __XBEE_XSYS_H
#define __XBEE_XSYS_H

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

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#define __XBEE_XSYS_LOAD_H
#if defined(__GNUC__) /* ------- */
#include "xsys_linux.h"
#elif defined(_WIN32) /* ------- */
#include "xsys_win32.h"
#else /* ----------------------- */
#error Unsupported OS
#endif /* ---------------------- */
#undef __XBEE_XSYS_LOAD_H

#include "errors.h"
#include "log.h"

/* file I/O */
int xsys_open(char *path, int flags);
int xsys_close(int fd);
ssize_t xsys_read(int fd, void *buf, size_t count);
ssize_t xsys_write(int fd, void *buf, size_t count);

FILE *xsys_fopen(char *path, char *mode);
FILE *xsys_fdopen(int fd, char *mode);
int xsys_fclose(FILE *fp);

size_t xsys_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t xsys_fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream);
int xsys_fflush(FILE *stream);
int xsys_ferror(FILE *stream);
int xsys_feof(FILE *stream);

int xsys_select(FILE *stream, struct timeval *timeout);

int xsys_disableBuffer(FILE *stream);


/* configuration */
int xsys_setupSerial(int fd, FILE *stream, int baudrate);


/* threads */
int xsys_thread_create(xsys_thread *thread, void*(*start_routine)(void*), void *arg);
int xsys_thread_cancel(xsys_thread thread);
int xsys_thread_join(xsys_thread thread, void **retval);
int xsys_thread_tryjoin(xsys_thread thread, void **retval);


/* mutexes */
int xsys_mutex_init(xsys_mutex *mutex);
int xsys_mutex_destroy(xsys_mutex *mutex);
int xsys_mutex_lock(xsys_mutex *mutex);
int xsys_mutex_trylock(xsys_mutex *mutex);
int xsys_mutex_unlock(xsys_mutex *mutex);


/* semaphores */
int xsys_sem_init(xsys_sem *sem);
int xsys_sem_destroy(xsys_sem *sem);
int xsys_sem_wait(xsys_sem *sem);
int xsys_sem_post(xsys_sem *sem);

#endif /* __XBEE_XSYS_H */
