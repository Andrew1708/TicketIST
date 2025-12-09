#ifndef SELLTICKETMESSAGE_H
#define SELLTICKETMESSAGE_H

#include "AuthenticatedMessage.h"
#include <string>

class SellTicketMessage : public AuthenticatedMessage {
public:
    SellTicketMessage(const std::string username, const std::string session_cookies, long nonce, const std::string& ticket, double new_price);

    const std::string& getTicket() const;
    void setTicket(const std::string& ticket);

    double getNewPrice() const;
    void setNewPrice(double new_price);

    std::string serialize() const;
    static SellTicketMessage deserialize(const std::string& data);

private:
    std::string serealized_ticket;
    double new_price;
};

#endif // SELLTICKETMESSAGE_H