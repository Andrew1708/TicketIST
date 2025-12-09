#include <iostream>
#include <optional>
#include "Repository.h"
#include "UserRepository.h"
#include "TicketRepository.h"
#include "User.h"
#include "TLSServer.h"

#include <mysql_driver.h>
#include <mysql_connection.h>

int PORT = 8080;
std::string CERT_FILE = "server.crt";
std::string KEY_FILE = "server.key";
std::string SERVER_IP = "127.0.0.1";
std::string DB_IP = "172.22.96.1";

// Function to test database connection and check if the users table has at least one entry
bool databaseConnection() {
    try {

        sql::mysql::MySQL_Driver *driver;
        sql::Connection *conn;
        std::string connectionString = "tcp://" + DB_IP + ":3306";

        driver = sql::mysql::get_mysql_driver_instance();
        conn = driver->connect(connectionString, "root", "password");
        conn->setSchema("ticketistdb"); 

        Repository repository(conn);

        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        return false;
    }

}


int main() {
    std::cout << "Opening Server..." << std::endl;
    // Test if the database is running and the users table has at least one entry
    if(!databaseConnection()) {
        std::cerr << "Could not connect to the database. Server not started." << std::endl;
        exit(EXIT_FAILURE);
    }

    /*Repositories tests here, delete later*/
    UserRepository userRepository;
    //-> Create user test
    User user(7, "Mirtilo", "password"); //já n é preciso
    userRepository.createUser("Mirtilo", "password");


    //-> Get user by Id test
    std::optional<User> userOpt = userRepository.getUserById(6);
    if (userOpt.has_value()) {
        User user = userOpt.value();
        std::cout << user.getUsername() << std::endl;
    } 

    //-> Get all users test
    std::vector<User> users = userRepository.getAllUsers();
    for (const auto &user : users) { std::cout << "User ID: " << user.getId() << ", Username: " << user.getUsername() << std::endl; }

    TicketRepository ticketRepository;
    //-> Create ticket test
    Ticket ticket(6, 1, 1, 2, 1.99, "A2", "val5");
    //ticketRepository.createTicket(ticket);

    //-> Delete ticket test
    //ticketRepository.deleteTicket(6);

    //-> Change ticket owner test
    //ticketRepository.changeTicketOwner(2, 3, 1);

    //-> Get all tickets from an event
    //std::vector<Ticket> tickets = ticketRepository.getAllTicketsFromEventId(1);
    //for (const Ticket& ticketi : tickets) { std::cout << "Ticket ID: " << ticketi.getTicketID() << std::endl; }

    //-> Get all tickets from a user
    //std::vector<Ticket> tickets = ticketRepository.getAllTicketsFromUserId(2);
    //for (const Ticket& ticketi : tickets) { std::cout << "Ticket ID: " << ticketi.getTicketID() << std::endl; }
}