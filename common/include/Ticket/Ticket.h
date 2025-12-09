#ifndef TICKET_H
#define TICKET_H

#include <string>
#include <memory>
#include <ctime> // For storing the purchase date

class Ticket {
public:
    static int nextID; 
    // Constructors
    Ticket(int ticketID, int eventID, int sellerID, int buyerID,
               double price, const std::string& seat, const std::string& validationCode);

    Ticket(int eventID, int sellerID, int buyerID,
           double price, const std::string& seat, const std::string& validationCode);

    Ticket(int ticketID, int eventID, int sellerID, int buyerID,
           double price, const std::string& seat, const std::string& validationCode,
           const std::string& purchaseDate);

    Ticket();

    // Getter functions
    int getTicketID() const;
    int getEventID() const;
    int getSellerID() const;
    int getBuyerID() const;
    std::string getPurchaseDate() const;
    double getPrice() const;
    std::string getSeat() const;
    std::string getValidationCode() const;

    // Methods
    void changeOwnership(int newBuyerID);
    void printTicket() const;

    // Serialization
    std::string serialize() const;

    // Deserialization
    static bool deserialize(const std::string& data, Ticket& ticket);

    static std::string generateValidationCode(int length);

    static std::string generateRandomSeat();

private:
                                // Static variable to generate unique ticket IDs
    int ticketID;               // Unique ticket ID
    int eventID;                // ID of the event this ticket belongs to
    int sellerID;               // Last person who sold the ticket
    int buyerID;                // Current owner of the ticket
    double price;               // Price of the ticket
    std::string seat;           // Seat associated with the ticket
    std::string validationCode; // Validation code to validate the ticket
    std::string purchaseDate;   // Date the ticket was purchased
};

#endif // TICKET_H
