#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>
#include <iostream>
using namespace std;

// Enum to define the types of actions (sign up, log in, etc.)
enum class ActionType {
    SIGN_UP,
    LOG_IN,
    LOG_IN_VALIDATOR,
    TICKET_SELL,
    GET_AVAILABLE_TICKETS,
    BUY_TICKET,
    ACCEPT,
    REJECT,
    ACCEPT_AUTH,
    REJECT_AUTH,
    VALIDATE_TICKET,
    CHECK_INTEGRITY,
    LOG_OUT,
    SELL_TICKET,
    UNKNOWN,
};

// Base class for all types of messages
class Message {
private:
   ActionType action;
public:
    // Constructor for base message class
    Message(ActionType action_type);

    // Get the action type
    ActionType get_action() const;

    // Set the action type
    void set_action(ActionType action_type);

    // Serialize method for the message
    virtual std::string serialize() const = 0;

    // Deserialize method for the message
    static Message deserialize(const std::string& serialized);
};

#endif // MESSAGE_H
