#include "TicketInfo.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

TicketInfo::TicketInfo(int ticketID, int eventID, int sellerID, double price, const std::string& seat)
    : ticketID(ticketID), eventID(eventID), sellerID(sellerID), price(price), seat(seat) {}


int TicketInfo::getTicketID() const { return ticketID; }
int TicketInfo::getEventID() const { return eventID; }
int TicketInfo::getSellerID() const { return sellerID; }
double TicketInfo::getPrice() const { return price; }
std::string TicketInfo::getSeat() const { return seat; }

void TicketInfo::displayTicketInfo() const {
    std::cout << "Ticket ID: " << ticketID << " | ";
    std::cout << "Event ID: " << eventID << " | ";
    std::cout << "Seller ID: " << sellerID << " | ";
    std::cout << "Price: " << price << " | ";
    std::cout << "Seat: " << seat << std::endl;
}

std::string TicketInfo::toString() const {
    std::ostringstream oss;
    oss << std::setw(10) << ticketID << " | "
        << std::setw(10) << eventID << " | "
        << std::setw(10) << sellerID << " | "
        << std::setw(10) << std::fixed << std::setprecision(2) << price << "€ | "
        << std::setw(5) << seat;
    return oss.str();
}