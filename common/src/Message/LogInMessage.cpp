#include "Message.h"
#include "LogInMessage.h"

LogInMessage::LogInMessage(const std::string& username, const std::string& password)
    : Message(ActionType::LOG_IN), username(username), password(password) {}

    // Constructor with bytes
LogInMessage LogInMessage::deserialize(const std::string& serialized) {
// Extract the username and password from the message
    size_t first_pos = serialized.find(':');
    size_t second_pos = serialized.find(':', first_pos + 1);

    if (first_pos != std::string::npos && second_pos != std::string::npos) {
        // Extract username and password from the message
        std::string username = serialized.substr(first_pos + 1, second_pos - first_pos - 1);
        std::string password = serialized.substr(second_pos + 1);
        return LogInMessage(username, password);

    } else {
        std::cerr << "Error: Invalid message format." << std::endl;
        throw std::invalid_argument("Error: Invalid message format!");
    }
}


// Serialize method to convert the message into a byte stream
std::string LogInMessage::serialize() const {
    return std::to_string(static_cast<int>(ActionType::LOG_IN)) + ":" + username + ":" + password;
}

// Get the username
std::string LogInMessage::get_username() const {
    return username;
}

// Get the password
std::string LogInMessage::get_password() const {
    return password;
}




