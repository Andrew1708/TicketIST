#include "GetAvailableTicketsMessage.h"
#include <sstream>

GetAvailableTicketsMessage::GetAvailableTicketsMessage(const std::string& event_name, const std::string& username, const std::string& session_cookies, long nonce)
    :AuthenticatedMessage(username, session_cookies, nonce), event_name(event_name) {
        this->set_action(ActionType::GET_AVAILABLE_TICKETS);
    }

// Getter for event name
const std::string GetAvailableTicketsMessage::getEventName() const {
    return event_name;
}

// Setter for event name
void GetAvailableTicketsMessage::setEventName(const std::string& event_name) {
    this->event_name = event_name;
}

// Serialization: Convert object to a single string
std::string GetAvailableTicketsMessage::serialize() const {
    std::stringstream ss;

    // Format: "ActionType:EventName:SessionCookies:Nonce"
    ss << std::to_string(static_cast<int>(ActionType::GET_AVAILABLE_TICKETS)) << ":"
       << getUsername() << ":"
       << getSessionCookies() << ":"
       << getNonce() << ":"
       << getEventName();

    return ss.str();
}

GetAvailableTicketsMessage GetAvailableTicketsMessage::deserialize(const std::string& serialized) {
    std::stringstream ss(serialized);
    std::string action_type_str, event_name, username, session_cookies, nonce_str;

    // Parse the serialized string
    if (!(std::getline(ss, action_type_str, ':') &&
          std::getline(ss, username, ':') &&
          std::getline(ss, session_cookies, ':') &&
          std::getline(ss, nonce_str, ':') &&
          std::getline(ss, event_name))) {
        throw std::invalid_argument("Error: Invalid message format! Missing fields.");
    }

    // Convert and validate ActionType
    int action_type = std::stoi(action_type_str);
    if (action_type != static_cast<int>(ActionType::GET_AVAILABLE_TICKETS)) {
        throw std::invalid_argument("Error: ActionType mismatch during deserialization!");
    }

    // Convert nonce from string to long
    long nonce;
    try {
        nonce = std::stol(nonce_str);
    } catch (const std::exception& e) {
        throw std::invalid_argument("Error: Invalid nonce format!");
    }

    // Return the reconstructed object
    return GetAvailableTicketsMessage(event_name, username, session_cookies, nonce);
}
