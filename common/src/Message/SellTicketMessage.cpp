#include "SellTicketMessage.h"
#include <sstream>
#include <stdexcept>

// Constructor
SellTicketMessage::SellTicketMessage(const std::string username, const std::string session_cookies, long nonce, const std::string& ticket, double new_price)
    : AuthenticatedMessage(username, session_cookies, nonce), serealized_ticket(ticket), new_price(new_price) {
    this->set_action(ActionType::SELL_TICKET); // Set the action type to SELL_TICKET
}

// Get the ticket string
const std::string& SellTicketMessage::getTicket() const {
    return serealized_ticket;
}

// Set the ticket string
void SellTicketMessage::setTicket(const std::string& ticket) {
    serealized_ticket = ticket;
}

// Get the new price
double SellTicketMessage::getNewPrice() const {
    return new_price;
}

// Set the new price
void SellTicketMessage::setNewPrice(double new_price) {
    this->new_price = new_price;
}

// Serialize the SellTicketMessage into a string
std::string SellTicketMessage::serialize() const {
    std::stringstream ss;

    // Include the ActionType as an integer for serialization
    ss << static_cast<int>(get_action()) << ":"
       << getUsername() << ":"
       << getSessionCookies() << ":"
       << getNonce() << ":"
       << getNewPrice() << ":"
       << serealized_ticket;

    return ss.str();
}

// Deserialize a string into a SellTicketMessage object
SellTicketMessage SellTicketMessage::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string actionStr, username, sessionCookies, nonceStr, ticket, new_price_str;

    // Parse the serialized string
    if (std::getline(ss, actionStr, ':') &&
        std::getline(ss, username, ':') &&
        std::getline(ss, sessionCookies, ':') &&
        std::getline(ss, nonceStr, ':') &&
        std::getline(ss, new_price_str, ':') &&
        std::getline(ss, ticket)) {

        // Convert the action type from string to integer and validate it
        int actionType = std::stoi(actionStr);
        if (static_cast<ActionType>(actionType) != ActionType::SELL_TICKET) {
            throw std::invalid_argument("Error: Not a valid SellTicketMessage!");
        }

        // Convert nonce from string to long
        long nonce = std::stol(nonceStr);

        // Conver the new price from string to double
        double new_price = std::stod(new_price_str);

        // Create the SellTicketMessage object and set the action type
        SellTicketMessage message(username, sessionCookies, nonce, ticket, new_price);
        message.set_action(ActionType::SELL_TICKET);

        return message;
    } else {
        throw std::invalid_argument("Error: Invalid message format!");
    }
}
