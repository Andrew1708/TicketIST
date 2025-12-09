#include <iostream>
#include <fstream>
#include <cassert>
#include "KeyGenerator.h"
#include "CryptographicLibrary.h"
#define PRIVATE_KEY "../private_key.pem"
#define PUBLIC_KEY "../public_key.pem"
#define RANDOM_KEY "../random_key.txt" 
#define MESSAGE "i am a (very cute) test message"

// Unit test for encryption and decryption
void Symmetric(std::string message) {
    const std::string randomKeyFile = RANDOM_KEY;
    // Read the symmetric key back from the file
    std::string symmetricKey = KeyGenerator::readKey((std::string&)randomKeyFile);
    if (symmetricKey.empty()) { std::cerr << "Failed to read symmetric key from file." << std::endl; return; }
    // --- Symmetric Encryption/Decryption Test ---
    std::string symEncrypted = CryptographicLibrary::protectSymetric(message, symmetricKey);
    if (symEncrypted.empty()) { std::cerr << "Symmetric encryption failed." << std::endl; }
    else { std::cout << "Symmetrically Encrypted: " << symEncrypted << std::endl; }
    // Check if the library detects it as encrypted
    if (CryptographicLibrary::check(symEncrypted)) { std::cout << "Symmetric encrypted data recognized as encrypted." << std::endl; }
    else { std::cerr << "Symmetric encrypted data not recognized as encrypted." << std::endl; }
    std::string symDecrypted = CryptographicLibrary::unprotectSymetric(symEncrypted, symmetricKey);
    if (symDecrypted == message) { std::cout << "Symmetric decryption successful. Decrypted message: " << symDecrypted << std::endl; }
    else { std::cerr << "Symmetric decryption failed. Decrypted message does not match the original." << std::endl; }
}

void Asymetric(std::string message) {
    const std::string privateKeyFile = PRIVATE_KEY;
    const std::string publicKeyFile = PUBLIC_KEY;
    // Read Keys Back
    std::string privateKeyContent = KeyGenerator::readPrivateKey((std::string&)privateKeyFile);
    std::string publicKeyContent = KeyGenerator::readPublicKey((std::string&)publicKeyFile);
    if (privateKeyContent.empty() || publicKeyContent.empty()) { std::cerr << "Failed to read one or both RSA keys." << std::endl; return; }
    // --- Asymmetric Encryption/Decryption Test ---
    // Encrypt using the public key
    std::string asymEncrypted = CryptographicLibrary::protectAsymetric(message, publicKeyContent);
    if (asymEncrypted.empty()) { std::cerr << "Asymmetric encryption failed." << std::endl; }
    else { std::cout << "Asymmetrically Encrypted: " << asymEncrypted << std::endl; }

    // Check if the library detects it as encrypted
    if (CryptographicLibrary::check(asymEncrypted)) {std::cout << "Asymmetric encrypted data recognized as encrypted." << std::endl; }
    else { std::cerr << "Asymmetric encrypted data not recognized as encrypted." << std::endl; }

    // Decrypt using the private key
    std::string asymDecrypted = CryptographicLibrary::unprotectAsymetric(asymEncrypted, privateKeyContent);
    if (asymDecrypted == message) { std::cout << "Asymmetric decryption successful. Decrypted message: " << asymDecrypted << std::endl; }
    else { std::cerr << "Asymmetric decryption failed. Decrypted message does not match the original." << std::endl; }
}

int main() {
    Symmetric(MESSAGE);
    Asymetric(MESSAGE);
    return 0;
}
