#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <set>

// Include OpenSSL headers
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/opensslv.h>
#include "TLSServer.h"
#include "SignUpMessage.h"
#include "LogInMessage.h"
#include "GetAvailableTicketsMessage.h"
#include "AcceptMessage.h"
#include "RejectMessage.h"
#include "Firewall.h"

// Constructor implementation
TLSServer::TLSServer(const std::string& cert_file, const std::string& key_file, int port, const std::string& ip)
    : port_(port), ip_(ip), server_fd_(-1), ctx_(nullptr), address_(), controller(), firewall() {

    // Initialize OpenSSL library
    if (!init_openssl()) {
        std::cerr << "Failed to initialize OpenSSL." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create SSL context
    if (!create_context()) {
        std::cerr << "Failed to create SSL context." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configure context with cert and key
    if (!configure_context(cert_file, key_file)) {
        std::cerr << "Failed to configure SSL context." << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Destructor implementation
TLSServer::~TLSServer() {
    if (server_fd_ != -1) {
        close(server_fd_);
    }
    if (ctx_) {
        SSL_CTX_free(ctx_);
    }
    cleanup_openssl();
}

bool TLSServer::init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    return true;
}

void TLSServer::cleanup_openssl() {
    EVP_cleanup();
}

bool TLSServer::create_context() {
    // Use a flexible method that supports all TLS versions (adjust if needed)
    const SSL_METHOD* method = TLS_server_method();
    ctx_ = SSL_CTX_new(method);
    if (!ctx_) {
        ERR_print_errors_fp(stderr);
        return false;
    }
    return true;
}

bool TLSServer::configure_context(const std::string& cert_file, const std::string& key_file) {
    // Load the server’s certificate
    if (SSL_CTX_use_certificate_file(ctx_, cert_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return false;
    }

    // Load the server’s private key
    if (SSL_CTX_use_PrivateKey_file(ctx_, key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return false;
    }

    // Check if the server certificate and private key match
    if (!SSL_CTX_check_private_key(ctx_)) {
        std::cerr << "Private key does not match the public certificate." << std::endl;
        return false;
    }

    return true;
}

void TLSServer::sendData(SSL* ssl, const std::string& data) {
    int bytes_sent = SSL_write(ssl, data.c_str(), data.size());
    
    if (bytes_sent <= 0) {
        int error = SSL_get_error(ssl, bytes_sent);
        std::cerr << "SSL_write error: " << ERR_error_string(error, nullptr) << std::endl;
    } else {
        std::cout << "Successfully sent " << bytes_sent << " bytes." << std::endl;
    }
}

void TLSServer::sendAccept(SSL* ssl, const std::string& message) {
    AcceptMessage accept_message(message);
    sendData(ssl, accept_message.serialize());
}

void TLSServer::sendReject(SSL* ssl, const std::string& message) {
    RejectMessage reject_message(message);
    sendData(ssl, reject_message.serialize());
}

bool TLSServer::start() {
    // Add firewall rules
    firewall.addAllowedIP("127.0.0.1"); // Example allowed IP
    firewall.addBlockedIP("203.0.113.1");   // Example blocked IP

    // Create a TCP socket
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        perror("socket failed");
        return false;
    }

    int opt = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd_);
        return false;
    }

    memset(&address_, 0, sizeof(address_));
    address_.sin_family = AF_INET;
    address_.sin_port = htons(port_);

    // Convert the human-readable IP address string into a binary form and store it in sin_addr
    if (inet_pton(AF_INET, ip_.c_str(), &address_.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(server_fd_);
        return false;
    }

    // Bind the socket
    if (bind(server_fd_, (struct sockaddr*)&address_, sizeof(address_)) < 0) {
        perror("bind failed");
        close(server_fd_);
        return false;
    }

    // Listen for incoming connections
    if (listen(server_fd_, 5) < 0) {
        perror("listen failed");
        close(server_fd_);
        return false;
    }

    std::cout << "TLS Server is listening on port " << port_ << "..." << std::endl;
    return true;
}

void TLSServer::run() {
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue; // keep accepting next clients
        }

        // Get client IP
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        // Firewall checks
        if (!firewall.isAllowed(client_ip)) {
            close(client_fd);
            continue;
        }

        if (firewall.isRateLimited(client_ip)) {
            close(client_fd);
            continue;
        }

        // Create an SSL object for the new connection
        SSL* ssl = SSL_new(ctx_);
        SSL_set_fd(ssl, client_fd);

        // Perform the TLS/SSL handshake
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            close(client_fd);
            continue;
        }

        std::cout << "New TLS connection established." << std::endl;


        // Launch a thread to handle the client
        std::thread client_thread(&TLSServer::handleClient, this, ssl, client_fd);
        client_thread.detach();
    }
}

void TLSServer::handleClient(SSL* ssl, int client_socket) {
    try{
        char buffer[1024] = {0};

        while (true){
            memset(buffer, 0, sizeof(buffer));

            int valread = SSL_read(ssl, buffer, sizeof(buffer));
            if (valread <= 0) {
                std::cerr << "Failed to read message" << std::endl;
                SSL_free(ssl);
                close(client_socket);
                return;
            }

            std::string received_message(buffer, valread);

            // Get action type from the message
            size_t pos = received_message.find(':');
            if (pos == std::string::npos) {
                sendReject(ssl, "Invalid message format.");
                //std::cout << "Invalid message format." << std::endl;
                return;
            }

            std::string response = controller.messageHandler(ssl, received_message);
            sendData(ssl, response);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    controller.logoutUserBySSL(ssl);
    SSL_free(ssl);
    close(client_socket);
}
