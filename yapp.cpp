// Yet Another Ping Program
// Madhav Mittal, CS19B029
// To compile, run $ g++ yapp.cpp -o yapp
// To ping an IP address x.x.x.x, run $ ./yapp x.x.x.x

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>

using namespace std;

#define PACKET_SIZE 64

double ya_ping(int socketfd, struct sockaddr_in dest_addr, char buf[]) {
    struct pollfd pfd;

    timeval start, finish; // timestamps to calculate rtt

    int status;
    int send_val;

    long int sec, usec;

    double msec;

    pfd.fd = socketfd;
    pfd.events = POLLIN;

    gettimeofday(&start, NULL); // start time

    // send packet to the destination
    send_val = sendto(socketfd, buf, PACKET_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    if (send_val == -1) {
        return -1;
    }

    // check if response received
    status = poll(&pfd, 1, 1000);

    gettimeofday(&finish, NULL); // end time

    if (status <= 0) { // timeout or error
        return -1;
    }

    sec = finish.tv_sec - start.tv_sec;
    usec = finish.tv_usec - start.tv_usec;

    msec = sec * 1000 + (double)usec / 1000;

    return msec;
}

int main(int argc, char *argv[]) {

    struct in_addr ip;
    struct sockaddr_in dest_addr;
    struct icmphdr *icmp_hdr;
    struct pollfd pfd;

    char *ip_str;
    char buf[PACKET_SIZE] = {0};

    int ip_valid;
    int socketfd;

    double rtt;

    ip_str = argv[1];                  // get ip address from command line arguments
    ip_valid = inet_aton(ip_str, &ip); // convert ip address from string to binary and store in ip

    if (ip_valid == 0) {
        printf("Bad hostname\n");
        return 0;
    }

    // create socket and store its file descriptor in socketfd
    socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);

    if (socketfd < 0) {
        printf("ERROR!\n");
        return 0;
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(0);
    dest_addr.sin_addr = ip;

    icmp_hdr = (struct icmphdr *)buf;

    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->un.echo.sequence = htons(1);
    icmp_hdr->un.echo.id = 1;

    rtt = ya_ping(socketfd, dest_addr, buf);

    if (rtt < 0) {
        printf("Request timed out or host unreacheable\n");
    } else {
        printf("Reply from %s. RTT = %f ms\n", inet_ntoa(ip), rtt);
    }

    return 0;
}