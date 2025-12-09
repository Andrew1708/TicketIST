#ifndef KEYGENERATOR_H
#define KEYGENERATOR_H

#include <string>

class KeyGenerator {
public:
    // Generate RSA key pair (private and public keys)
    // Saves private key to 'privateKeyFile' and public key to 'publicKeyFile'
    static bool generateKeyPair(const std::string& privateKeyFile, const std::string& publicKeyFile, int keySize = 2048);

    // Generate a random key of the specified length (default is 32 characters)
    static bool generateKey(std::string& key, int length = 32);

    // Read key from Key_File
    static std::string readPrivateKey(std::string& path);

    // Read key from Key_File
    static std::string readPublicKey(std::string& path);

    // Read key from Key_File
    static std:: string readKey(std::string& path);

    static std::string keyDerivationFunction(const std::string& salted_pwd, int iterations, int keyLength);
};

#endif // KEYGENERATOR_H
