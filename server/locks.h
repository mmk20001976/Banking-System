#ifndef LOCKS_H
#define LOCKS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "/home/mubashir/banking/utilities/utilities.h"
#include "/home/mubashir/banking/server/map.h"

int acquire_read_lock(int fd, off_t start, off_t len);
int acquire_write_lock(int fd, off_t start, off_t len);
void release_lock(int fd, off_t start, off_t len);
#endif