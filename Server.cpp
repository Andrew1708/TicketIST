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
std::string DB_IP = "192.168.1.6";

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

void reset_db() {
    TicketRepository ticketRepository;
    EventRepository eventRepository;
    eventRepository.addDummyEvents();
    ticketRepository.resetTickets();
}

int main() {
    std::cout << "Opening Server... on" << std::endl;
    // Test if the database is running and the users table has at least one entry
    if(!databaseConnection()) {
        std::cerr << "Could not connect to the database. Server not started." << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // APAGAR NO FIM 
    reset_db();

    TLSServer serverSocket(CERT_FILE, KEY_FILE, PORT, SERVER_IP);
    if (!serverSocket.start()) {
        std::cerr << "Could not start the server." << std::endl;
        exit(EXIT_FAILURE);
    }
    serverSocket.run();
    
    return 0;
}
