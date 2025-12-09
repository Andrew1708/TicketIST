#ifndef LOGOUTMESSAGE_H
#define LOGOUTMESSAGE_H

#include "AuthenticatedMessage.h"

class LogOutMessage : public AuthenticatedMessage {
public:
    LogOutMessage(const std::string& username, const std::string& sessionCookies, long nonce);

    std::string serialize() const;
    static LogOutMessage deserialize(const std::string& data);


    // Additional methods and members for LogOutMessage can be added here
};

#endif // LOGOUTMESSAGE_H