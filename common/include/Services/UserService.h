#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include <string>
#include <vector>
#include "UserRepository.h"

class UserService {
private:
    UserRepository user_repository;

public:
    UserService();

    bool signUp(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
    bool loginValidator(const std::string& username, const std::string& password);
    int getUserId(const std::string& username);

};

#endif // USER_SERVICE_H