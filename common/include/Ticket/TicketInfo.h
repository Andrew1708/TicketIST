#ifndef TICKETINFO_H
#define TICKETINFO_H

#include <string>



class TicketInfo {
public:
    TicketInfo(int ticketID, int eventID, int sellerID, double price, const std::string& seat);

    int getTicketID() const;
    int getEventID() const;
    int getSellerID() const;
    double getPrice() const;
    std::string getSeat() const;
    std::string toString() const;

    void displayTicketInfo() const;


private:
    int ticketID;               // Unique ticket ID
    int eventID;                // ID of the event this ticket belongs to
    int sellerID;               // Last person who sold the ticket
    double price;               // Price of the ticket
    std::string seat;           // Seat associated with the ticket
};

#endif // TICKETINFO_H