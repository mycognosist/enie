#define _DEFAULT_SOURCE
#include <errno.h>
#include <stdio.h>
#include <memory.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>

char* iface;

// helper for parsing messages using netlink macros
void parseRtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len)
{
    memset(tb, 0, sizeof(struct rtattr *) * (max + 1));

    // while not end of the message
    while (RTA_OK(rta, len)) {
        if (rta->rta_type <= max) {
            // read attr
            tb[rta->rta_type] = rta;
        }
        // get next attr
        rta = RTA_NEXT(rta,len);
    }
}

int main(int argc, char *argv[])
{
    // parse command line arguments
    if( argc == 2  ) {
        // assign interface-of-interest based on user input
        iface = (char*)argv[1];
    }
    else if( argc > 2  ) {
        printf("Too many arguments supplied. Expected a single argument representing the interface identifier, e.g. wlan0\n");
        return 1;
    }
    else {
        printf("Please provide an interface identifier for enie, e.g. enie wlan0\n");
        return 1;
    }
    
    // create netlink socket
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

    if (fd < 0) {
        printf("Failed to create netlink socket: %s\n", (char*)strerror(errno));
        return 1;
    }

    struct sockaddr_nl  local;  // local addr struct
    char buf[8192];             // message buffer
    struct iovec iov;           // message structure
    iov.iov_base = buf;         // set message buffer as io
    iov.iov_len = sizeof(buf);  // set size

    memset(&local, 0, sizeof(local));

    // set protocol family
    local.nl_family = AF_NETLINK;
    // set groups we are interested i
    local.nl_groups =   RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE;
    // set out id using current process id
    local.nl_pid = getpid();

    // initialize protocol message header
    struct msghdr msg;  
    {
        msg.msg_name = &local;                  // local address
        msg.msg_namelen = sizeof(local);        // address size
        msg.msg_iov = &iov;                     // io vector
        msg.msg_iovlen = 1;                     // io size
    }   

    // bind the socket
    // if that fails, print an error message & close the file descriptor
    if (bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0) {
        printf("Failed to bind netlink socket: %s\n", (char*)strerror(errno));
        close(fd);
        return 1;
    }   

    // read and parse all messages
    while (1) {
        ssize_t status = recvmsg(fd, &msg, MSG_DONTWAIT);
        //  check status
        if (status < 0) {
            if (errno == EINTR || errno == EAGAIN)
            {
                usleep(250000);
                continue;
            }

            printf("Failed to read netlink: %s", (char*)strerror(errno));
            continue;
        }

        // check message length
        if (msg.msg_namelen != sizeof(local)) {
            printf("Invalid length of the sender address struct\n");
            continue;
        }

        // message parser
        struct nlmsghdr *h;

        // read all message headers
        for (h = (struct nlmsghdr*)buf; status >= (ssize_t)sizeof(*h); ) {
            int len = h->nlmsg_len;
            int l = len - sizeof(*h);
            char *ifName;

            if ((l < 0) || (len > status)) {
                printf("Invalid message length: %i\n", len);
                continue;
            }

            // now we can check message type
            char *ifUpp;
            char *ifRunn;
            struct ifinfomsg *ifi;  // structure for network interface info
            struct rtattr *tb[IFLA_MAX + 1];
            
            // get information about changed network interface
            ifi = (struct ifinfomsg*) NLMSG_DATA(h);

            // get attributes
            parseRtattr(tb, IFLA_MAX, IFLA_RTA(ifi), h->nlmsg_len);
            
            // validation
            if (tb[IFLA_IFNAME]) {
                // get network interface name
                ifName = (char*)RTA_DATA(tb[IFLA_IFNAME]);
            }
            
            // check ifName against user-supplied iface
            // only take action if the message is relevant to the user
            // ie. if iface == ifName
            if (strcmp(ifName, iface) == 0) {
                // get UP flag of the network interface
                if (ifi->ifi_flags & IFF_UP) {
                    ifUpp = (char*)"UP";
                } else {
                    ifUpp = (char*)"DOWN";
                }

                // get RUNNING flag of the network interface
                if (ifi->ifi_flags & IFF_RUNNING) {
                    ifRunn = (char*)"RUNNING";
                } else {
                    ifRunn = (char*)"NOT RUNNING";
                }

                char ifAddress[256];    // network addr
                struct ifaddrmsg *ifa;  // structure for network interface data
                struct rtattr *tba[IFA_MAX+1];

                // get data from the network interface
                ifa = (struct ifaddrmsg*)NLMSG_DATA(h);

                parseRtattr(tba, IFA_MAX, IFA_RTA(ifa), h->nlmsg_len);

                if (tba[IFA_LOCAL]) {
                    // get IP addr
                    inet_ntop(AF_INET, RTA_DATA(tba[IFA_LOCAL]), ifAddress, sizeof(ifAddress));
                }

                // match on the receive message type & take action
                switch (h->nlmsg_type) {
                    case RTM_DELADDR:
                        fprintf(stdout, "3_%s\n", ifName);
                        fflush(stdout);
                        break;

                    case RTM_DELLINK:
                        fprintf(stdout, "1_%s\n", ifName);
                        fflush(stdout);
                        break;

                    case RTM_NEWLINK:
                        fprintf(stdout, "0_%s_%s_%s\n", ifName, ifUpp, ifRunn);
                        fflush(stdout);
                        break;

                    case RTM_NEWADDR:
                        fprintf(stdout, "2_%s_%s\n", ifName, ifAddress);
                        fflush(stdout);
                        break;
                }

                // align offsets by the message length (important!)
                status -= NLMSG_ALIGN(len);

                // get the next message
                h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
            }
        }

        // sleep for a while
        usleep(250000);
    }

    // close the socket
    close(fd);

    return 0;
}
