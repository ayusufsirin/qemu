// Server.cpp
#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

VerilatedModuleServer::VerilatedModuleServer(int port) : serverPort(port) {
    verilatedModule = new Vsimple_module(); // Instantiate the Verilated module
}

void VerilatedModuleServer::startServer() {
    // Initialize Verilated module if needed
    verilatedModule->reset = 1; // Example of resetting the module
    verilatedModule->clk = 0; // Initialize clock

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(serverPort);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on port " << serverPort << std::endl;

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    handleClient(new_socket);

    close(new_socket);
    close(server_fd);
}

void VerilatedModuleServer::registerFunction(const std::string& command, std::function<void(int)> func) {
    commandMap[command] = func;
}

void VerilatedModuleServer::handleClient(int clientSocket) {
    char buffer[1024] = {0};
    while (true) {
        verilatedModule->clk = !verilatedModule->clk;
        verilatedModule->eval();

        memset(buffer, 0, 1024);
        int valread = read(clientSocket, buffer, 1024);
        if (valread <= 0) {
            break;
        }

        std::string command(buffer);
        auto it = commandMap.find(command);
        if (it != commandMap.end()) {
            it->second(clientSocket);
        } else {
            std::cout << "Unknown command received: " << command << std::endl;
        }
    }
}

// Example Main Function
int main() {
    VerilatedModuleServer server(8080);
    server.registerFunction("read_reg", [](int clientSocket) {
        // Implement functionality to interact with the Verilated model
        int value = server.verilatedModule->some_output;;
        send(clientSocket, &value, sizeof(value), 0);
    });
    server.startServer();
    return 0;
}
