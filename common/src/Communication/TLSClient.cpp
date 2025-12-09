#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// OpenSSL headers
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "TLSClient.h"


TLSClient::TLSClient(const std::string& ip, int port, const std::string& ca_file, bool verify)
    : ip_(ip), port_(port), ca_file_(ca_file), verify_(verify), sock_(-1), ctx_(nullptr), ssl_(nullptr)
{
    if (!init_openssl()) {
        throw std::runtime_error("Failed to initialize OpenSSL");
    }

    create_context();
    configure_context();
}

TLSClient::~TLSClient() {
    if (ssl_) {
        SSL_shutdown(ssl_);
        SSL_free(ssl_);
    }

    if (sock_ != -1) {
        close(sock_);
    }

    if (ctx_) {
        SSL_CTX_free(ctx_);
    }

    cleanup_openssl();
}

bool TLSClient::init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    return true;
}

void TLSClient::cleanup_openssl() {
    EVP_cleanup();
}

void TLSClient::create_context() {
    const SSL_METHOD *method = TLS_client_method();
    ctx_ = SSL_CTX_new(method);
    if (!ctx_) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Unable to create SSL context");
    }
}

void TLSClient::configure_context() {
    // If verification is enabled, load CA file and set verification mode
    if (verify_ && !ca_file_.empty()) {
        if (SSL_CTX_load_verify_locations(ctx_, ca_file_.c_str(), nullptr) != 1) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Failed to load CA file");
        }
        SSL_CTX_set_verify(ctx_, SSL_VERIFY_PEER, nullptr);
    } else {
        // For demonstration, disable verification. Do NOT do this in production.
        SSL_CTX_set_verify(ctx_, SSL_VERIFY_NONE, nullptr);
    }
}

void TLSClient::connectToServer() {
    // Create TCP socket
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
        perror("socket");
        throw std::runtime_error("Failed to create socket");
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, ip_.c_str(), &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock_);
        sock_ = -1;
        throw std::runtime_error("Invalid hostname/IP");
    }

    if (connect(sock_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock_);
        sock_ = -1;
        throw std::runtime_error("Failed to connect to server");
    }

    // Create SSL object
    ssl_ = SSL_new(ctx_);
    if (!ssl_) {
        close(sock_);
        sock_ = -1;
        throw std::runtime_error("Failed to create SSL object");
    }

    SSL_set_fd(ssl_, sock_);

    // Perform TLS handshake
    if (SSL_connect(ssl_) <= 0) {
        ERR_print_errors_fp(stderr);
        close(sock_);
        sock_ = -1;
        SSL_free(ssl_);
        ssl_ = nullptr;
        throw std::runtime_error("TLS handshake failed");
    }

    std::cout << "Connected with " << SSL_get_cipher(ssl_) << " encryption\n";
}

void TLSClient::sendData(const std::string& data) {
    if (!ssl_) {
        throw std::runtime_error("SSL not initialized");
    }

    int ret = SSL_write(ssl_, data.c_str(), static_cast<int>(data.size()));
    if (ret <= 0) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Failed to write data over TLS");
    }
}
std::string TLSClient::receiveData(size_t bufferSize) {
    if (!ssl_) {
        throw std::runtime_error("SSL not initialized");
    }

    std::string result;
    char buffer[bufferSize]; // Temporary buffer for each read

    while (true) {
        int ret = SSL_read(ssl_, buffer, static_cast<int>(bufferSize));
        if (ret < 0) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Failed to read data over TLS");
        } else if (ret == 0) {
            // Connection closed gracefully
            break;
        }

        // Append received data to the result
        result.append(buffer, static_cast<size_t>(ret));

        // If the amount of data read is less than the buffer size,
        // it likely indicates that the message has been fully received
        if (static_cast<size_t>(ret) < bufferSize) {
            break;
        }
    }
    
    return result;
}
