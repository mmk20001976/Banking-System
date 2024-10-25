#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "/home/mubashir/banking/server/map.h"
#include "locks.h"

void Customer_functionalities(const int client_socket, struct user_map *ptr);
void Customer_login(const int client_socket, struct login_message *msg);
#endif
