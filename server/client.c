// CS4461 - PUT STUDENT NAME HERE

/* Template code provided by Scott Kuhl and based on:
 *   http://www.beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define TEST 0

typedef enum
{
    START,
    ACK,
    WAIT
} STATE;

#define DATA_BREAK 4094

int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char buffer[DATA_BREAK + 1];
    char* data = NULL;
    
    
    if (argc != 3) {
        fprintf(stderr,"usage: %s hostname port\n", argv[0]);
        exit(1);
    }
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "client: failed to bind socket\n");
        return 2;
    }
    
    
    STATE state = START;
    int loop = 1;
    int buffLen = 0;
    
    while(loop)
    {
        switch(state)
        {
            case START:
            {
                printf("Sending Start\n");
                sendto(sockfd, "START", 5,0, p->ai_addr, p->ai_addrlen);
                state = WAIT;
                break;
            }
            case ACK:
            {
                
                sendto(sockfd, "ACK", 3,0, p->ai_addr, p->ai_addrlen);
                state = WAIT;
                break;
            }
            case WAIT:
            {
                int finished = 0;
                int totalSize = 0;
                while(!finished)
                {
                    if((buffLen = recvfrom(sockfd, buffer, DATA_BREAK, 0, p->ai_addr, &(p->ai_addrlen))) == -1)
                    {
                        perror("recvfrom");
                        exit(1);
                    }
                    buffer[buffLen] = '\0';
                    
                    
                    if(strcmp(buffer, "BEGIN") == 0)
                    {
                        printf("RECV: %s\n", buffer);
                        //SETUP
                        printf("Starting New Packet\n");
                        totalSize = DATA_BREAK;
                        data = realloc(data, totalSize);
                    }
                    else if(strcmp(buffer, "DONE") == 0)
                    {
                        printf("RECV: %s\n", buffer);
                        //DONE
                        finished = 1;
                        loop = 0;
                    }
                    else if(strcmp(buffer, "END") == 0)
                    {
                        printf("RECV: %s\n", buffer);
                        //DONE
                        finished = 1;
                    }
                    else
                    {
                        //COLLECT DATA
                        printf("RECV: data\n");
                        totalSize += DATA_BREAK;
                        char* newLoc = realloc(data, totalSize);
                        if(newLoc != NULL)
                        {
                            data = newLoc;
                        }
                        else
                        {
                            printf("\nExiting!!");
                            free(data);
                            exit(0);
                        }
                        printf("BEFORE: %s\n---------------------------\n", data);
                        strcat(data, buffer);
                        printf("AFTER: %s\n\n", data);
                    }
                }
                if(data) printf("%s\n",data);
                sleep(1);
                free(data);
                data = NULL;
                state = ACK;
                break;
            }
        }
    }
    
    freeaddrinfo(servinfo);
    close(sockfd);
    return 0;
}
