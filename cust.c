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
    int fd = open("/home/mubashir/banking/db/customer", O_RDWR | O_CREAT | O_TRUNC, 0744);
    struct customer c;

    c.id = 0;
    strcpy(c.username, "mmk");
    strcpy(c.password, "34512");
    c.balance = 5000;
    c.logged_in = 0;
    c.active = 1;
    strcpy(c.filename, "/home/mubashir/banking/db/passbook/mmk");
    c.account_no = 100;
    strcpy(c.name, "Mohammed Mubashir Khan");
    c.age = 20;
    write(fd, &c, sizeof(struct customer));

    c.id = 1;
    strcpy(c.username, "yus09");
    strcpy(c.password, "LM10");
    c.balance = 50000;
    c.logged_in = 0;
    c.active = 1;
    strcpy(c.filename, "/home/mubashir/banking/db/passbook/yus09");
    c.account_no = 101;
    strcpy(c.name, "yusuf Khan");
    c.age = 19;
    write(fd, &c, sizeof(struct customer));

    c.id = 2;
    strcpy(c.username, "Ahmed");
    strcpy(c.password, "12");
    c.balance = 23000;
    c.logged_in = 0;
    c.active = 1;
    strcpy(c.filename, "/home/mubashir/banking/db/passbook/ahmed");
    c.account_no = 102;
    strcpy(c.name, "syed Ahmed");
    c.age = 45;
    write(fd, &c, sizeof(struct customer));
    close(fd);
}
