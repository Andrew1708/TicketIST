// common/buyerSeller.h
#ifndef BUYERSELLER_H
#define BUYERSELLER_H

#include "User.h"
#include "Ticket.h"
#include <vector>
#include <bits/algorithmfwd.h>

class BuyerSeller : public User {
public:
    std::vector<Ticket> ownedTickets;

    // Constructor
    BuyerSeller(int id, const std::string& username) : User(id, username) {}

    // Method to buy a ticket
    void buy(const Ticket& ticket) {
        ownedTickets.push_back(ticket);
    }

    // Method to sell a ticket
    void sell(int ticketId) {
        // Logic to remove a ticket from ownedTickets based on ticketId
        auto it = std::find_if(ownedTickets.begin(), ownedTickets.end(), [ticketId](const Ticket& t) {
            return t.getTicketID() == ticketId;
        });

        if (it != ownedTickets.end()) {
            ownedTickets.erase(it);
        }
    }
};

#endif // BUYERSELLER_H
