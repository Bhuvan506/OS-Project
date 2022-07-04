#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include<errno.h>
#include "dboperations.h"

extern int errno;

User getUser(int userid)
{
    User validate; 
    int seekQ = userid -1; 
    int fd = open("./data/Users.dat", O_RDONLY, 0744);
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      exit(1);
    }
    static struct flock lock;

    lock.l_type = F_RDLCK;
    lock.l_start = seekQ*sizeof(User);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(User);
    lock.l_pid = getpid();
    

    int ret = fcntl(fd, F_SETLK, &lock);
    if (ret == -1) 
    {
        int err = errno;
        fprintf(stderr, "Attempt to set write lock failed %s\n",strerror(err));
        return validate;
    }
    lseek(fd,seekQ*sizeof(validate), SEEK_SET);
    read(fd, &validate, sizeof(validate));
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return validate;
}

int getAccountID(int userid)
{
    User user = getUser(userid);
    return user.accid;
}


int dbDeposit(int deposit, int userid)
{
    int accid = getAccountID(userid);
    Account update;
    int seekQ = accid -1; 
    int fd = open("./data/Accounts.dat", O_RDWR);
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      return -1;
    }
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = seekQ*sizeof(update);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(update);
    lock.l_pid = getpid();

    int ret = fcntl(fd, F_SETLKW, &lock);
    if (ret == -1) 
    {
      printf("Attempt to set write lock failed\n");
      return -1;
    }
    
    lseek(fd, seekQ*sizeof(update), SEEK_SET);
    read(fd, &update, sizeof(update));
    printf("initial balace : %d\n", update.balance);

    update.balance += deposit;
    lseek(fd, seekQ*(sizeof(update)), SEEK_SET);
    write(fd, &update, sizeof(update));
    
    lseek(fd, seekQ*(sizeof(update)), SEEK_SET);
    read(fd, &update, sizeof(update));
    printf("final balance : %d\n",update.balance);
    
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW, &lock);
    close(fd);
    return 1;
}


int dbWithdraw(int withdraw, int userid)
{
    int accid = getAccountID(userid);
    Account update;
    int seekQ = accid -1; 
    int fd = open("./data/Accounts.dat", O_RDWR);
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      exit(1);
    }
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = seekQ*sizeof(update);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(update);
    lock.l_pid = getpid();

    int ret = fcntl(fd, F_SETLKW, &lock);
    if (ret == -1) 
    {
      printf("Attempt to set write lock failed\n");
      return -1;
    }
    
    lseek(fd, seekQ*sizeof(update), SEEK_SET);
    read(fd, &update, sizeof(update));
    printf("initial balace : %d\n", update.balance);
    printf("withdraw amount %d\n", withdraw);
    if (update.balance < withdraw)
    {
        printf("insuffient funds for withdrawal\n");
        return -1;
    }
    update.balance -= withdraw;
    lseek(fd, seekQ*(sizeof(update)), SEEK_SET);
    write(fd, &update, sizeof(update));
    //getchar(); 
    lseek(fd, seekQ*(sizeof(update)), SEEK_SET);
    read(fd, &update, sizeof(update));
    printf("final balance : %d\n",update.balance);
    
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW, &lock);
    close(fd);
    return 1;
}
int *dbaccount_details(int userid)
{
    static int account_details[4] = 
    {0,0,0,0}; 
    User user = getUser(userid);
    int account_balance = dbBalance(userid);
    printf("UserID: %d\n", userid);
    printf("AccountNo: %d\n", user.accid);
    printf("accountType: %d\n", user.acctype);
    printf("account balance: %d\n", account_balance);
    account_details[0] = userid;
    account_details[1] = user.accid;
    account_details[2] = user.acctype;
    account_details[3] = account_balance;
    return account_details;
}
int dbpassword_change(char *newpass, int userid)
{
    User validate; 
    int seekQ = userid - 1; 
    int fd = open("./data/Users.dat", O_RDWR);
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      return -1;
    }
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = seekQ*sizeof(validate);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(validate);
    lock.l_pid = getpid();

    int ret = fcntl(fd, F_SETLKW, &lock);
    if (ret == -1) 
    {
        printf("Attempt to set write lock failed\n");
        return -1;
    }
    lseek(fd, seekQ*sizeof(validate), SEEK_SET);
    read(fd, &validate, sizeof(validate));
    printf("old pass: %s\n", validate.password);
    
    strcpy(validate.password, newpass);
    lseek(fd, seekQ*sizeof(validate), SEEK_SET);
    write(fd, &validate, sizeof(validate));
    
    
    lseek(fd, seekQ*sizeof(validate), SEEK_SET);
    read(fd, &validate, sizeof(validate));
    printf("new pass: %s\n", validate.password);
    
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    return 1;
}

