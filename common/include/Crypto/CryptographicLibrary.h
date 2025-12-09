#ifndef CRYPTOGRAPHIC_LIBRARY_H
#define CRYPTOGRAPHIC_LIBRARY_H

#include <string>
#include <vector>

class CryptographicLibrary {
public:
    static bool check(const std::string& isEncrypted);
    static std::string protectSymetric(const std::string& toEncrypt, const std::string& key);
    static std::string unprotectSymetric(const std::string& toDecrypt, const std::string& key);
    static std::string protectAsymetric(const std::string& toEncrypt, const std::string& key);
    static std::string unprotectAsymetric(const std::string& toDecrypt, const std::string& key);
    static std::string computeSHA256(const std::string& data);
    static std::string getHelp();
    static bool validateArgs(const std::vector<std::string>& args);


private:
    static void padData(std::string& data);
    static void unpadData(std::string& data);
};

#endif // CRYPTOGRAPHIC_LIBRARY_H
