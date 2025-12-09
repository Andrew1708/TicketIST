#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include <string>
#include <vector>
#include <optional>
#include "Repository.h"
#include "User.h"

class UserRepository: public Repository {
    public:
        UserRepository();

        bool createUser(const std::string& username, const std::string& password);
        std::optional<User> getUserById(int userId);
        std::vector<User> getAllUsers();
        std::optional<User> findUserByUsername(const std::string& username);
        bool checkPassword(std::string username, std::string password);
        bool checkValidatorPassword(std::string username, std::string password);
        int getUserId(const std::string& username); 

};

#endif // USERREPOSITORY_H