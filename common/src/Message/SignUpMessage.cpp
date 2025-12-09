#include "SignUpMessage.h"
#include <iostream>

// Constructor for SignUpMessage
SignUpMessage::SignUpMessage(const std::string& user, const std::string& pass)
    : Message(ActionType::SIGN_UP), username(user), password(pass) {}

// Constructor with bytes
SignUpMessage SignUpMessage::deserialize(const std::string& serialized) {
    size_t first_pos = serialized.find(':');
    size_t second_pos = serialized.find(':', first_pos + 1);

    if (first_pos != std::string::npos && second_pos != std::string::npos) {
        // Extract username and password from the message
        std::string username = serialized.substr(first_pos + 1, second_pos - first_pos - 1);
        std::string password = serialized.substr(second_pos + 1);
        return SignUpMessage(username, password);

    } else {
        std::cerr << "Error: Invalid message format." << std::endl;
        throw std::invalid_argument("Error: Invalid message format!");
    }
}

// Serialize method to convert the message into a byte stream
std::string SignUpMessage::serialize() const {
    return std::to_string(static_cast<int>(ActionType::SIGN_UP)) + ":" + username + ":" + password;
}

// Get the username
std::string SignUpMessage::get_username() const {
    return username;
}

// Get the password
std::string SignUpMessage::get_password() const {
    return password;
}

