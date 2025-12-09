#ifndef LOGIN_MESSAGE_H
#define LOGIN_MESSAGE_H

#include "Message.h"

class LogInMessage : public Message {
private:
    std::string username;
    std::string password;

public:
    // Constructor with username and password
    LogInMessage(const std::string& username, const std::string& password);
    
    // Constructor with serialized bytes
    static LogInMessage deserialize(const std::string& serialized);
    
    // Get the username
    std::string get_username() const;

    // Get the password
    std::string get_password() const;
    
    // Serialize method to convert the message into a string
    std::string serialize() const;
};

#endif // LOGIN_MESSAGE_H
