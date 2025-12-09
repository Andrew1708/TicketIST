#ifndef TICKETSERVICE_H
#define TICKETSERVICE_H

#include <string>
#include <vector>
#include "Ticket.h"
#include "TicketRepository.h"
#include "TicketInfo.h"

class TicketService {
private:
    TicketRepository ticketRepository;
    // <eventID, <ticketID, TicketInfo>>
    std::map<int, std::map<int, TicketInfo>> ticketsInfoCache;
    

public:
    TicketService();
    
    std::vector<TicketInfo> getAllAvailableTicketsByEventId(const int eventID);
    Ticket buyTicket(const int ticketID, const int eventID, int userID);
    bool validateTicket(const std::string& validationCode, std::string& info);
    bool checkIntegrity(const std::string& validationCode, const std::string& integrityCode, std::string& info);
    bool sellTicket(const std::string& ticket, double new_price);



};
#endif // TICKETSERVICE_H