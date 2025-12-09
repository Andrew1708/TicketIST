#include <iostream>
#include <fstream>
#include <cassert>
#include "KeyGenerator.h"
#include "CryptographicLibrary.h"
#define KEYSIZE 2048
#define PRIVATE_KEY "private_key.pem"
#define PUBLIC_KEY "public_key.pem"
#define RANDOM_KEY "random_key.txt"

int main() {
    const std::string privateKeyFile = PRIVATE_KEY;
    const std::string publicKeyFile = PUBLIC_KEY;
    const std::string randomKeyFile = RANDOM_KEY;
    const int keySize = KEYSIZE;
    return 0;
    // Generate RSA Key Pair
    if (KeyGenerator::generateKeyPair(privateKeyFile, publicKeyFile, keySize)) {
        std::cout << "RSA key pair generated successfully." << std::endl;
    } else {
        std::cerr << "Failed to generate RSA key pair." << std::endl;
        return 1;
    }
    // Generate a symmetric key
    std::string randomKey;
    if (KeyGenerator::generateKey(randomKey, 32)) {
        std::cout << "Random symmetric key generated: " << randomKey << std::endl;
        // Write the symmetric key to a file for demonstration
        std::ofstream out(randomKeyFile);
        if (out) {
            out << randomKey;
        } else {
            std::cerr << "Failed to write random key to file." << std::endl;
        }
    } else {
        std::cerr << "Failed to generate random symmetric key." << std::endl;
        return 1;
    }
}