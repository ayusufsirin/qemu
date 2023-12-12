#include "obj_dir/Vsimple_module.h"
#include "verilated.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#define PORT 8080

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vsimple_module* top = new Vsimple_module;

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started, waiting for client connection..." << std::endl;

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    std::cout << "Client connected, starting simulation..." << std::endl;

    char buffer[1024] = {0};
    while (!Verilated::gotFinish()) {
        top->enable = 1;

        for (int i = 0; i < 10; ++i) {
            top->clk = !top->clk;
            top->eval();
        }

        // Send the counter value
        send(new_socket, &top->counter, sizeof(top->counter), 0);
        std::cout << "Counter value sent: " << (int)top->counter << std::endl;
    }

    close(new_socket);
    close(server_fd);
    delete top;
    return 0;
}
