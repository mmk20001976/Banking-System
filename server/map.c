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
#include "locks.h"
#include "customer.h"
void add_user(char *username, int index, struct user_map **m)
{
    struct user_map *record = malloc(sizeof(struct user_map));
    strcpy(record->username, username);
    record->index = index;
    printf("username: %s\n", record->username);
    printf("index: %d\n", record->index);
    printf("record added\n");
    HASH_ADD_STR(*m, username, record);
}
struct user_map *find_user(char *username, struct user_map **m)
{
    struct user_map *record;
    HASH_FIND_STR(*m, username, record);
    return record;
}