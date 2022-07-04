#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "datastructures.h"


int main()
{
    int fd = open("../data/Users.dat", O_CREAT | O_RDWR, 0777);
    if (fd == -1)
    {
        printf("unable to create and open file\n");
        exit(1);
    }
    lseek(fd, 0, SEEK_SET);

    User user1 = 
    {
        .id = 1,
        .password = "abc123",
        .acctype = singleAccount,
        .accid = 1,
    };

    write(fd, &user1, sizeof(user1));

    User user2 = 
    {
        .id = 2,
        .password = "abc124",
        .acctype = singleAccount,
        .accid = 2,
    };

    write(fd, &user2, sizeof(user2));
    
    User user3 = 
    {
        .id = 3,
        .password = "abc125",
        .acctype = singleAccount,
        .accid = 3,
    };

    write(fd, &user3, sizeof(user3));
    
    User user4 = 
    {
        .id = 4,
        .password = "abc126",
        .acctype = singleAccount,
        .accid = 4,
    };

    write(fd, &user4, sizeof(user4));
    close(fd);
    return 0;
}


