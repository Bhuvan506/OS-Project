#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "datastructures.h"


int main()
{
    int fd = open("../data/Admins.dat", O_CREAT | O_RDWR, 0777);
    if (fd == -1)
    {
        printf("unable to create and open file\n");
        exit(1);
    }
    lseek(fd, 0, SEEK_SET);

    AdminCredentials user1 = 
    {
        .username = "admin1",
        .password = "123456",
    };

    write(fd, &user1, sizeof(user1));

    AdminCredentials user2 = 
    {
        .username = "admin2",
        .password = "123457",
    };
    write(fd, &user2, sizeof(user2));
    close(fd);
    return 0;
}


