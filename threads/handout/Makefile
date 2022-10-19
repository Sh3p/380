# File: Makefile
# Author: Chad Hogg
# Makefile that can be used for Lab 10

CC := gcc

CFLAGS := -Wall -Werror -O0 -g

LDLIBS := -lpthread

all : ThreadSum HelloPThreads

HelloPthreads : HelloPthreads.c
	$(CC) $(CFLAGS) -o HelloPthreads HelloPthreads.c $(LDLIBS)

ThreadSum : ThreadSum.c
	$(CC) $(CFLAGS) -o ThreadSum ThreadSum.c $(LDLIBS)
