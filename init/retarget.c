/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * retarget.c
 *
 * Created on: Mar 16, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <assert.h>
#include <_ansi.h>
#include <_syslist.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdatomic.h>

void __aeabi_unwind_cpp_pr0(void);
void __aeabi_unwind_cpp_pr1(void);
void __aeabi_unwind_cpp_pr2(void);

void __malloc_lock(struct _reent *reent) __attribute__((weak));
void __malloc_unlock(struct _reent *reent) __attribute__((weak));
caddr_t _sbrk(int incr) __attribute__((weak));
void _exit(int code) __attribute__((weak));
int raise(int sig) __attribute__((weak));
int kill(pid_t pid, int sig) __attribute__((weak));
int _chown(const char* path, uid_t owner, gid_t group) __attribute__((weak));
int _close(int fildes) __attribute__((weak));
int _execve(char* name, char** argv, char** env) __attribute__((weak));
int _fork(void) __attribute__((weak));
int _fstat(int fildes, struct stat* st) __attribute__((weak));
int _getpid(void) __attribute__((weak));
int _gettimeofday(struct timeval* ptimeval, void* ptimezone) __attribute__((weak));
int _isatty(int file) __attribute__((weak));
int _kill(int pid, int sig) __attribute__((weak));
int _link(char* existing, char* _new) __attribute__((weak));
int _lseek(int file, int ptr, int dir) __attribute__((weak));
int _open(char* file, int flags, int mode) __attribute__((weak));
int _read(int file, char* ptr, int len) __attribute__((weak));
int _readlink(const char* path, char* buf, size_t bufsize) __attribute__((weak));
int _stat(const char* file, struct stat* st) __attribute__((weak));
int _symlink(const char* path1, const char* path2) __attribute__((weak));
clock_t _times(struct tms* buf) __attribute__((weak));
int _unlink(char* name) __attribute__((weak));
int _wait(int* status) __attribute__((weak));
int _write(int file, char* ptr, int len) __attribute__((weak));

extern void *__heap;
static void* current_heap_end = &__heap;

/* This prevents the linking of libgcc unwinder code */
void __aeabi_unwind_cpp_pr0(void)
{
};

void __aeabi_unwind_cpp_pr1(void)
{
};

void __aeabi_unwind_cpp_pr2(void)
{
};

#include <reent.h>

/*
void __attribute__((weak)) __malloc_lock(struct _reent *reent __attribute((unused)))
{
	abort();
}

void __attribute__((weak)) __malloc_unlock(struct _reent *reent __attribute((unused)))
{
	abort();
}
*/

caddr_t __attribute__((weak)) _sbrk(int incr)
{
	void *current_block_address = current_heap_end;
	current_heap_end += incr;
	return (caddr_t)current_block_address;
}

void __attribute__((weak, noreturn)) _exit(int code __attribute__((unused)))
{
	while (true);
}

void __attribute__((weak, noreturn)) abort(void)
{
	_exit(1);
}

int raise(int sig __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int kill(pid_t pid __attribute__((unused)), int sig __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _chown(const char* path __attribute__((unused)), uid_t owner __attribute__((unused)), gid_t group __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _close(int fildes __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _execve(char* name __attribute__((unused)), char** argv __attribute__((unused)), char** env __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _fork(void)
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _fstat(int fildes __attribute__((unused)), struct stat* st __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _getpid(void)
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _gettimeofday(struct timeval* ptimeval __attribute__((unused)), void* ptimezone __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _isatty(int file __attribute__((unused)))
{
	errno = ENOSYS;
	return 0;
}

int __attribute__((weak)) _kill(int pid __attribute__((unused)), int sig __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _link(char* existing __attribute__((unused)), char* _new __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _lseek(int file __attribute__((unused)), int ptr __attribute__((unused)), int dir __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _open(char* file __attribute__((unused)), int flags __attribute__((unused)), int mode __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _read(int file __attribute__((unused)), char* ptr __attribute__((unused)), int len __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _readlink(const char* path __attribute__((unused)), char* buf __attribute__((unused)), size_t bufsize __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _stat(const char* file __attribute__((unused)), struct stat* st __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _symlink(const char* path1 __attribute__((unused)), const char* path2 __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

clock_t __attribute__((weak)) _times(struct tms* buf __attribute__((unused)))
{
	errno = ENOSYS;
	return ((clock_t) -1);
}

int __attribute__((weak)) _unlink(char* name __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak))_wait(int* status __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}

int __attribute__((weak)) _write(int file __attribute__((unused)), char* ptr __attribute__((unused)), int len __attribute__((unused)))
{
	errno = ENOSYS;
	return -1;
}
