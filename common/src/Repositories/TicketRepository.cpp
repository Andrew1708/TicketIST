#include "TicketRepository.h"
#include "Ticket.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>


TicketRepository::TicketRepository() : Repository() {}

void TicketRepository::deleteAllTickets(){
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "TRUNCATE TABLE ticket_info"
            );
            stmt->execute();
            delete stmt;
            return;

        } else {
            std::cerr << "Database connection is not valid." << std::endl;
            return;
        }

    } catch (sql::SQLException &e) {
        std::cerr << "Error deleting tickets: " << e.what() << std::endl;
        return;
    }
}

void TicketRepository::resetTickets(){
    deleteAllTickets();
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "INSERT INTO ticket_info (eventID, sellerID, price, seat) VALUES (?, 0, ?, ?)"
            );

            for (int eventID = 1; eventID <= 4; ++eventID) { // Loop for each event
                for (int i = 0; i < 20; ++i) { // Generate 20 tickets per event
                    double price = (rand() % 10 + 1) * 10; // Random multiple of 10 (10-100)
                    std::string seat = Ticket::generateRandomSeat();
                    
                    stmt->setInt(1, eventID);
                    stmt->setDouble(2, price);
                    stmt->setString(3, seat);
                    stmt->execute();

                }
            }
            delete stmt;
            return;

        } else {
            std::cerr << "Database connection is not valid." << std::endl;
            return;
        }

    } catch (sql::SQLException &e) {
        std::cerr << "Error creating user: " << e.what() << std::endl;
        return;
    }
}

std::vector<TicketInfo> TicketRepository::getAllTicketsFromEventId(const int eventID) {
    std::vector<TicketInfo> tickets;
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "SELECT * FROM ticket_info WHERE eventID = ?"
            );
            stmt->setInt(1, eventID);
            sql::ResultSet *res = stmt->executeQuery();
            while (res->next()) {
                int ticketID = res->getInt("ticketID");
                int sellerID = res->getInt("sellerID");
                double price = res->getDouble("price");
                std::string seat = res->getString("seat");
                TicketInfo ticket(ticketID, eventID, sellerID, price, seat);
                tickets.push_back(ticket);
            }
            delete res;
            delete stmt;
        } else {
            std::cerr << "Database connection is not valid." << std::endl;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error getting tickets: " << e.what() << std::endl;
    }
    return tickets;
}

std::optional<TicketInfo> TicketRepository::getTicketInfoById(int ticket_id) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "SELECT * FROM ticket_info WHERE ticketID = ?"
            );
            stmt->setInt(1, ticket_id);
            sql::ResultSet* res = stmt->executeQuery();

            if (res->next()) {
                int eventID = res->getInt("eventID");
                int sellerID = res->getInt("sellerID");
                double price = res->getDouble("price");
                std::string seat = res->getString("seat");

                delete res;
                delete stmt;

                return TicketInfo(ticket_id, eventID, sellerID, price, seat);
            }

            delete res;
            delete stmt;
            return std::nullopt; // No ticket found
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error getting ticket: " << e.what() << std::endl;
    }
    return std::nullopt;
}


bool TicketRepository::addNewTicket(std::string hash_val_code, std::string hash_ticket, std::string info, int user_id) {
    try {
        if(conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "INSERT INTO ticket (validation_code, ticket_hash, info, owner_id) VALUES (?, ?, ?, ?)"
            );
            stmt->setString(1, hash_val_code);
            stmt->setString(2, hash_ticket);
            stmt->setString(3, info);
            stmt->setInt(4, user_id);
            stmt->execute();
            delete stmt;
            return true;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error adding ticket: " << e.what() << std::endl;
    }
    return false;
}

bool TicketRepository::makeTicketUnavailable(int ticketID) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "DELETE FROM ticket_info WHERE ticketID = ?"
            );
            stmt->setInt(1, ticketID);
            stmt->execute();
            delete stmt;
            return true;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error making ticket unavailable: " << e.what() << std::endl;
    }
    return false;
}

bool TicketRepository::existsTicketWithValidationCode(const std::string& validationCode, std::string& info, bool& used) {
    try {
        if (conn && conn->isValid()) {
            // Prepare the SQL statement
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "SELECT info, used FROM ticket WHERE validation_code = ?"
            );
            stmt->setString(1, validationCode);

            // Execute the query
            sql::ResultSet* res = stmt->executeQuery();
            if (res->next()) {
                // Retrieve the 'info' column and save it
                info = res->getString("info");
                used = res->getBoolean("used");
                delete res;
                delete stmt;
                return true;
            }

            // Clean up resources if no ticket was found
            delete res;
            delete stmt;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error checking ticket existence: " << e.what() << std::endl;
    }

    // Return false if the ticket does not exist or an error occurs
    return false;
}

