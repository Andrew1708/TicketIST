#include "UserService.h"
#include "CryptographicLibrary.h"
#include "../Crypto/KeyGenerator.h"

UserService::UserService() : user_repository() {}

bool UserService::signUp(const std::string& username, const std::string& password) {
    // Check if the user already exists
    if (user_repository.findUserByUsername(username).has_value()) {
        std::cout << "User already exists" << std::endl;
        return false;
    } 

    std::cout << "SignUp successful" << std::endl;

    std::string hashed_pwd = CryptographicLibrary::computeSHA256(password); 

    // Create the user with the encrypted password
    return user_repository.createUser(username, hashed_pwd);
}


bool UserService::loginUser(const std::string& username, const std::string& password) {
    std::string pwd = CryptographicLibrary::computeSHA256(password);
    if (user_repository.checkPassword(username, pwd)){
        std::cout << "Login successful" << std::endl;
        return true;
    } else {
        std::cout << "Login failed" << std::endl;
        return false;
    }
}

bool UserService::loginValidator(const std::string& username, const std::string& password) {
    std::string pwd = CryptographicLibrary::computeSHA256(password);
    if (user_repository.checkValidatorPassword(username, pwd)){
        std::cout << "Validator login successful" << std::endl;
        return true;
    } else {
        std::cout << "Validator login failed" << std::endl;
        return false;
    }
}

int UserService::getUserId(const std::string& username) {
    int id = user_repository.getUserId(username);
    if (id == -1) {
        throw std::invalid_argument("User not found");
    }
    return id;
}