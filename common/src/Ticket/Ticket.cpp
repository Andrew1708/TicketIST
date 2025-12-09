#include "Ticket.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <random>

int Ticket::nextID = 1; // Initialize static member

// Constructor implementation
Ticket::Ticket(int ticketID, int eventID, int sellerID, int buyerID,
               double price, const std::string& seat, const std::string& validationCode)
    : ticketID(ticketID), eventID(eventID), sellerID(sellerID), buyerID(buyerID),
      price(price), seat(seat), validationCode(validationCode), purchaseDate("") {

    // Get the current time as the purchase date
    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);

    std::stringstream ss;
    ss << 1900 + localTime->tm_year << "-"
       << 1 + localTime->tm_mon << "-"
       << localTime->tm_mday
       << " " << localTime->tm_hour << ":"
       << localTime->tm_min << ":" << localTime->tm_sec;

    purchaseDate = ss.str(); // Format the date as YYYY-MM-DD
}

// Constructor implementation
Ticket::Ticket(int eventID, int sellerID, int buyerID,
               double price, const std::string& seat, const std::string& validationCode)
    : ticketID(nextID++), eventID(eventID), sellerID(sellerID), buyerID(buyerID),
      price(price), seat(seat), validationCode(validationCode), purchaseDate("") {

    // Get the current time as the purchase date
    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);

    std::stringstream ss;
    ss << 1900 + localTime->tm_year << "-"
       << 1 + localTime->tm_mon << "-"
       << localTime->tm_mday
       << " " << localTime->tm_hour << ":"
       << localTime->tm_min << ":" << localTime->tm_sec;

    purchaseDate = ss.str(); // Format the date as YYYY-MM-DD
}

Ticket::Ticket(int ticketID, int eventID, int sellerID, int buyerID,
               double price, const std::string& seat, const std::string& validationCode, 
               const std::string& purchaseDate)
    : ticketID(ticketID), eventID(eventID), sellerID(sellerID), buyerID(buyerID),
      price(price), seat(seat), validationCode(validationCode), purchaseDate(purchaseDate) {}



// Add constructor for empty Ticket
Ticket::Ticket()
    : ticketID(0), eventID(0), sellerID(0), buyerID(0),
      price(0.0), seat(""), validationCode(""), purchaseDate("") {}

// Getter functions
int Ticket::getTicketID() const { return ticketID; }
int Ticket::getEventID() const { return eventID; }
int Ticket::getSellerID() const { return sellerID; }
int Ticket::getBuyerID() const { return buyerID; }
std::string Ticket::getPurchaseDate() const { return purchaseDate; }
double Ticket::getPrice() const { return price; }
std::string Ticket::getSeat() const { return seat; }
std::string Ticket::getValidationCode() const { return validationCode; }

// Method to change the ownership of the ticket
void Ticket::changeOwnership(int newBuyerID) {
    sellerID = buyerID;
    buyerID = newBuyerID;
}

// Serialize the Ticket into a string
std::string Ticket::serialize() const {
    std::stringstream ss;
    ss << ticketID << "|"
       << eventID << "|"
       << sellerID << "|"
       << buyerID << "|"
       << purchaseDate << "|"
       << price << "|"
       << seat << "|"
       << validationCode;
    return ss.str();
}

// Deserialize the string back into a Ticket object
bool Ticket::deserialize(const std::string& data, Ticket& ticket) {
    std::istringstream stream(data);
    std::string part;
    int ticketID, eventID, sellerID, buyerID;
    std::string purchaseDate, seat, validationCode;
    double price;

    // Deserialize data from the serialized string
    std::getline(stream, part, '|');
    ticketID = std::stoi(part);
    std::getline(stream, part, '|');
    eventID = std::stoi(part);
    std::getline(stream, part, '|');
    sellerID = std::stoi(part);
    std::getline(stream, part, '|');
    buyerID = std::stoi(part);
    std::getline(stream, purchaseDate, '|');
    std::getline(stream, part, '|');
    price = std::stod(part);
    std::getline(stream, seat, '|');
    std::getline(stream, validationCode, '|');

    // Rebuild the Ticket object
    ticket = Ticket(ticketID, eventID, sellerID, buyerID, price, seat, validationCode, purchaseDate);
    return true;
}


void Ticket::printTicket() const {
    const int fieldWidth = 20; // Width for the "Field" column
    const int valueWidth = 30; // Width for the "Value" column
    const std::string separator = "+" + std::string(fieldWidth + 2, '-') + "+" + std::string(valueWidth + 2, '-') + "+";

    std::cout << separator << std::endl;
    std::cout << "| " << std::setw(fieldWidth) << std::left << "Field"
              << " | " << std::setw(valueWidth) << std::left << "Value" << " |" << std::endl;
    std::cout << separator << std::endl;

    std::cout << "| " << std::setw(fieldWidth) << std::left << "Ticket ID"
              << " | " << std::setw(valueWidth) << std::left << ticketID << " |" << std::endl;
    std::cout << "| " << std::setw(fieldWidth) << std::left << "Event ID"
              << " | " << std::setw(valueWidth) << std::left << eventID << " |" << std::endl;
    std::cout << "| " << std::setw(fieldWidth) << std::left << "Seller ID"
              << " | " << std::setw(valueWidth) << std::left << sellerID << " |" << std::endl;
    std::cout << "| " << std::setw(fieldWidth) << std::left << "Buyer ID"
              << " | " << std::setw(valueWidth) << std::left << buyerID << " |" << std::endl;
    std::cout << "| " << std::setw(fieldWidth) << std::left << "Purchase Date"
              << " | " << std::setw(valueWidth) << std::left << purchaseDate << " |" << std::endl;

    std::cout << "| " << std::setw(fieldWidth) << std::left << "Price"
              << " | " << std::setw(valueWidth) << std::fixed << std::setprecision(2) << price << " |" << std::endl;

    std::cout << "| " << std::setw(fieldWidth) << std::left << "Seat"
              << " | " << std::setw(valueWidth) << std::left << seat << " |" << std::endl;
    std::cout << "| " << std::setw(fieldWidth) << std::left << "Validation Code"
              << " | " << std::setw(valueWidth) << std::left << validationCode << " |" << std::endl;

    std::cout << separator << std::endl;
}





std::string Ticket::generateValidationCode(int length) {
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string code;

    // Use a random device to seed the generator
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<> dist(0, chars.size() - 1);

    for (int i = 0; i < length; ++i) {
        code += chars[dist(gen)];
    }

    return code;
}

// Function to generate a random seat in the format "42A"
std::string Ticket::generateRandomSeat() {
    int seatNumber = rand() % 100; // Random number 0-99
    char seatLetter = 'A' + (rand() % 26); // Random letter A-Z
    std::ostringstream seat;
    seat << seatNumber << seatLetter;
    return seat.str();
}