#include "BuyTicketMessage.h"
#include <sstream>
#include <stdexcept>

// Constructor
BuyTicketMessage::BuyTicketMessage(const std::string& username, const std::string& sessionCookies, long nonce, int ticketId, std::string event_name)
    : AuthenticatedMessage(username, sessionCookies, nonce), ticketId(ticketId), event_name(event_name) {
    // Set the action type to BUY_TICKET
    this->set_action(ActionType::BUY_TICKET);
    }

// Getter for ticketId
int BuyTicketMessage::getTicketId() const {
    return ticketId;
}

// Setter for ticketId
void BuyTicketMessage::setTicketId(int ticketId) {
    this->ticketId = ticketId;
}

// Getter for event name
const std::string& BuyTicketMessage::getEventName() const {
    return event_name;
}

// Setter for event name
void BuyTicketMessage::setEventName(const std::string& event_name) {
    this->event_name = event_name;
}

// Serialize the object into a string
std::string BuyTicketMessage::serialize() const {
    std::stringstream ss;

    // Format: "ActionType:Username:SessionCookies:Nonce:TicketId"
    ss << static_cast<int>(ActionType::BUY_TICKET) << ":"
       << getUsername() << ":"
       << getSessionCookies() << ":"
       << getNonce() << ":"
       << ticketId << ":"
       << event_name;

    return ss.str();
}

// Deserialize a string into a BuyTicketMessage object
BuyTicketMessage BuyTicketMessage::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string action_type_str, username, sessionCookies, nonce_str, ticketId_str, event_name;

    // Parse the serialized string
    if (!(std::getline(ss, action_type_str, ':') &&
          std::getline(ss, username, ':') &&
          std::getline(ss, sessionCookies, ':') &&
          std::getline(ss, nonce_str, ':') &&
          std::getline(ss, ticketId_str, ':') && 
          std::getline(ss, event_name))) {
        throw std::invalid_argument("Error: Invalid message format!");
    }

    // Validate ActionType
    int action_type = std::stoi(action_type_str);
    if (action_type != static_cast<int>(ActionType::BUY_TICKET)) {
        throw std::invalid_argument("Error: ActionType mismatch during deserialization!");
    }

    // Convert nonce from string to long
    long nonce = std::stol(nonce_str);

    // Convert ticketId from string to int
    int ticketId = std::stoi(ticketId_str);

    // Return the reconstructed object
    return BuyTicketMessage(username, sessionCookies, nonce, ticketId, event_name);
}
