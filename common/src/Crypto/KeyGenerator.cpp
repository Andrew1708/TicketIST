#include "KeyGenerator.h"
#include <iostream>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

bool KeyGenerator::generateKeyPair(const std::string& privateKeyFile, const std::string& publicKeyFile, int keySize) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx) {
        std::cerr << "Error creating EVP_PKEY_CTX" << std::endl;
        return false;
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        std::cerr << "Error initializing key generation" << std::endl;
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, keySize) <= 0) {
        std::cerr << "Error setting RSA key size" << std::endl;
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    EVP_PKEY* evpKey = nullptr;
    if (EVP_PKEY_keygen(ctx, &evpKey) <= 0) {
        std::cerr << "Error generating key pair" << std::endl;
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    FILE* privateKeyFilePtr = fopen(privateKeyFile.c_str(), "wb");
    if (!privateKeyFilePtr) {
        std::cerr << "Error opening private key file for writing" << std::endl;
        EVP_PKEY_free(evpKey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    if (PEM_write_PrivateKey(privateKeyFilePtr, evpKey, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
        std::cerr << "Error writing private key to file" << std::endl;
        fclose(privateKeyFilePtr);
        EVP_PKEY_free(evpKey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }
    fclose(privateKeyFilePtr);

    FILE* publicKeyFilePtr = fopen(publicKeyFile.c_str(), "wb");
    if (!publicKeyFilePtr) {
        std::cerr << "Error opening public key file for writing" << std::endl;
        EVP_PKEY_free(evpKey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    if (PEM_write_PUBKEY(publicKeyFilePtr, evpKey) != 1) {
        std::cerr << "Error writing public key to file" << std::endl;
        fclose(publicKeyFilePtr);
        EVP_PKEY_free(evpKey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }
    fclose(publicKeyFilePtr);

    EVP_PKEY_free(evpKey);
    EVP_PKEY_CTX_free(ctx);

    return true;
}

bool KeyGenerator::generateKey(std::string& key, int length) {
    static const char charset[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";
    const size_t charsetSize = sizeof(charset) - 1; // exclude the null terminator

    // Resize the key to the desired length
    key.clear();
    key.reserve(length);

    // Temporary buffer for random bytes
    std::vector<unsigned char> buffer(length);
    if (RAND_bytes(buffer.data(), length) != 1) {
        std::cerr << "Error generating random bytes" << std::endl;
        return false;
    }

    // Map each random byte to a character in the charset
    for (int i = 0; i < length; ++i) {
        unsigned char randByte = buffer[i];
        key.push_back(charset[randByte % charsetSize]);
    }

    return true;
}

// Define the static member functions properly
std::string KeyGenerator::readPrivateKey(std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open key file: " + path);
    }

    std::ostringstream contentStream;
    contentStream << file.rdbuf();
    file.close();
    std::string keyContent = contentStream.str();

    BIO* bio = BIO_new_mem_buf(keyContent.data(), (int)keyContent.size());
    if (!bio) {
        throw std::runtime_error("Failed to create BIO for key content");
    }

    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    if (!pkey) {
        BIO_free(bio);
        throw std::runtime_error("Failed to read private key: " + std::string(ERR_reason_error_string(ERR_get_error())));
    }

    // Cleanup
    EVP_PKEY_free(pkey);
    BIO_free(bio);

    return keyContent;
}

std::string KeyGenerator::readPublicKey(std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open public key file: " + path);
    }

    std::ostringstream contentStream;
    contentStream << file.rdbuf();
    file.close();
    std::string keyContent = contentStream.str();

    BIO* bio = BIO_new_mem_buf(keyContent.data(), (int)keyContent.size());
    if (!bio) {
        throw std::runtime_error("Failed to create BIO for key content");
    }

    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    if (!pkey) {
        BIO_free(bio);
        throw std::runtime_error("Failed to read public key: " + std::string(ERR_reason_error_string(ERR_get_error())));
    }

    // Cleanup
    EVP_PKEY_free(pkey);
    BIO_free(bio);

    return keyContent;
}

std::string KeyGenerator::readKey(std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open key file: " + path);
    }

    std::ostringstream contentStream;
    contentStream << file.rdbuf();
    file.close();
    return contentStream.str();
}

std::string KeyGenerator::keyDerivationFunction(const std::string& saltedPassword, int iterations, int keyLength) {
    std::vector<unsigned char> derivedKey(keyLength);

    // Perform key derivation
    if (!PKCS5_PBKDF2_HMAC(saltedPassword.c_str(), saltedPassword.length(),
                           nullptr, 0,  // No separate salt
                           iterations, EVP_sha256(),
                           keyLength, derivedKey.data())) {
        throw std::runtime_error("Key derivation failed");
    }

    // Convert the derived key to a string
    return std::string(derivedKey.begin(), derivedKey.end());
}