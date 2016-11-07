#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <time.h>
#include <string.h>
#include "ntp_helper.h"
#include "client_helper.h"

#define PORT 1234
#define SLEEP_TIME 5

int main (int argc, char * argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;

    /* open socket */
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(1);
    }

    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(PORT);
    
    if (inet_pton(PF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("pton error");
        exit(1);
    }

    while (1) {
        requestMessage(sockfd, server_addr);
        sleep(SLEEP_TIME);
    }
    close(sockfd);

    return 0;
}
