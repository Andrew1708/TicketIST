#include "TicketEncryption.h"
#include <sstream>
#include <iostream>

// Protects a ticket by serializing and encrypting it, then returns the encrypted ticket as a string
std::string TicketEncryption::protectTicket(const Ticket& ticket, const std::string& key) {
    // Serialize the ticket
    std::string serializedTicket = ticket.serialize();
    // Encrypt the serialized ticket
    return CryptographicLibrary::protectSymetric(serializedTicket, key);
}

// Checks if the ticket is encrypted (verifies if it's protected)
bool TicketEncryption::isTicketProtected(const std::string& encryptedTicket) {
    return CryptographicLibrary::check(encryptedTicket);
}

// Unprotects a ticket by decrypting the encrypted ticket and returning the ticket object
Ticket TicketEncryption::unprotectTicket(const std::string& encryptedTicket, const std::string& key) {
    std::string serializedTicket;
    // Decrypt the encrypted ticket
    serializedTicket = CryptographicLibrary::unprotectSymetric(encryptedTicket, key);
    // Create and return a ticket by deserializing the decrypted string
    Ticket ticket;
    ticket.deserialize(serializedTicket, ticket);
    return ticket;
}