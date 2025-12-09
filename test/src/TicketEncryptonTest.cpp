#include <iostream>
#include <cassert>
#include "TicketEncryption.h"  // Your cryptographic library header file
#include "Ticket.h"  // Assuming a Ticket structure is defined here

// Unit test for encryption and decryption
void TestSerialization(Ticket ticket) {
    std::string serializedTicket = ticket.serialize();
    // Deserialize the ticket
    Ticket deserializedTicket;
    Ticket::deserialize(serializedTicket, deserializedTicket);
    // Check if the deserialized ticket is the same as the original ticket
    assert(ticket.getTicketID() == deserializedTicket.getTicketID());
    assert(ticket.getEventID() == deserializedTicket.getEventID());
    assert(ticket.getSellerID() == deserializedTicket.getSellerID());
    assert(ticket.getBuyerID() == deserializedTicket.getBuyerID());
    assert(ticket.getPurchaseDate() == deserializedTicket.getPurchaseDate());
    assert(ticket.getPrice() == deserializedTicket.getPrice());
    assert(ticket.getSeat() == deserializedTicket.getSeat());
    assert(ticket.getValidationCode() == deserializedTicket.getValidationCode());
}

void TestSymetricEncryption(Ticket ticket) {
    std::string key = "this_is_a_key";
    std::string encryptedTicket = TicketEncryption::protectTicket(ticket, key);

    std::cout << "\n\nEncrypted Ticket: " << encryptedTicket << std::endl;

    // Check if the ticket is encrypted
    assert(TicketEncryption::isTicketProtected(encryptedTicket));

    // Unprotect the ticket and return the decrypted Ticket
    Ticket decryptedTicket = TicketEncryption::unprotectTicket(encryptedTicket, key);

    // print decrypted ticket
    decryptedTicket.printTicket();

    // Check if the decrypted ticket is the same as the original ticket
    assert(ticket.getTicketID() == decryptedTicket.getTicketID());
    assert(ticket.getEventID() == decryptedTicket.getEventID());
    assert(ticket.getSellerID() == decryptedTicket.getSellerID());
    assert(ticket.getBuyerID() == decryptedTicket.getBuyerID());
    assert(ticket.getPurchaseDate() == decryptedTicket.getPurchaseDate());
    assert(ticket.getPrice() == decryptedTicket.getPrice());
    assert(ticket.getSeat() == decryptedTicket.getSeat());
    assert(ticket.getValidationCode() == decryptedTicket.getValidationCode());

    std::cout << "Decrypted Ticket: ";
    decryptedTicket.printTicket();
    printf("Encryption and Decryption successful!\n");
}

int main() { // Deixei aqui os teus shared pointers boo <3
    //std::shared_ptr<User> seller = std::make_shared<User>(1, "Alice");
    //std::shared_ptr<User> buyer = std::make_shared<User>(2, "Bob");
    Ticket ticket(1, 101, 1, 2, 50.0, "A1", "VALID-1-123");
    TestSerialization(ticket);
    TestSymetricEncryption(ticket);
    return 0;
}