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
    int fd = open("/home/mubashir/banking/db/manager", O_RDWR | O_CREAT | O_TRUNC, 0744);
    struct manager c;
    c.id = 0;
    strcpy(c.username, "Av6");
    strcpy(c.password, "312");
    c.logged_in = 0;
    write(fd, &c, sizeof(struct manager));
    close(fd);
}