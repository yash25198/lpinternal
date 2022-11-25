#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#define IP_PROTOCOL 0
#define PORT_NO 15050
#define NET_BUF_SIZE 32
#define sendrecvflag 0
#define nofile "File Not Found!"
// function to clear buffer
void _ls(const char *dir, int op_a, int op_l)
{
        // Here we will list the directory
        struct dirent *d;
        FILE *fp;
        fp = fopen("ls.txt", "w");
        DIR *dh = opendir(dir);
        if (!dh)
        {
                if (errno = ENOENT)
                {
                        // If the directory is not found
                        perror("Directory doesn't exist");
                }
                else
                {
                        // If the directory is not readable then throw error and exit
                        perror("Unable to read directory");
                }
                exit(EXIT_FAILURE);
        }
        // While the next entry is not readable we will print directory files
        while ((d = readdir(dh)) != NULL)
        {
                // If hidden files are found we continue
                if (!op_a && d->d_name[0] == '.')
                        continue;
                fprintf(fp, "%s  ", d->d_name);
                if (op_l)
                        fprintf(fp, "\n");
        }
        if (!op_l)
                fprintf(fp, "\n");
        fclose(fp);
}
void clearBuf(char *b)
{
        int i;
        for (i = 0; i < NET_BUF_SIZE; i++)
                b[i] = '\0';
}
// char Cipher(char ch)
// {
//         return ch ^ cipherKey;
// }
// function sending file
int sendFile(FILE *fp, char *buf, int s)
{
        int i, len;
        if (fp == NULL)
        {
                strcpy(buf, nofile);
                len = strlen(nofile);
                buf[len] = EOF;
                // for (i = 0; i <= len; i++)
                //         buf[i] = Cipher(buf[i]);
                return 1;
        }
        char ch, ch2;
        for (i = 0; i < s; i++)
        {
                ch = fgetc(fp);
                // ch2 = Cipher(ch);
                buf[i] = ch;
                if (ch == EOF)
                        return 1;
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
        addr_con.sin_addr.s_addr = INADDR_ANY;
        char net_buf[NET_BUF_SIZE];
        char command[NET_BUF_SIZE];
        char buf[NET_BUF_SIZE];
        FILE *fp;
        // socket()
        sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
        if (sockfd < 0)
                printf("\nfile descriptor not received!!\n");
        else
                printf("\nfile descriptor %d received\n", sockfd);
        // bind()
        if (bind(sockfd, (struct sockaddr *)&addr_con, sizeof(addr_con)) == 0)
                printf("\nSuccessfully binded!\n");
        else
                printf("\nBinding Failed!\n");
        while (1)
        {
                printf("\nWaiting for request...\n");
                // receive file name
                clearBuf(net_buf);
                clearBuf(command);
                clearBuf(buf);
                nBytes = recvfrom(sockfd, buf,
                                  NET_BUF_SIZE, sendrecvflag,
                                  (struct sockaddr *)&addr_con, &addrlen);
                sscanf(buf, "%s", command);
                if (!strcmp(command, "get"))
                {
                        sscanf(buf, "%s%s", command, net_buf);
                        fp = fopen(net_buf, "r");
                        printf("\nCommand Received: %s\n", command);
                        printf("\nFile Name Received: %s\n", net_buf);
                        if (fp == NULL)
                                printf("\nFile open failed!\n");
                        else
                                printf("\nFile Successfully opened!\n");
                        while (1)
                        {
                                // process
                                if (sendFile(fp, net_buf, NET_BUF_SIZE))
                                {
                                        sendto(sockfd, net_buf, NET_BUF_SIZE,
                                               sendrecvflag,
                                               (struct sockaddr *)&addr_con, addrlen);
                                        break;
                                }
                                // send
                                sendto(sockfd, net_buf, NET_BUF_SIZE,
                                       sendrecvflag,
                                       (struct sockaddr *)&addr_con, addrlen);
                                clearBuf(net_buf);
                        }
                        if (fp != NULL)
                                fclose(fp);
                }
                else if (!strcmp(command, "ls"))
                {
                        if (fork() == 0)
                        {
                                _ls(".", 0, 1);
                                fp = fopen("ls.txt", "r");
                                printf("\nCommand Received: %s\n", command);
                                while (1)
                                {
                                        // process
                                        if (sendFile(fp, net_buf, NET_BUF_SIZE))
                                        {
                                                sendto(sockfd, net_buf, NET_BUF_SIZE,
                                                       sendrecvflag,
                                                       (struct sockaddr *)&addr_con, addrlen);
                                                break;
                                        }
                                        // send
                                        sendto(sockfd, net_buf, NET_BUF_SIZE,
                                               sendrecvflag,
                                               (struct sockaddr *)&addr_con, addrlen);
                                        clearBuf(net_buf);
                                }
                                _exit(0); // exit child process
                        }
                        continue;
                }
                else if (!strcmp(command, "pwd"))
                {
                        system("pwd > pwd.txt");
                        fp = fopen("pwd.txt", "r");
                        printf("\nCommand Received: %s\n", command);
                        while (1)
                        {
                                // process
                                if (sendFile(fp, net_buf, NET_BUF_SIZE))
                                {
                                        sendto(sockfd, net_buf, NET_BUF_SIZE,
                                               sendrecvflag,
                                               (struct sockaddr *)&addr_con, addrlen);
                                        break;
                                }
                                // send
                                sendto(sockfd, net_buf, NET_BUF_SIZE,
                                       sendrecvflag,
                                       (struct sockaddr *)&addr_con, addrlen);
                                clearBuf(net_buf);
                        }
                }
                else if(!strcmp(command,"count"))
                {
                        char token[32];
                        sscanf(buf, "%s%s%s", command, net_buf,token);
                        printf("\nCommand Received: %s\n", command);
                        printf("\nFile Name Received: %s\n", net_buf);
                        char line[100];
                        strcpy(line,"grep -c ");
                        strcat(line,token);
                        strcat(line," ");
                        strcat(line,net_buf);
                        strcat(line," > count.txt");
                        system(line);
                        fp = fopen("count.txt", "r");
                        while (1)
                        {
                                // process
                                if (sendFile(fp, net_buf, NET_BUF_SIZE))
                                {
                                        sendto(sockfd, net_buf, NET_BUF_SIZE,
                                               sendrecvflag,
                                               (struct sockaddr *)&addr_con, addrlen);
                                        break;
                                }
                                // send
                                sendto(sockfd, net_buf, NET_BUF_SIZE,
                                       sendrecvflag,
                                       (struct sockaddr *)&addr_con, addrlen);
                                clearBuf(net_buf);
                        }
                        continue;

                }
                else
                {
                        printf("\nCommand Received: %s\n", command);
                        printf("\nInvalid Command!\n");
                }
        }
        return 0;
}
