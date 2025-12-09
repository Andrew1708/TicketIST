#include "CheckIntegrityMessage.h"
#include <sstream>
#include <stdexcept>

// Constructor
CheckIntegrityMessage::CheckIntegrityMessage(const std::string& user, const std::string& session_cookie, long nonce, const std::string& val_code, const std::string& integrity_code)
    : AuthenticatedMessage(user, session_cookie, nonce), val_code(val_code), integrity_code(integrity_code) {
    // Set the action type to CHECK_INTEGRITY
    this->set_action(ActionType::CHECK_INTEGRITY);
}

// Getter for validation code
const std::string& CheckIntegrityMessage::getValCode() const {
    return val_code;
}

// Setter for validation code
void CheckIntegrityMessage::setValCode(const std::string& val_code) {
    this->val_code = val_code;
}

// Getter for integrity code
const std::string& CheckIntegrityMessage::getIntegrityCode() const {
    return integrity_code;
}

// Setter for integrity code
void CheckIntegrityMessage::setIntegrityCode(const std::string& integrity_code) {
    this->integrity_code = integrity_code;
}

// Serialize the object into a string
std::string CheckIntegrityMessage::serialize() const {
    std::stringstream ss;

    // Format: "ActionType:Username:SessionCookie:Nonce:ValCode:IntegrityCode"
    ss << static_cast<int>(ActionType::CHECK_INTEGRITY) << ":"
       << getUsername() << ":"
       << getSessionCookies() << ":"
       << getNonce() << ":"
       << val_code << ":"
       << integrity_code;

    return ss.str();
}

// Deserialize a string into a CheckIntegrityMessage object
CheckIntegrityMessage CheckIntegrityMessage::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string action_type_str, username, session_cookie, nonce_str, val_code, integrity_code;

    // Parse the serialized string
    if (!(std::getline(ss, action_type_str, ':') &&
          std::getline(ss, username, ':') &&
          std::getline(ss, session_cookie, ':') &&
          std::getline(ss, nonce_str, ':') &&
          std::getline(ss, val_code, ':') &&
          std::getline(ss, integrity_code))) {
        throw std::invalid_argument("Error: Invalid message format!");
    }

    // Validate ActionType
    int action_type = std::stoi(action_type_str);
    if (action_type != static_cast<int>(ActionType::CHECK_INTEGRITY)) {
        throw std::invalid_argument("Error: ActionType mismatch during deserialization!");
    }

    // Convert nonce from string to long
    long nonce = std::stol(nonce_str);

    // Return the reconstructed object
    return CheckIntegrityMessage(username, session_cookie, nonce, val_code, integrity_code);
}
