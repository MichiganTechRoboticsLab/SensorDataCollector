#include<iostream>
#include<string>

#include"IRLServer.h"

IRLServer::IRLServer(int serv_port) {

    myPort=serv_port;

    int recvlen;
    socklen_t len;
    char mesg[100];
    
    setupServer(myPort);

}

void IRLServer::setupServer(int serv_port) {

    myPort=serv_port;

    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(myPort);
    bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

}

int IRLServer::send(const void* data, int n, int CLI_PORT, const char* ip) {

    bzero(&cliaddr,sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    
    inet_pton(AF_INET, ip, &(cliaddr.sin_addr));
    cliaddr.sin_port = htons(CLI_PORT);

    return sendto(sockfd, data, n,0,(struct sockaddr *)&cliaddr,
        sizeof(struct sockaddr_in));
}

int IRLServer::receive(unsigned char* data, int buffsize) {

    return recvfrom(sockfd, data, buffsize,0,NULL,NULL);
}

int IRLServer::close_server() {
    std::cout<<"Closing Port "<<myPort<<std::endl;

    return close(sockfd);
}

IRLServer::~IRLServer() {
    close_server();
}
