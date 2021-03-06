/* Basic platform-independent macro definitions for mutexes,
   thread-specific data and parameters for malloc.
   Copyright (C) 2003-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _MALLOC_MACHINE_H
#define _MALLOC_MACHINE_H

#undef thread_atfork_static

#include <atomic.h>
#include <bits/libc-lock.h>

/* Assume hurd, with cthreads */

/* Cthreads `mutex_t' is a pointer to a mutex, and malloc wants just the
   mutex itself.  */
#undef mutex_t
#define mutex_t struct mutex

#undef mutex_init
#define mutex_init(m) (__mutex_init(m), 0)

#undef mutex_lock
#define mutex_lock(m) (__mutex_lock(m), 0)

#undef mutex_unlock
#define mutex_unlock(m) (__mutex_unlock(m), 0)

#define mutex_trylock(m) (!__mutex_trylock(m))

#define thread_atfork(prepare, parent, child) do {} while(0)
#define thread_atfork_static(prepare, parent, child) \
 text_set_element(_hurd_fork_prepare_hook, prepare); \
 text_set_element(_hurd_fork_parent_hook, parent); \
 text_set_element(_hurd_fork_child_hook, child);

/* No we're *not* using pthreads.  */
#define __pthread_initialize ((void (*)(void))0)

/* thread specific data for glibc */

#include <bits/libc-tsd.h>

typedef int tsd_key_t[1];	/* no key data structure, libc magic does it */
__libc_tsd_define (static, void *, MALLOC)	/* declaration/common definition */
#define tsd_key_create(key, destr)	((void) (key))
#define tsd_setspecific(key, data)	__libc_tsd_set (void *, MALLOC, (data))
#define tsd_getspecific(key, vptr)	((vptr) = __libc_tsd_get (void *, MALLOC))

#define nvm_tsd_setspecific(key, data)  __libc_nvm_tsd_set (void *, MALLOC, (data))
#define nvm_tsd_getspecific(key, vptr)  ((vptr) = __libc_nvm_tsd_get (void *, MALLOC))
/* madvise is a stub on Hurd, so don't bother calling it.  */

#include <sys/mman.h>

#undef	__madvise
#define __madvise(addr, len, advice)	\
  ((void) (addr), (void) (len), (void) (advice))

#include <sysdeps/generic/malloc-machine.h>

#endif /* !defined(_MALLOC_MACHINE_H) */
