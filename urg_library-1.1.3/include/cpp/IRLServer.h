#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

#ifndef IRLSERVERH
#define IRLSERVERH

class IRLServer {
    std::string ip;
    int myPort;
    struct sockaddr_in servaddr,cliaddr;
    int sockfd;
  public:
    ~IRLServer();
    IRLServer(int);
    void setupServer(int);
    int send(const void*, int, int, const char*);
    int receive(unsigned char* data, int buffsize);
    int close_server();
};
#endif
