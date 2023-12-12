#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    struct sockaddr_in serv_addr;
    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    char buffer[1024] = {0};
    // uint8_t counterValue;

    // // Send data to server
    // send(sock, "Hello from client", strlen("Hello from client"), 0);

    // // Read data sent by server
    // read(sock, &counterValue, sizeof(counterValue));
    // printf("Counter value: %d\n", counterValue);

    // Example: send '1' to enable the counter
    send(sock, "1", 1, 0);
    sleep(1); // Wait for a while to allow counter to increment

    // Read counter value
    uint8_t counterValue;
    read(sock, &counterValue, sizeof(counterValue));
    printf("Counter value: %d\n", counterValue);

    close(sock);
    return 0;
}
