#include "RejectAuthMessage.h"
#include <stdexcept>

// Constructor
RejectAuthMessage::RejectAuthMessage(const std::string& message, const std::string& username, const std::string& cookie, long nonce)
    : AuthenticatedMessage(username, cookie, nonce), message(message){
    // Set the action type to REJECT_AUTH
    this->set_action(ActionType::REJECT_AUTH);
}

// Getter for message
const std::string& RejectAuthMessage::getMessage() const {
    return message;
}

// Setter for message
void RejectAuthMessage::setMessage(const std::string& message) {
    this->message = message;
}

// Serialize the object into a string
std::string RejectAuthMessage::serialize() const {
    std::stringstream ss;

    // Format: "ActionType:Message:Cookie:Nonce"
    ss << static_cast<int>(ActionType::REJECT_AUTH) << ":"
        << this->getUsername() << ":"
        << this->getSessionCookies() << ":"
        << this->getNonce() << ":"
        << message;

    return ss.str();
}


RejectAuthMessage RejectAuthMessage::deserialize(const std::string& data) {
    std::size_t pos = 0;

    // Extract ActionType
    pos = data.find(':');
    if (pos == std::string::npos) throw std::invalid_argument("Error: Invalid message format!");
    std::string action_type_str = data.substr(0, pos);
    int action_type = std::stoi(action_type_str);
    if (action_type != static_cast<int>(ActionType::REJECT_AUTH)) {
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
    return RejectAuthMessage(message, username, cookie, nonce);
}

