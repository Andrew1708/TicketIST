#include "AcceptAuthMessage.h"
#include <stdexcept>

// Constructor
AcceptAuthMessage::AcceptAuthMessage(const std::string& message, const std::string& username, const std::string& cookie, long nonce)
    : AuthenticatedMessage(username, cookie, nonce), message(message) {
    // Set the action type to ACCEPT_AUTH
    this->set_action(ActionType::ACCEPT_AUTH);
}

// Getter for message
const std::string& AcceptAuthMessage::getMessage() const {
    return message;
}

// Setter for message
void AcceptAuthMessage::setMessage(const std::string& message) {
    this->message = message;
}

// Serialize the object into a string
std::string AcceptAuthMessage::serialize() const {
    std::stringstream ss;

    // Format: "ActionType:Message:Cookie:Nonce"
    ss << static_cast<int>(ActionType::ACCEPT_AUTH) << ":"
       << getUsername() << ":"
       << getSessionCookies() << ":"
       << getNonce() << ":"
       << message;

    return ss.str();
}

// Deserialize a string into an AcceptAuthMessage object
AcceptAuthMessage AcceptAuthMessage::deserialize(const std::string& data) {
    std::size_t pos = 0;

    // Extract ActionType
    pos = data.find(':');
    if (pos == std::string::npos) throw std::invalid_argument("Error: Invalid message format!");
    std::string action_type_str = data.substr(0, pos);
    int action_type = std::stoi(action_type_str);
    if (action_type != static_cast<int>(ActionType::ACCEPT_AUTH)) {
        throw std::invalid_argument("Error: ActionType mismatch during deserialization!");
    }
    
    // Extract Username
    std::size_t prev_pos = pos + 1;
    pos = data.find(':', prev_pos);
    if (pos == std::string::npos) throw std::invalid_argument("Error: Invalid message format!");
    std::string username = data.substr(prev_pos, pos - prev_pos);

    // Extract Cookie
    prev_pos = pos + 1;
    pos = data.find(':', prev_pos);
    if (pos == std::string::npos) throw std::invalid_argument("Error: Invalid message format!");
    std::string cookie = data.substr(prev_pos, pos - prev_pos);

    // Extract Nonce
    prev_pos = pos + 1;
    pos = data.find(':', prev_pos);
    if (pos == std::string::npos) throw std::invalid_argument("Error: Invalid message format!");
    std::string nonce_str = data.substr(prev_pos, pos - prev_pos);
    long nonce = std::stol(nonce_str);

    // Extract Message (Remaining string)
    prev_pos = pos + 1;
    if (prev_pos >= data.length()) throw std::invalid_argument("Error: Invalid message format!");
    std::string message = data.substr(prev_pos);

    // Return reconstructed object
    return AcceptAuthMessage(message, username, cookie, nonce);
}
