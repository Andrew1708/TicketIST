#ifndef TLSCLIENT_H
#define TLSCLIENT_H

#include <string>
#include <openssl/ssl.h>

class TLSClient {
public:
    TLSClient(const std::string& hostname, int port, const std::string& ca_file, bool verify = true);
    ~TLSClient();

    void connectToServer();
    void sendData(const std::string& data);
    std::string receiveData(size_t bufferSize = 1024);

private:
    bool init_openssl();
    void cleanup_openssl();
    void create_context();
    void configure_context();

    std::string ip_;
    int port_;
    std::string ca_file_;
    bool verify_;

    int sock_;
    SSL_CTX* ctx_;
    SSL* ssl_;
};

#endif // TLSCLIENT_H
