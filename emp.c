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

int main(){
    int fd = open("/home/mubashir/banking/db/employee", O_RDWR | O_CREAT | O_TRUNC, 0744);
    struct employee c;
    c.id = 0;
    strcpy(c.username, "Ali");
    strcpy(c.password, "39");
    strcpy(c.name, "Mushtaq Ali");
    strcpy(c.filename, "/home/mubashir/banking/db/assignedLoan/Ali");
    c.logged_in = 0;
    write(fd, &c, sizeof(struct employee));
    c.id = 1;
    strcpy(c.username, "Ishan2000");
    strcpy(c.password, "gta6");
    strcpy(c.name, "Ishan Awasti");
    strcpy(c.filename, "/home/mubashir/banking/db/assignedLoan/Ishan2000");
    c.logged_in = 0;
    write(fd, &c, sizeof(struct employee));
    close(fd);
}