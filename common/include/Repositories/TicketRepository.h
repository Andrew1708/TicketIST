#ifndef TICKETREPOSITORY_H
#define TICKETREPOSITORY_H

#include <string>
#include <vector>
#include <optional>
#include "Repository.h"
#include "Ticket.h"
#include "TicketInfo.h"
#include "User.h"

class TicketRepository: public Repository {
    public:
        TicketRepository();
        void resetTickets();
        void deleteAllTickets();
        void deleteTicketByValidationCode(const std::string& validationCode);
        std::vector<TicketInfo>  getAllTicketsFromEventId(const int eventID);
        std::optional<TicketInfo> getTicketInfoById(const int ticketID);
        bool addNewTicket(std::string hash_val_code, std::string hash_ticket, std::string info, int user_id);
        bool makeTicketUnavailable(const int ticketID);
        bool existsTicketWithValidationCode(const std::string& validationCode, std::string& info, bool& in_use);
        void setTicketInUse(const std::string& validationCode);
        std::optional<std::string> getTicketHashByValidationCode(const std::string& validationCode);
        int getNrOfTicketsOwned(int userId);
        bool addTicketInfo(const TicketInfo& ticketInfo);
        bool addTicketToResell(const std::string& hash_ticket, int ticketID);
        bool isTicketInResell(int ticketID);
        std::string getOldTicketValCodeHash(int ticketID);
        void removeTicketFromResell(int ticketID);
        void removeTicket(const std::string& hash_val_code);

    
    private: 
        std::map<int, Ticket> ticketsMap;
};

#endif // TICKETREPOSITORY_H