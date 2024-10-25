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

int main()
{
    int fd = open("/home/mubashir/banking/db/admin", O_RDWR | O_CREAT | O_TRUNC, 0744);
    struct admin c;
    c.id = 0;
    strcpy(c.username, "liam");
    strcpy(c.password, "0007");
    c.logged_in = 0;
    write(fd, &c, sizeof(struct admin));
    close(fd);
}