#include "ntp_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/select.h>

#define POLL 3

struct timeval mTime;

void setMessage(struct sntpMessageFormat *msg) {
    struct timeval myTime;
    gettimeofday(&myTime, NULL);

    // LI
    msg->li_vn_mode = 0;

    // VN
    msg->li_vn_mode <<= 2; // shift left 2 bits
    msg->li_vn_mode |= 4;

    // MODE
    msg->li_vn_mode <<= 3;
    // I am a client
    msg->li_vn_mode |= 3; 

    msg->originateTimestamp = time2ntp(myTime);
    msg->transmitTimestamp = time2ntp(myTime);
}

void requestMessage(int sockfd, struct sockaddr_in server_addr) {
    int numbytes;
    int addr_len = sizeof(server_addr);

    struct sntpMessageFormat msg;
    struct sntpMessageFormat recvMsg;

    /* get current time */
    gettimeofday(&mTime, NULL);

    /* set all fields to be 0 */
    initializeMessageFormat(&msg);
    /* set time message departed for server */
    setMessage(&msg);

    printf("\nMessage request: \n");
    printTime(mTime);
    printf("\n");
    printMessageDetails(msg);
    printf("\n");

    /* msg is in ntp format */
    convertMessageFormat(&msg);

    if ((numbytes = sendto(sockfd, &msg, sizeof(msg), 0, 
        (struct sockaddr *) &server_addr, addr_len)) == -1) {
        perror("sendto error");
        exit(1);
    }
    

    if ((numbytes = recvfrom(sockfd, (struct sntpMessageFormat *) &recvMsg,
            sizeof(struct sntpMessageFormat), 0, (struct sockaddr *) &server_addr,
            &addr_len)) == -1) {
        perror("recvfrom error");
        exit(1);
    }

    gettimeofday(&mTime, NULL);
    if (numbytes > 0) {
        //convertMessageFormat(&recvMsg);

        msg.refTimestamp = recvMsg.transmitTimestamp;
        //msg is in host format
        convertMessageFormat(&msg);
        msg.recvTimestamp = time2ntp(mTime);

        printf("\nMessage received: \n");
        printTime(mTime);
        printf("\n");
        printMessageDetails(msg);
    } else {
        printf("No replies from server. \n");
    }
}
