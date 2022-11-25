#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define IP_PROTOCOL 0
#define IP_ADDRESS "127.0.0.1" // localhost
#define PORT_NO 15050
#define NET_BUF_SIZE 32
#define sendrecvflag 0
// function to clear buffer
void clearBuf(char *b)
{
        int i;
        for (i = 0; i < NET_BUF_SIZE; i++)
                b[i] = '\0';
}
// function for decryption
// char Cipher(char ch)
// {
//         return ch ^ cipherKey;
// }
// function to receive file
int recvFile(char *buf, int s)
{
        int i;
        char ch;
        for (i = 0; i < s; i++)
        {
                ch = buf[i];
                // ch = Cipher(ch);
                if (ch == EOF)
                        return 1;
                else
                        printf("%c", ch);
        }
        return 0;
}
// driver code
int main()
{
        int sockfd, nBytes;
        struct sockaddr_in addr_con;
        int addrlen = sizeof(addr_con);
        addr_con.sin_family = AF_INET;
        addr_con.sin_port = htons(PORT_NO);
        addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);
        char net_buf[NET_BUF_SIZE];
        char command[NET_BUF_SIZE];
        char file_name[20];
        int service;
        char buff[100];
        FILE *fp;
        FILE *fp2;
        // socket()
        sockfd = socket(AF_INET, SOCK_DGRAM,
                        IP_PROTOCOL);
        if (sockfd < 0)
                printf("\nfile descriptor not received!!\n");
        else
                printf("\nfile descriptor %d received\n", sockfd);
        while (1)
        {
                printf("\nEnter serive: \n1.get file\n2.ls\n3.pwd\n");
                scanf("%d", &service);
                clearBuf(net_buf);
                clearBuf(command);
                clearBuf(file_name);
                switch (service)
                {
                case 1:
                        printf("\nPlease enter file name to download:\n");
                        scanf("%s", file_name);
                        strcpy(command, "get ");
                        strcat(command, file_name);
                        sendto(sockfd, command, NET_BUF_SIZE,
                               sendrecvflag, (struct sockaddr *)&addr_con,
                               addrlen);
                        printf("\n---------Data Received---------\n");
                        while (1)
                        {
                                // receive
                                clearBuf(net_buf);
                                nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE,
                                                  sendrecvflag, (struct sockaddr *)&addr_con,
                                                  &addrlen);
                                // process
                                fp2 = fopen("damn1.txt", "w");
                                strcpy(buff, net_buf);
                                fprintf(fp2, "%s", buff);
                                fclose(fp2);
                                break;
                        }
                        printf("\n--------File Downloaded--------\n");

                        break;
                case 2:
                        strcpy(command, "ls ");
                        sendto(sockfd, command, NET_BUF_SIZE,
                               sendrecvflag, (struct sockaddr *)&addr_con,
                               addrlen);
                        printf("\n---------Data Received---------\n");
                        while (1)
                        {
                                // receive
                                clearBuf(net_buf);
                                nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE,
                                                  sendrecvflag, (struct sockaddr *)&addr_con,
                                                  &addrlen);
                                // process
                                if (recvFile(net_buf, NET_BUF_SIZE))
                                {
                                        break;
                                }
                        }
                        printf("\n-------------------------------\n");

                        break;
                case 3:
                        strcpy(command, "pwd ");
                        sendto(sockfd, command, NET_BUF_SIZE,
                               sendrecvflag, (struct sockaddr *)&addr_con,
                               addrlen);
                        printf("\n---------Data Received---------\n");
                        while (1)
                        {
                                // receive
                                clearBuf(net_buf);
                                nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE,
                                                  sendrecvflag, (struct sockaddr *)&addr_con,
                                                  &addrlen);
                                // process
                                if (recvFile(net_buf, NET_BUF_SIZE))
                                {
                                        break;
                                }
                        }
                        printf("\n-------------------------------\n");


                        break;
                case 4:
                        strcpy(command, "count ");
                        printf("\nPlease enter file name to count:\n");
                        scanf("%s", file_name);
                        strcat(command, file_name);
                        printf("\nPlease enter word to count:\n");
                        scanf("%s", file_name);
                        strcat(command, " ");
                        strcat(command, file_name);

                        sendto(sockfd, command, NET_BUF_SIZE,
                               sendrecvflag, (struct sockaddr *)&addr_con,
                               addrlen);
                        printf("\n---------Data Received---------\n");
                        while (1)
                        {
                                // receive
                                clearBuf(net_buf);
                                nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE,
                                                  sendrecvflag, (struct sockaddr *)&addr_con,
                                                  &addrlen);
                                // process
                                if (recvFile(net_buf, NET_BUF_SIZE))
                                {
                                        break;
                                }
                        }
                        printf("\n-------------------------------\n");
                default:
                        break;
                }
        }
}