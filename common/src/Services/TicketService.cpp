#include "TicketService.h"
#include "TicketInfo.h"
#include "CryptographicLibrary.h"
#include <iostream>

TicketService::TicketService() : ticketRepository() {}

std::vector<TicketInfo> TicketService::getAllAvailableTicketsByEventId(const int eventId) {
    // Fetch tickets from the repository
    std::vector<TicketInfo> tickets = ticketRepository.getAllTicketsFromEventId(eventId);

    return tickets;
}

Ticket TicketService::buyTicket(const int ticketId, const int eventId, int userID) {
    // Check if the ticket is available in DB not cache, because if app is distributed,
    int ticket_owned = ticketRepository.getNrOfTicketsOwned(userID);
    if (ticket_owned >= 6){
        throw std::invalid_argument("You already own 6 tickets. You cannot buy more.");
    }
    // cache may not correspond to the actual state of the DB
    auto db_response = ticketRepository.getTicketInfoById(ticketId);
    if (!db_response.has_value()) {
        // If the ticket is not found, remove it from the cache
        throw std::invalid_argument("Ticket not found. Please revise available tickets.");
    }

    // Create a Ticket object from the TicketInfo
    TicketInfo ticketInfo = db_response.value();

    // Check if the ticket is from the provided event
    if(ticketInfo.getEventID() != eventId){
        throw std::invalid_argument("Ticket not found. Please revise available tickets.");
    }

    std::string val_code = Ticket::generateValidationCode(10);
    Ticket ticket(ticketInfo.getTicketID(), ticketInfo.getEventID(), ticketInfo.getSellerID(), userID, ticketInfo.getPrice(), ticketInfo.getSeat(), val_code);
    
    // Create valcode and ticket hash
    std::string val_code_hash = CryptographicLibrary::computeSHA256(val_code);
    std::string ticket_hash = CryptographicLibrary::computeSHA256(ticket.serialize());


    if (!ticketRepository.makeTicketUnavailable(ticketId)) {
        throw std::invalid_argument("Ticket is no longer available. Please revise available tickets.");
    }

    // Update the ticket in the DB
    if (!ticketRepository.addNewTicket(val_code_hash, ticket_hash, ticket.getSeat(), userID)) {
        throw std::invalid_argument("Something went wrong. Please try again.");
    }

    // Check if is a resold ticket
    if (ticketRepository.isTicketInResell(ticket.getTicketID())) {

        // Get "old" ticket val_code_hash
        std::string old_val_code_hash = ticketRepository.getOldTicketValCodeHash(ticket.getTicketID());

        // Remove the old ticket from the resell table
        ticketRepository.removeTicketFromResell(ticket.getTicketID());

        // Remove the old ticket from the DB of available tickets
        ticketRepository.removeTicket(old_val_code_hash);

    }

    return ticket;
}

bool TicketService::validateTicket(const std::string& validationCode, std::string& info) {
    std::string hash_val_code = CryptographicLibrary::computeSHA256(validationCode);

    // Fetch the ticket from the repository
    bool used;
    if(ticketRepository.existsTicketWithValidationCode(hash_val_code, info, used)){
        // If the ticket is in use, return false
        if (used) {
            info = "Ticket is already in use.";
            return false;
        }
        // If the ticket is not in use, return true
        // Set the ticket in use
        ticketRepository.setTicketInUse(hash_val_code);
        return true;
    }
    info = "Ticket not found. Please revise the validation code.";
    return false;
}

bool TicketService::checkIntegrity(const std::string& validationCode, const std::string& integrityCode, std::string& info) {
    std::string hash_val_code = CryptographicLibrary::computeSHA256(validationCode);
    auto db_response = ticketRepository.getTicketHashByValidationCode(hash_val_code);
    if (!db_response.has_value()) {
        info = "Ticket not found. Please revise the validation code.";
        return false;
    }

    std::string hash_ticket = db_response.value();
    std::string firstFive = hash_ticket.substr(0, 5); // First 5 characters
    std::string lastFive = hash_ticket.substr(hash_ticket.size() - 5); // Last 5 characters
    std::string real_integrity_code = firstFive + lastFive;

    if (real_integrity_code == integrityCode) {
        info = "Integrity check successful.";
        return true;
    }
    else{
        info = "Integrity check failed. Integrity of the ticket has been compromised.";
        return false;
    }
}

bool TicketService::sellTicket(const std::string& ticket, double newPrice) {
    // Deserialize the ticket
    Ticket t;
    if (!Ticket::deserialize(ticket, t)) {
        throw std::invalid_argument("Invalid ticket format.");
    }

    // Get the ticket stored hash from db
    std::string hash_val_code = CryptographicLibrary::computeSHA256(t.getValidationCode());
    auto db_response = ticketRepository.getTicketHashByValidationCode(hash_val_code);
    if (!db_response.has_value()) {
        throw std::invalid_argument("Ticket not found. Please revise the validation code.");
    }


    // Check ticket integrity
    std::string true_hash_ticket = db_response.value();
    std::string received_hash_ticket = CryptographicLibrary::computeSHA256(t.serialize());
    if (true_hash_ticket != received_hash_ticket) {
        throw std::invalid_argument("Ticket integrity compromised. Please revise the ticket.");
    } 

    // Check if new price valid
    if (newPrice < 0 || newPrice > t.getPrice()) {
        throw std::invalid_argument("Invalid price. Please provide a price greater than 0 and the current price.");
    }

    // Add the ticket info to the repository
    TicketInfo ticketInfo(t.getTicketID(), t.getEventID(), t.getBuyerID(), newPrice, t.getSeat());
    if (!ticketRepository.addTicketInfo(ticketInfo)) {
        throw std::invalid_argument("Process Failed");
    }
    ticketsInfoCache[t.getEventID()].insert({t.getTicketID(), ticketInfo});

    // Add ticket to resell table in DB
    if (!ticketRepository.addTicketToResell(hash_val_code, t.getTicketID())) {
        throw std::invalid_argument("Process Failed");
    }
    
    return true;

}