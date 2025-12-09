#ifndef VALIDATETICKETMESSAGE_H
#define VALIDATETICKETMESSAGE_H

#include "AuthenticatedMessage.h"

class ValidateTicketMessage : public AuthenticatedMessage {
public:
    ValidateTicketMessage(const std::string& user, const std::string& session_cookie, long nonce, const std::string& val_code);
    std::string serialize() const;
    static ValidateTicketMessage deserialize(const std::string& data);

    const std::string& getValCode() const;
    void setValCode(const std::string& val_code);


private:
    std::string val_code;
};

#endif // VALIDATETICKETMESSAGE_H