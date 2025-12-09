#ifndef GET_AVAILABLE_TICKETS_MESSAGE_H
#define GET_AVAILABLE_TICKETS_MESSAGE_H

#include "AuthenticatedMessage.h"

class GetAvailableTicketsMessage : public AuthenticatedMessage {
private:
    std::string event_name;

public:
    GetAvailableTicketsMessage(const std::string& event_name,const std::string& username, const std::string& session_cookies, long nonce);

    static GetAvailableTicketsMessage deserialize(const std::string& serialized);

    void setEventName(const std::string& event_name);

    const std::string getEventName() const;

    std::string serialize() const;
};

#endif // SEE_AVAILABLE_TICKETS_MESSAGE_H