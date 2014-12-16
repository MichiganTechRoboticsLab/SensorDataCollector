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

#define MYPORT "8080"    // the port users will be connecting to

#define MAXBUFLEN 100
#define BREAK_SIZE 4094

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

typedef enum
{
    WAIT,
    SEND
} STATE;

int main(int argc, char **argv) {
    // File Stuff
    FILE *input;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    if (argc > 1) {
        if(!strcmp(argv[1],"-")) {
            input = stdin;
        } else {
            input = fopen(argv[1],"r");
            if (NULL == input) {
                fprintf(stderr, "Unable to open '%s': %s\n",
                        argv[1], strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    } else {
        input = stdin;
    }
    
    // Server Stuff
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];
    
    // Set up sockets and fun things
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
            perror("listener: socket");
        continue;
            }
            
            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                perror("listener: bind");
                continue;
            }
            
            break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }
    
    freeaddrinfo(servinfo);
    
    printf("Waiting for client to connect...\n");
    
    int loop = 1;
    STATE state = WAIT;
    
    while( loop )
    {
        switch( state )
        {
            
            case WAIT:
            {
                addr_len = sizeof their_addr;
                if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
                    (struct sockaddr *)&their_addr, &addr_len)) == -1) 
                {
                    perror("recvfrom");
                    exit(1);
                }
                state = SEND;
                break;
            }
            case SEND:
            {
                if ((read = getline(&line, &len, input)) != -1)
                {
                    printf("Retrieved line from input of length %zu: %s\n", read, line);
                    //Break Data into Packets
                    char* start = line;
                    sendto(sockfd, "BEGIN", 5, 0, (struct sockaddr *)&their_addr, addr_len);
                    while(read > BREAK_SIZE)
                    {
                        sendto(sockfd, start, BREAK_SIZE ,0,(struct sockaddr *)&their_addr, addr_len);
                        start+=BREAK_SIZE;
                        read-=BREAK_SIZE;
                    }
                    sendto(sockfd, start, read ,0,(struct sockaddr *)&their_addr, addr_len);
                    sendto(sockfd, "END", 3, 0, (struct sockaddr *)&their_addr, addr_len);
                }
                else
                {
                    loop = 0;
                }
                state = WAIT;
                break;
            }
        }
            
    }
    sendto(sockfd, "DONE", 4, 0, (struct sockaddr *)&their_addr, addr_len);
    if(input != stdin)
        fclose(input);
    free(line);
    return EXIT_SUCCESS;
}