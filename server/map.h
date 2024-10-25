#ifndef MAP_H
#define MAP_H
#include "/home/mubashir/banking/uthash/include/uthash.h"

struct user_map
{
    char username[30];
    int index;
    UT_hash_handle hh;
};
void add_user(char *username, int index, struct user_map **m);
struct user_map *find_user(char *username, struct user_map **m);
#endif

// void add_user(char *username, off_t offset, struct user_map **m)
// {
//     struct user_map *b = malloc(sizeof(struct user_map));
// }
