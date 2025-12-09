#include "CryptographicLibrary.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <memory>
#include <iomanip>

// AES Block size for padding
#define AES_BLOCK_SIZE 16

namespace {

// Handle OpenSSL Errors
std::string handleOpenSSLError(const std::string& context) {
    unsigned long err = ERR_get_error();
    char buf[120];
    ERR_error_string_n(err, buf, sizeof(buf));
    std::cerr << context << ": " << buf << std::endl;
    return "";
}

// Load RSA Key (private/public)
EVP_PKEY* loadKey(const std::string& key, bool isPrivateKey) {
    BIO* bio = BIO_new_mem_buf(key.c_str(), -1);
    if (!bio) {
        handleOpenSSLError("BIO_new_mem_buf");
        return nullptr;
    }
    EVP_PKEY* pkey = isPrivateKey
                         ? PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr)
                         : PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (!pkey) {
        handleOpenSSLError(isPrivateKey ? "PEM_read_bio_PrivateKey" : "PEM_read_bio_PUBKEY");
    }
    return pkey;
}

// Perform RSA Operation (Encrypt/Decrypt)
std::string performRSAOperation(const std::string& data, EVP_PKEY* pkey, bool encrypt) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) return handleOpenSSLError("EVP_PKEY_CTX_new");

    if ((encrypt ? EVP_PKEY_encrypt_init(ctx) : EVP_PKEY_decrypt_init(ctx)) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return handleOpenSSLError("EVP_PKEY operation init");
    }

    size_t outlen;
    if ((encrypt ? EVP_PKEY_encrypt(ctx, nullptr, &outlen, reinterpret_cast<const unsigned char*>(data.c_str()), data.size())
                 : EVP_PKEY_decrypt(ctx, nullptr, &outlen, reinterpret_cast<const unsigned char*>(data.c_str()), data.size())) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return handleOpenSSLError("EVP_PKEY size query");
    }

    std::vector<unsigned char> buffer(outlen);
    if ((encrypt ? EVP_PKEY_encrypt(ctx, buffer.data(), &outlen, reinterpret_cast<const unsigned char*>(data.c_str()), data.size())
                 : EVP_PKEY_decrypt(ctx, buffer.data(), &outlen, reinterpret_cast<const unsigned char*>(data.c_str()), data.size())) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return handleOpenSSLError("EVP_PKEY operation");
    }

    EVP_PKEY_CTX_free(ctx);
    return std::string(buffer.begin(), buffer.begin() + outlen);
}

} // namespace

//--------------------------------------------------------------------------------------------
// Check if data is encrypted
//--------------------------------------------------------------------------------------------
bool CryptographicLibrary::check(const std::string& isEncrypted) {
    return !isEncrypted.empty();
}

