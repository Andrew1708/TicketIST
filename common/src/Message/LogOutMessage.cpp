#include "LogOutMessage.h"
#include <sstream>
#include <stdexcept>

// Constructor
LogOutMessage::LogOutMessage(const std::string& username, const std::string& sessionCookies, long nonce)
    : AuthenticatedMessage(username, sessionCookies, nonce) {
    this->set_action(ActionType::LOG_OUT); // Set the action type to LOG_OUT
}

// Serialize the LogOutMessage into a string
std::string LogOutMessage::serialize() const {
    std::stringstream ss;

    // Include the ActionType as an integer for serialization
    ss << static_cast<int>(get_action()) << ":"
       << getUsername() << ":"
       << getSessionCookies() << ":"
       << getNonce();

    return ss.str();
}

// Deserialize a string into a LogOutMessage object
LogOutMessage LogOutMessage::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string actionStr, username, sessionCookies, nonceStr;

    // Parse the serialized string
    if (std::getline(ss, actionStr, ':') &&
        std::getline(ss, username, ':') &&
        std::getline(ss, sessionCookies, ':') &&
        std::getline(ss, nonceStr)) {
        
        // Convert the action type from string to integer and validate it
        int actionType = std::stoi(actionStr);
        if (static_cast<ActionType>(actionType) != ActionType::LOG_OUT) {
            throw std::invalid_argument("Error: Not a valid LogOutMessage!");
        }

        // Convert nonce from string to long
        long nonce = std::stol(nonceStr);

        // Create the LogOutMessage object and set the action type
        LogOutMessage message(username, sessionCookies, nonce);
        message.set_action(ActionType::LOG_OUT);

        return message;
    } else {
        throw std::invalid_argument("Error: Invalid message format!");
    }
}
