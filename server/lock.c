#include "locks.h"

int acquire_read_lock(int fd, off_t start, off_t len)
{
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK; // Read lock
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;

    return fcntl(fd, F_SETLKW, &lock); // Blocking until lock is acquired
}

int acquire_write_lock(int fd, off_t start, off_t len)
{
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK; // Write lock
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;

    return fcntl(fd, F_SETLKW, &lock); // Blocking until lock is acquired
}
void release_lock(int fd, off_t start, off_t len)
{
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_UNLCK; // Unlock
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;

    fcntl(fd, F_SETLK, &lock); // Release the lock
}