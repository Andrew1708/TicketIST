#ifndef TICKET_ENCRYPTION_H
#define TICKET_ENCRYPTION_H

#include <string>
#include "Ticket.h"
#include "CryptographicLibrary.h"

class TicketEncryption {
public:
    // Encrypts a ticket, serializes it, and then protects it with the given key
    static std::string protectTicket(const Ticket& ticket, const std::string& key);

    // Checks if the ticket is encrypted (does not decrypt, just verifies if it's encrypted)
    static bool isTicketProtected(const std::string& encryptedTicket);

    // Removes security (decrypts the ticket and returns the ticket object)
    static Ticket unprotectTicket(const std::string& encryptedTicket, const std::string& key);    
private:

};

#endif // TICKET_ENCRYPTION_H
