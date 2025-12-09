#ifndef REJECTAUTHMESSAGE_H
#define REJECTAUTHMESSAGE_H

#include <string>
#include <sstream>
#include "AuthenticatedMessage.h"

class RejectAuthMessage : public AuthenticatedMessage {
public:
    RejectAuthMessage(const std::string& message, const std::string& username, const std::string& cookie, long nonce);

    const std::string& getMessage() const;

    void setMessage(const std::string& message);


    std::string serialize() const;
    static RejectAuthMessage deserialize(const std::string& data);

private:
    std::string message;
};

#endif // REJECTAUTHMESSAGE_H
