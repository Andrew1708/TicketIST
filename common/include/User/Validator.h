// common/validator.h
#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "User.h"
#include <vector>
#include <bits/algorithmfwd.h>

class Validator : public User {
public:
    std::vector<int> events;  // List of event IDs they can validate

    // Constructor
    Validator(int id, const std::string& username) : User(id, username, true) {}

    // Method to validate an event (simplified for now)
    bool validateEvent(int eventId) {
        return std::find(events.begin(), events.end(), eventId) != events.end();
    }
};

#endif // VALIDATOR_H
