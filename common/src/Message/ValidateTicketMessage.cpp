#include "ValidateTicketMessage.h"
#include <sstream>
#include <stdexcept>

// Constructor
ValidateTicketMessage::ValidateTicketMessage(const std::string& username, const std::string& sessionCookies, long nonce, const std::string& val_code)
    : AuthenticatedMessage(username, sessionCookies, nonce), val_code(val_code) {
    // Set the action type to VALIDATE_TICKET
    this->set_action(ActionType::VALIDATE_TICKET);
}

// Getter for validation code
const std::string& ValidateTicketMessage::getValCode() const {
    return val_code;
}

// Setter for validation code
void ValidateTicketMessage::setValCode(const std::string& val_code) {
    this->val_code = val_code;
}

// Serialize the object into a string
std::string ValidateTicketMessage::serialize() const {
    std::stringstream ss;

    // Format: "ActionType:Username:SessionCookies:Nonce:ValCode"
    ss << static_cast<int>(ActionType::VALIDATE_TICKET) << ":"
       << getUsername() << ":"
       << getSessionCookies() << ":"
       << getNonce() << ":"
       << val_code;

    return ss.str();
}

// Deserialize a string into a ValidateTicketMessage object
ValidateTicketMessage ValidateTicketMessage::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string action_type_str, username, sessionCookies, nonce_str, val_code;

    // Parse the serialized string
    if (!(std::getline(ss, action_type_str, ':') &&
          std::getline(ss, username, ':') &&
          std::getline(ss, sessionCookies, ':') &&
          std::getline(ss, nonce_str, ':') &&
          std::getline(ss, val_code))) {
        throw std::invalid_argument("Error: Invalid message format!");
    }

    // Validate ActionType
    int action_type = std::stoi(action_type_str);
    if (action_type != static_cast<int>(ActionType::VALIDATE_TICKET)) {
        throw std::invalid_argument("Error: ActionType mismatch during deserialization!");
    }

    // Convert nonce from string to long
    long nonce = std::stol(nonce_str);

    // Return the reconstructed object
    return ValidateTicketMessage(username, sessionCookies, nonce, val_code);
}
