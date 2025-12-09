#ifndef BUYTICKETMESSAGE_H
#define BUYTICKETMESSAGE_H

#include "AuthenticatedMessage.h"
#include <string>

class BuyTicketMessage : public AuthenticatedMessage {
public:
    BuyTicketMessage(const std::string& username, const std::string& sessionCookies, long nonce, int ticketId, std::string event_name);

    int getTicketId() const;
    void setTicketId(int ticketId);

    const std::string& getEventName() const;
    void setEventName(const std::string& event_name);

    std::string serialize() const;
    static BuyTicketMessage deserialize(const std::string& data);

private:
    std::string event_name;
    int ticketId;
};

#endif // BUYTICKETMESSAGE_H
