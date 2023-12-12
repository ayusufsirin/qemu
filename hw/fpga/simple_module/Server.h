// Server.h
#include <map>
#include <string>
#include <functional>
#include "Vsimple_module.h" // Include Verilated model

class VerilatedModuleServer {
public:
    VerilatedModuleServer(int port);
    void startServer();
    void registerFunction(const std::string& command, std::function<void(int)> func);

private:
    Vsimple_module* verilatedModule; // Verilated module instance
    int serverPort;
    void handleClient(int clientSocket);
    std::map<std::string, std::function<void(int)>> commandMap;
    // Add other necessary members and methods
};
