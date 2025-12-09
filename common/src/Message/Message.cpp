#include "Message.h"
#include <vector>
#include <sstream>
#include "SignUpMessage.h"
#include <iostream>
#include <stdexcept>

// Constructor for the Message base class
Message::Message(ActionType action_type) : action(action_type) {}

// Getter for the action type
ActionType Message::get_action() const {
    return action;
}

// Setter for the action type
void Message::set_action(ActionType action_type) {
    action = action_type;
}

