#include "ntp_helper.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

const unsigned long long OFFSET = 2208988800ULL;
const unsigned long long NTP_SCALE = 4294967295ULL;

/* initialize all fields to be 0 */
void initializeMessageFormat(struct sntpMessageFormat *format) {
  format->li_vn_mode = 0;
  format->stratum = 0;
  format->poll = 0;
  format->precision = 0;
  format->rootDelay = 0;
  format->rootDispersion = 0;
  format->refIdentifier = 0;
  format->refTimestamp = 0;
  format->originateTimestamp = 0;
  format->recvTimestamp = 0;
  format->transmitTimestamp = 0;
}

/* convert message format from hots byte order to network byte order and vice versa */
void convertMessageFormat(struct sntpMessageFormat *msg) {
  msg->rootDelay = htobe32(msg->rootDelay);
  msg->rootDispersion = htobe32(msg->rootDispersion);
  msg->refIdentifier = htobe32(msg->refIdentifier);
  msg->refTimestamp = htobe64(msg->refTimestamp);
  msg->originateTimestamp = htobe64(msg->originateTimestamp);
  msg->recvTimestamp = htobe64(msg->recvTimestamp);
  msg->transmitTimestamp = htobe64(msg->transmitTimestamp);
}

/* convert from struct timeval to ntp time */ 
uint64_t time2ntp(struct timeval time) {
  unsigned long long time_ntp, time_usecs;

  time_ntp = time.tv_sec + OFFSET;
  time_usecs = (NTP_SCALE * time.tv_usec) / 1000000UL;

  return (time_ntp << 32) | time_usecs;
}

/* convert from ntp time to timeval */
struct timeval ntp2time(unsigned long long ntp) {
  unsigned long long time_secs, time_usecs;

  time_usecs = ntp & 0xFFFFFFFF;
  time_secs = (ntp >> 32) & 0xFFFFFFFF;
			
  struct timeval t;

  time_secs = time_secs - OFFSET;

  time_usecs = (time_usecs + 1) * 1000000UL / NTP_SCALE;

  t.tv_sec = (time_t) time_secs;
  t.tv_usec = (suseconds_t) time_usecs;

  return t;
}

/* print out message  details */
void printMessageDetails(struct sntpMessageFormat msg) {
  printf("LI: %d\n", msg.li_vn_mode >> 6);
  printf("VN: %d\n", (msg.li_vn_mode >> 3) & 0x07);
  printf("MODE: %d\n", msg.li_vn_mode & 0x07);

  printf("STRATUM: %d\n", msg.stratum);
  printf("POLL: %d\n", pow(2, msg.stratum));
  printf("PRECISION: %d\n", pow(2, msg.precision));

  printf("ROOT DELAY: %d\n", msg.rootDelay);
  printf("ROOT DISPERSION: %d\n", msg.rootDispersion);

  // get reference identifier char into a char array
  char refIdentifierChar[5];
  int i;
  int32_t temp = msg.refIdentifier;

  for (i=3; i>=0; i--) {
    refIdentifierChar[i] = temp & 0xFF;
    temp = temp >> 8;
  }

  refIdentifierChar[4] = '\0';
  printf("Reference Identifier: %s\n", refIdentifierChar);

  printf("Reference Timestamp: ");
  printTime(ntp2time(msg.refTimestamp));

  printf("Originate Timestamp: ");
  printTime(ntp2time(msg.originateTimestamp));

  printf("Receive Timestamp: ");
  printTime(ntp2time(msg.recvTimestamp));

  printf("Transmit Timestamp: ");
  printTime(ntp2time(msg.transmitTimestamp));
}

/* print time from timeval struct */
void printTime(struct timeval tv) {
  time_t nowtime;
  struct tm *nowtm;
  char tmbuf[64], buf[64];

  nowtime = tv.tv_sec;
  nowtm = localtime(&nowtime);

  strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", nowtm);
  snprintf(buf, sizeof(buf), "%s.%06d", tmbuf, tv.tv_usec);

  printf("%s\n", buf);
}