void TicketRepository::deleteTicketByValidationCode(const std::string& validationCode) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "DELETE FROM ticket WHERE validation_code = ?"
            );
            stmt->setString(1, validationCode);
            stmt->execute();
            delete stmt;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error deleting ticket: " << e.what() << std::endl;
    }
}

void TicketRepository::setTicketInUse(const std::string& validationCode) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "UPDATE ticket SET used = 1 WHERE validation_code = ?"
            );
            stmt->setString(1, validationCode);
            stmt->execute();
            delete stmt;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error setting ticket in use: " << e.what() << std::endl;
    }
}

std::optional<std::string> TicketRepository::getTicketHashByValidationCode(const std::string& validationCode) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "SELECT ticket_hash FROM ticket WHERE validation_code = ?"
            );
            stmt->setString(1, validationCode);
            sql::ResultSet* res = stmt->executeQuery();
            if (res->next()) {
                std::string ticketHash = res->getString("ticket_hash");
                delete res;
                delete stmt;
                return ticketHash;
            }
            delete res;
            delete stmt;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error getting ticket hash: " << e.what() << std::endl;
    }
    return std::nullopt;
}

int TicketRepository::getNrOfTicketsOwned(int userId){
    int count = 0;
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "SELECT COUNT(*) FROM ticket WHERE owner_id = ?"
            );
            stmt->setInt(1, userId);
            sql::ResultSet* res = stmt->executeQuery();
            if (res->next()) {
                count = res->getInt(1);
            }
            delete res;
            delete stmt;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error getting ticket count: " << e.what() << std::endl;
    }
    return count;
}

bool TicketRepository::addTicketInfo(const TicketInfo& ticketInfo) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "INSERT INTO ticket_info (ticketID, eventID, sellerID, price, seat) VALUES (?, ?, ?, ?, ?)"
            );
            stmt->setInt(1, ticketInfo.getTicketID());
            stmt->setInt(2, ticketInfo.getEventID());
            stmt->setInt(3, ticketInfo.getSellerID());
            stmt->setDouble(4, ticketInfo.getPrice());
            stmt->setString(5, ticketInfo.getSeat());
            stmt->execute();
            delete stmt;
            return true;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error adding ticket info: " << e.what() << std::endl;
    }
    return false;
}

bool TicketRepository::addTicketToResell(const std::string& hash_ticket, int ticketID) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "INSERT INTO resell_tickets (hash_val_code, ticket_Id) VALUES (?, ?)"
            );
            stmt->setString(1, hash_ticket);
            stmt->setInt(2, ticketID);
            stmt->execute();
            delete stmt;
            return true;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error adding ticket to resell: " << e.what() << std::endl;
    }
    return false;
}

bool TicketRepository::isTicketInResell(int ticketID) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "SELECT * FROM resell_tickets WHERE ticket_Id = ?"
            );
            stmt->setInt(1, ticketID);
            sql::ResultSet* res = stmt->executeQuery();
            if (res->next()) {
                delete res;
                delete stmt;
                return true;
            }
            delete res;
            delete stmt;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error checking if ticket is in resell: " << e.what() << std::endl;
    }
    return false;
}

std::string TicketRepository::getOldTicketValCodeHash(int ticketID) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "SELECT hash_val_code FROM resell_tickets WHERE ticket_Id = ?"
            );
            stmt->setInt(1, ticketID);
            sql::ResultSet* res = stmt->executeQuery();
            if (res->next()) {
                std::string hash_val_code = res->getString("hash_val_code");
                delete res;
                delete stmt;
                return hash_val_code;
            }
            delete res;
            delete stmt;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error getting old ticket val code hash: " << e.what() << std::endl;
    }
    return "";
}

void TicketRepository::removeTicketFromResell(int ticketID) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "DELETE FROM resell_tickets WHERE ticket_Id = ?"
            );
            stmt->setInt(1, ticketID);
            stmt->execute();
            delete stmt;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error removing ticket from resell: " << e.what() << std::endl;
    }
}

void TicketRepository::removeTicket(const std::string& hash_val_code) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement* stmt = conn->prepareStatement(
                "DELETE FROM ticket WHERE validation_code = ?"
            );
            stmt->setString(1, hash_val_code);
            stmt->execute();
            delete stmt;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "Error removing ticket: " << e.what() << std::endl;
    }
}