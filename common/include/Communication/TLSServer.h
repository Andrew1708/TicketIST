#ifndef TLSSERVER_H
#define TLSSERVER_H

#include <string>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include "Message.h"
#include "UserService.h"
#include "TicketService.h"
#include "EventService.h"
#include "GetAvailableTicketsMessage.h"
#include "Controller.h"
#include "Firewall.h" // Include the Firewall class

class TLSServer {
public:
    // Constructor: Initializes the server with certificate, key, IP, and port
    TLSServer(const std::string& cert_file, const std::string& key_file, int port, const std::string& ip);

    // Destructor: Cleans up resources
    ~TLSServer();

    // Starts the server (binds socket, listens for connections)
    bool start();

    // Runs the server, accepts and handles client connections
    void run();
    

private:
    // Initializes OpenSSL library
    bool init_openssl();

    // Cleans up OpenSSL library
    void cleanup_openssl();

    // Creates an SSL context
    bool create_context();

    // Configures the SSL context with the given certificate and key
    bool configure_context(const std::string& cert_file, const std::string& key_file);

    void handleClient(SSL* ssl, int client_socket);
    
    bool isAuthenticated(SSL* ssl);

    void sendData(SSL* ssl, const std::string& data);

    void sendAccept(SSL* ssl, const std::string& message);

    void sendReject(SSL* ssl, const std::string& message);




private:
    int port_;                  // Port the server listens on
    const std::string ip_;      // IP address the server binds to
    int server_fd_;             // File descriptor for the server socket
    SSL_CTX* ctx_;              // SSL context
    struct sockaddr_in address_;// Server socket address structure
    Controller controller;      // Controller object to handle client requests
    Firewall firewall;          // Firewall object for security checks
};

#endif // TLSSERVER_H
