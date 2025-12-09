#ifndef SIGNUPMESSAGE_H
#define SIGNUPMESSAGE_H

#include "Message.h"

class SignUpMessage : public Message {
private:
    std::string username;
    std::string password;

public:
    // Constructor for SignUpMessage
    SignUpMessage(const std::string& user, const std::string& pass);

    // Construcot with bytes
    static SignUpMessage deserialize(const std::string& serialized);

    // Get the username
    std::string get_username() const;

    // Get the password
    std::string get_password() const;

    // Serialize method to convert the message into a byte stream
    std::string serialize() const override;
};

#endif // SIGNUPMESSAGE_H
