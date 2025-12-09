#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
    int id;
    std::string username;
    bool is_validator;
    std::string password;

public:
    // Constructor
    User(int userId, const std::string& userName, const std::string& password, bool isValidator = false);
    User();

    // Setters
    void setId(int userId);
    void setUsername(std::string username);

    // Getters
    int getId() const;
    const std::string& getUsername() const;
    const std::string getPassword();
};

#endif // USER_H