//--------------------------------------------------------------------------------------------
// Protect Symmetrically
//--------------------------------------------------------------------------------------------
std::string CryptographicLibrary::protectSymetric(const std::string& toEncrypt, const std::string& key) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Error creating EVP context" << std::endl;
        return "";
    }

    unsigned char iv[AES_BLOCK_SIZE];
    if (!RAND_bytes(iv, AES_BLOCK_SIZE)) {
        std::cerr << "Error generating IV" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    // Determine cipher based on key size
    const EVP_CIPHER* cipher = nullptr;
    if (key.size() == 16) {
        cipher = EVP_aes_128_cbc();
    } else if (key.size() == 24) {
        cipher = EVP_aes_192_cbc();
    } else if (key.size() == 32) {
        cipher = EVP_aes_256_cbc();
    } else {
        std::cerr << "Error: Unsupported key size. Must be 16, 24, or 32 bytes." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    if (1 != EVP_EncryptInit_ex(ctx, cipher, nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv)) {
        std::cerr << "Error initializing encryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    // Encrypt the data
    std::vector<unsigned char> encrypted(toEncrypt.size() + AES_BLOCK_SIZE);
    int outlen1 = 0, outlen2 = 0;

    if (1 != EVP_EncryptUpdate(ctx, encrypted.data(), &outlen1, reinterpret_cast<const unsigned char*>(toEncrypt.c_str()), toEncrypt.size())) {
        std::cerr << "Error encrypting data" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    if (1 != EVP_EncryptFinal_ex(ctx, encrypted.data() + outlen1, &outlen2)) {
        std::cerr << "Error finalizing encryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    EVP_CIPHER_CTX_free(ctx);

    // Return IV + ciphertext
    return std::string(reinterpret_cast<char*>(iv), AES_BLOCK_SIZE) +
           std::string(encrypted.begin(), encrypted.begin() + outlen1 + outlen2);
}


//--------------------------------------------------------------------------------------------
// Unprotect Symmetrically
//--------------------------------------------------------------------------------------------
/* std::string CryptographicLibrary::unprotectSymetric(const std::string& toDecrypt, const std::string& key) {
    if (toDecrypt.size() < AES_BLOCK_SIZE) {
        std::cerr << "Error: Encrypted data too short" << std::endl;
        return "";
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Error creating EVP context" << std::endl;
        return "";
    }

    unsigned char iv[AES_BLOCK_SIZE];
    std::memcpy(iv, toDecrypt.data(), AES_BLOCK_SIZE);

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv)) {
        std::cerr << "Error initializing decryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    std::string encryptedData = toDecrypt.substr(AES_BLOCK_SIZE);
    std::vector<unsigned char> decrypted(encryptedData.size() + AES_BLOCK_SIZE);
    int outlen1 = decrypted.size();
    int outlen2 = 0;

    if (1 != EVP_DecryptUpdate(ctx, decrypted.data(), &outlen1, reinterpret_cast<const unsigned char*>(encryptedData.c_str()), encryptedData.size())) {
        std::cerr << "Error decrypting data" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    if (1 != EVP_DecryptFinal_ex(ctx, decrypted.data() + outlen1, &outlen2)) {
        std::cerr << "Error finalizing decryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    EVP_CIPHER_CTX_free(ctx);

    std::string result(decrypted.begin(), decrypted.begin() + outlen1 + outlen2);
    unpadData(result);
    return result;
} */

std::string CryptographicLibrary::unprotectSymetric(const std::string& toDecrypt, const std::string& key) {
    if (toDecrypt.size() < AES_BLOCK_SIZE) {
        throw std::runtime_error("Error: Encrypted data too short");
        return "";
    }

    // Determine cipher based on key size
    const EVP_CIPHER* cipher = nullptr;
    if (key.size() == 16) {
        cipher = EVP_aes_128_cbc();
    } else if (key.size() == 24) {
        cipher = EVP_aes_192_cbc();
    } else if (key.size() == 32) {
        cipher = EVP_aes_256_cbc();
    } else {
        throw std::runtime_error("Error: Unsupported key size. Must be 16, 24, or 32 bytes.");
        return "";
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Error creating EVP context");
        return "";
    }

    // Extract IV
    unsigned char iv[AES_BLOCK_SIZE];
    std::memcpy(iv, toDecrypt.data(), AES_BLOCK_SIZE);

    if (1 != EVP_DecryptInit_ex(ctx, cipher, nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv)) {
        throw std::runtime_error("Error initializing decryption");
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    // Decrypt the data
    std::string encryptedData = toDecrypt.substr(AES_BLOCK_SIZE);
    std::vector<unsigned char> decrypted(encryptedData.size() + AES_BLOCK_SIZE);
    int outlen1 = 0, outlen2 = 0;

    if (1 != EVP_DecryptUpdate(ctx, decrypted.data(), &outlen1, reinterpret_cast<const unsigned char*>(encryptedData.c_str()), encryptedData.size())) {
        throw std::runtime_error("Error decrypting data");
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    if (1 != EVP_DecryptFinal_ex(ctx, decrypted.data() + outlen1, &outlen2)) {
        throw std::runtime_error("Error finalizing decryption");
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    EVP_CIPHER_CTX_free(ctx);

    // Construct the resulting plaintext
    return std::string(decrypted.begin(), decrypted.begin() + outlen1 + outlen2);
}



//--------------------------------------------------------------------------------------------
// Protect Asymmetrically
//--------------------------------------------------------------------------------------------
std::string CryptographicLibrary::protectAsymetric(const std::string& toEncrypt, const std::string& key) {
    EVP_PKEY* pkey = loadKey(key, false);
    if (!pkey) return "";

    std::string result;
    result = performRSAOperation(toEncrypt, pkey, true);
    EVP_PKEY_free(pkey);
    return result;
}

//--------------------------------------------------------------------------------------------
// Unprotect Asymmetrically
//--------------------------------------------------------------------------------------------
std::string CryptographicLibrary::unprotectAsymetric(const std::string& toDecrypt, const std::string& key) {
    EVP_PKEY* pkey = loadKey(key, true);
    if (!pkey) return "";
    std::string result;
    result = performRSAOperation(toDecrypt, pkey, false);
    EVP_PKEY_free(pkey);
    return result;
}

//--------------------------------------------------------------------------------------------
// Pad and Unpad Data
//--------------------------------------------------------------------------------------------
void CryptographicLibrary::padData(std::string& data) {
    size_t paddingRequired = AES_BLOCK_SIZE - (data.size() % AES_BLOCK_SIZE);
    data.append(paddingRequired, static_cast<char>(paddingRequired));
}

void CryptographicLibrary::unpadData(std::string& data) {
    if (data.empty()) return;
    size_t paddingSize = static_cast<size_t>(data[data.size() - 1]);
    data.resize(data.size() - paddingSize);
}


// Function to compute SHA-256 hash and return it as a hex string
std::string CryptographicLibrary::computeSHA256(const std::string& data) {
    // Create an OpenSSL EVP_MD_CTX context for the hash operation
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    // Initialize the context with SHA-256 algorithm
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize SHA-256 context");
    }

    // Update the context with data
    if (EVP_DigestUpdate(ctx, data.c_str(), data.size()) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to update SHA-256 context");
    }

    // Finalize the hash computation
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen = 0;
    if (EVP_DigestFinal_ex(ctx, hash, &hashLen) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize SHA-256 hash");
    }

    // Clean up
    EVP_MD_CTX_free(ctx);

    // Convert hash to hexadecimal string
    std::ostringstream hexStream;
    for (unsigned int i = 0; i < hashLen; ++i) {
        hexStream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return hexStream.str();
}


std::string CryptographicLibrary::getHelp() {
    std::ostringstream helpMessage;
    helpMessage << "Usage:\n";
    helpMessage << "  help\n";
    helpMessage << "      Displays this help message explaining all commands and arguments.\n\n";
    helpMessage << "  protect <input-file> <dummy-key> <output-file>\n";
    helpMessage << "      Encrypts the input file and writes to the output file using the provided key.\n\n";
    helpMessage << "  check <input-file>\n";
    helpMessage << "      Checks if the input file is encrypted.\n\n";
    helpMessage << "  unprotect <input-file> <dummy-key> <output-file>\n";
    helpMessage << "      Decrypts the input file and writes to the output file using the provided key.\n\n";
    helpMessage << "Errors:\n";
    helpMessage << "  Any invalid input or missing arguments will result in an error with helpful guidance.\n";
    return helpMessage.str();
}

bool CryptographicLibrary::validateArgs(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: No command provided. Use 'help' for guidance.\n";
        return false;
    }

    const std::string& command = args[0];
    if (command == "help") {
        return true; // No additional validation needed for help.
    } else if (command == "protect" || command == "unprotect") {
        if (args.size() < 4) {
            std::cerr << "Error: Missing arguments for '" << command << "' command.\n";
            std::cerr << "Usage: " << command << " <input-file> <dummy-key> <output-file>\n";
            return false;
        }
    } else if (command == "check") {
        if (args.size() < 2) {
            std::cerr << "Error: Missing arguments for 'check' command.\n";
            std::cerr << "Usage: check <input-file>\n";
            return false;
        }
    } else {
        std::cerr << "Error: Unknown command '" << command << "'. Use 'help' for guidance.\n";
        return false;
    }

    return true;
}
