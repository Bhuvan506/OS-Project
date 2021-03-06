#ifndef CLIENT_H
#define CLIENT_H
#include "datastructures.h"

int normallogin(int client);
int adminlogin(int client); 
void DeleteAccountHandler(long cookie, int socket_d);
void AddAccountHandler(long cookie, int socket_d);
void ModifyAdminPasswordHandler(long cookie, int socket_d, char *username);
void ModifyAccountTypeHandler(long cookie, int socket_d);
void viewAccountDetailsHandler(long cookie, int socket_d);
#endif 