int dbBalance(int userid)
{
    int accid = getAccountID(userid);
    Account update;
    int seekQ = accid -1; 
    int fd = open("./data/Accounts.dat", O_RDWR);
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      return -1;
    }
    static struct flock lock;

    lock.l_type = F_RDLCK;
    lock.l_start = seekQ*sizeof(update);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(update);
    lock.l_pid = getpid();

    int ret = fcntl(fd, F_SETLKW, &lock);
    if (ret == -1) 
    {
      printf("Attempt to set write lock failed\n");
      return -1;
    }
    
    lseek(fd, seekQ*sizeof(update), SEEK_SET);
    read(fd, &update, sizeof(update));
    printf("balance : %d\n", update.balance);
    
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW, &lock);
    close(fd);
    return update.balance;
}
int ValidateLogin(Credentials *loginatt) 
{
    int userid = loginatt->id; 
    User validate = getUser(userid); 
    int result;
    if (validate.id == loginatt->id)
    {
        if (strcmp(validate.password, loginatt->password) == 0)
        {
            result = 1;
        }
        else
        {
            result = -1;
        }
    }
    else
    {
        result = -1;
    }
    return result;
} 


int ValidateAdminLogin(AdminCredentials *loginatt) 
{
    printf("inside the validateAdminLogin function\n"); 
    AdminCredentials validate; 
    int seekQ = 0; 
    int fd = open("./data/Admins.dat", O_RDONLY, 0744);
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      exit(1);
    }
    static struct flock lock;
    
    lock.l_type = F_RDLCK;
    lock.l_start = 0; 
    lock.l_whence = SEEK_SET;
    lock.l_len = 0; 
    lock.l_pid = getpid();
    

    int ret = fcntl(fd, F_SETLK, &lock);
    if (ret == -1) 
    {
        int err = errno;
        fprintf(stderr, "Attempt to set write lock failed %s\n",strerror(err));
        return -1;
    }
    int result = -1;
    lseek(fd,0,SEEK_SET); 
    printf("entered username : %s\n", loginatt->username);
    printf("entered password: %s\n", loginatt->password);
    while(read(fd, &validate, sizeof(validate)))
    {
        if (strcmp(validate.username, loginatt->username) == 0)
        {
            if (strcmp(validate.password, loginatt->password) == 0)
            {
                return 1;
            }
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return result;
} 

int db_delete_account(int account_no)
{
    Account account;
    Account delete = 
    {
        .id = -1,
        .balance = -1
    };
    int fd = open("./data/Accounts.dat", O_RDWR, 0744);
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      exit(1);
    }
    static struct flock lock;
    
    lock.l_type = F_WRLCK;
    lock.l_start = 0; 
    lock.l_whence = SEEK_SET;
    lock.l_len = 0; 
    lock.l_pid = getpid();
    

    int ret = fcntl(fd, F_SETLK, &lock);
    if (ret == -1) 
    {
        int err = errno;
        fprintf(stderr, "Attempt to set write lock failed %s\n", strerror(err));
        return -1;
    }
    int result = -1;
    lseek(fd, 0, SEEK_SET);
    while(read(fd, &account, sizeof(account)))
    {
        if (account.id == account_no)
        {
            lseek(fd, -sizeof(account), SEEK_CUR);
            write(fd, &delete, sizeof(delete));
            result = 0; 
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    
    User user;
    fd = open("./data/Users.dat", O_RDWR,0744);
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      exit(1);
    }
    static struct flock lock1;
    
    lock1.l_type = F_WRLCK;
    lock1.l_start = 0; 
    lock1.l_whence = SEEK_SET;
    lock1.l_len = 0; 
    lock1.l_pid = getpid();
    

    ret = fcntl(fd, F_SETLK, &lock1);
    if (ret == -1) 
    {
        int err = errno;
        fprintf(stderr, "Attempt to set write lock failed %s\n", strerror(err));
        return -1;
    }
    
    lseek(fd, 0, SEEK_SET);
    while(read(fd, &user, sizeof(user)))
    {
        if (user.accid == account_no)
        {
            user.accid = -1;
            lseek(fd, -sizeof(user), SEEK_CUR);
            write(fd, &user, sizeof(user));
            result = 1; 
        }
    }
    lock1.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock1);
    close(fd);
    return result;

}

