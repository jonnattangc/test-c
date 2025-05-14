#include <stdio.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <ifaddrs.h> // Necesario para getifaddrs y freeifaddrs

/**
 * Prints the names of all network interfaces that are up and not loopback.
 *
 * This function retrieves a linked list of network interfaces using
 * getifaddrs, iterates through the list, and prints the name of each
 * interface that is up (IFF_UP) and not a loopback interface (not IFF_LOOPBACK).
 * The function uses perror to handle errors from getifaddrs and ensures
 * that resources are freed using freeifaddrs.
 */
void print_eth_names() {
    struct ifaddrs *ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr != NULL && (ifa->ifa_flags & IFF_UP) && !(ifa->ifa_flags & IFF_LOOPBACK)) {
            printf("Name: %s\n", ifa->ifa_name);
        }
    }
    freeifaddrs(ifaddr);
    return ;
}

int main() {
    print_eth_names();
    return 0;
}
