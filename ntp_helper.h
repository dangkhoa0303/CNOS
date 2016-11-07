#include <netdb.h>

#ifndef NTP_HELPER_H
#define NTP_HELPER_H

/* message format for sntp */
struct sntpMessageFormat {
  u_int8_t li_vn_mode;
  u_int8_t stratum;
  u_int8_t poll;

  int8_t precision;
  int32_t rootDelay;

  uint32_t rootDispersion;
  uint32_t refIdentifier;
  uint64_t refTimestamp;
  uint64_t originateTimestamp;
  uint64_t recvTimestamp;
  uint64_t transmitTimestamp;
};

uint64_t time2ntp (struct timeval);
struct timeval ntp2time (unsigned long long);

void printTime(struct timeval);
void initializeMessageFormat(struct sntpMessageFormat*);
void convertMessageFormat(struct sntpMessageFormat*);
void printMessageDetails(struct sntpMessageFormat);

#endif

  
