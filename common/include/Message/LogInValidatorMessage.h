#ifndef LOGINVALIDATORMESSAGE_H
#define LOGINVALIDATORMESSAGE_H

#include <string>

class LogInValidatorMessage : public Message {
public:
    LogInValidatorMessage(const std::string& username, const std::string& password);
    
    // Constructor with serialized bytes
    static LogInValidatorMessage deserialize(const std::string& serialized);
    
    // Get the username
    std::string get_username() const;

    // Get the password
    std::string get_password() const;
    
    // Serialize method to convert the message into a string
    std::string serialize() const;

private:
    std::string username;
    std::string password;
};

#endif // LOGINVALIDATORMESSAGE_H