int db_modify_account_type(int account_no, int acctype)
{
    User user;
    acctype = acctype -1;
    printf("in the db_modify_acctype function\n");
    int fd = open("./data/Users.dat", O_RDWR, 0744);
    int result;
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      exit(1);
    }
    static struct flock lock1;
    
    lock1.l_type = F_WRLCK;
    lock1.l_start = 0; 
    lock1.l_whence = SEEK_SET;
    lock1.l_len = 0; 
    lock1.l_pid = getpid();
    

    int ret = fcntl(fd, F_SETLK, &lock1);
    if (ret == -1) 
    {
        int err = errno;
        fprintf(stderr, "Attempt to set write lock failed %s\n", strerror(err));
        return -1;
    }
    
    lseek(fd, 0, SEEK_SET);
    while(read(fd, &user, sizeof(user)))
    {
        if (user.accid == account_no)
        {
            user.acctype = acctype;
            lseek(fd,-sizeof(user), SEEK_CUR);
            write(fd, &user, sizeof(user));
            result = 1; 
            break;
        }
    }
    if (result != 1)
    {
        result = -1;
    }
    lock1.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock1);
    close(fd);
    return result;
}

int db_modify_admin_password(char *newpass, char *username)
{
    AdminCredentials validate; 
    printf("inside db_modify_admin_password function\n");
    int fd = open("./data/Admins.dat", O_RDWR, 0744);
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      exit(1);
    }
    static struct flock lock;
    
    lock.l_type = F_WRLCK;
    lock.l_start = 0; 
    lock.l_whence = SEEK_SET;
    lock.l_len = 0; 
    lock.l_pid = getpid();
    

    int ret = fcntl(fd, F_SETLK, &lock);
    if (ret == -1) 
    {
        int err = errno;
        fprintf(stderr, "Attempt to set write lock failed %s\n", strerror(err));
        return -1;
    }
    lseek(fd, 0, SEEK_SET); 
    while(read(fd, &validate, sizeof(validate)))
    {
        if (strcmp(validate.username, username) == 0)
        {
            strcpy(validate.password, newpass);
            lseek(fd,-sizeof(validate), SEEK_CUR);
            write(fd, &validate, sizeof(validate));
            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);
            close(fd);
            return 1;
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return -1;

}

int db_add_account(int account_no)
{
    Account update = 
    {
        .id = account_no,
        .balance = 0,
    };
    int fd = open("./data/Accounts.dat", O_RDWR | O_APPEND);
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      return -1; 
    }
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    int ret = fcntl(fd, F_SETLKW, &lock);
    if (ret == -1) 
    {
      printf("Attempt to set write lock failed\n");
      return -1;
    }
    
    write(fd, &update, sizeof(update));
    
    
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW, &lock);
    close(fd);
    printf("account created\n");
     
    User newUser;
    fd = open("./data/Users.dat", O_RDWR, 0744);
    if(fd == -1) 
    {
      printf("Unable to open the file\n");
      return -1;
    }
    static struct flock lock1;

    lock1.l_type = F_WRLCK;
    lock1.l_start = 0;
    lock1.l_whence = SEEK_SET;
    lock1.l_len = 0; 
    lock1.l_pid = getpid();
    

    ret = fcntl(fd, F_SETLK, &lock1);
    if (ret == -1) 
    {
        int err = errno;
        fprintf(stderr, "Attempt to set write lock1 failed %s\n", strerror(err));
        return -1;
    }
    
    int max_userid;
    while(read(fd, &newUser, sizeof(newUser)))
    {
        if (newUser.id > max_userid)
            max_userid = newUser.id;
    }
    
    printf("max_userid: %d\n", max_userid); 
    newUser.id = max_userid+1;
    newUser.accid = account_no; 
    newUser.acctype = singleAccount;
    char pass[50] = "dummypassword123";
    strcpy(newUser.password, pass);
    
    ret = write(fd, &newUser, sizeof(newUser)); 
    if (ret < 0)
    {
        printf("unable to write new User\n");
        return -1;
    }
    printf("written user, but still in critical section\n"); 
    lock1.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock1);
    close(fd);
    printf("successfully created new user and assigned account\n");
    return 1;
}

