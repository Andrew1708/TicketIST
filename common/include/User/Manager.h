// common/manager.h
#ifndef MANAGER_H
#define MANAGER_H

#include "User.h"
#include "Ticket.h"
#include <vector>

class Manager : public User {
public:
    std::vector<int> events;  // List of event IDs they created

    // Constructor
    Manager(int id, const std::string& username) : User(id, username) {}

    // Method to create an event (and tickets for that event)
    void createEvent(int eventId, const std::vector<Ticket>& tickets) {
        events.push_back(eventId);
        // Logic to add tickets to the event
        for (const Ticket& ticket : tickets) {
            // Create and assign tickets to the event
            // (This could be added to a list of tickets in a separate class)
        }
    }
};

#endif // MANAGER_H
