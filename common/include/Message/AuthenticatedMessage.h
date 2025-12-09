#ifndef AUTHENTICATEDMESSAGE_H
#define AUTHENTICATEDMESSAGE_H

#include "Message.h"
#include <string>

class AuthenticatedMessage : public Message {
public:
    AuthenticatedMessage(const std::string& username, const std::string& sessionCookies, long nonce);

    const std::string& getSessionCookies() const;
    void setSessionCookies(const std::string& sessionCookies);

    const long getNonce() const;
    void setNonce(long nonce);

    const std::string& getUsername() const;
    void setUsername(const std::string& username);

    static const std::string& generateCookie();
    static const long generateNonce();

    std::string serialize() const;
    static AuthenticatedMessage deserialize(const std::string& data);

private:
    std::string username;
    std::string session_cookies;
    long nonce;
};

#endif // AUTHENTICATEDMESSAGE_H
