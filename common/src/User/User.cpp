#include "User.h"

// Constructor to initialize the id and username
User::User(int userId, const std::string& userName, const std::string& password, bool isValidator)
    : id(userId), username(userName), is_validator(isValidator), password(password) {}


User::User() {}

// Setter for id
void User::setId(int userId) {
    this->id = userId;
}

// Setter for username
void User::setUsername(std::string username) {
    this->username = username;
}

// Getter for id
int User::getId() const {
    return id;
}

// Getter for password
const std::string User::getPassword() {
    return password;
}

// Getter for username
const std::string& User::getUsername() const {
    return username;
}
