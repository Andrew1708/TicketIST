#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "Message.h"
#include <unistd.h>
#include "SignUpMessage.h"
#include "LogInMessage.h"
#include "AcceptMessage.h"
#include "RejectMessage.h"
#include "TLSClient.h"
#include "GetAvailableTicketsMessage.h"
#include "AcceptAuthMessage.h"
#include "RejectAuthMessage.h"
#include "AuthenticatedMessage.h"
#include "BuyTicketMessage.h"
#include "CryptographicLibrary.h"
#include "KeyGenerator.h"
#include "Ticket.h"
#include "LogOutMessage.h"
#include "SellTicketMessage.h"
#include <limits>
#include <memory>
#include <csignal>

//----------------------------------//
// GLOBAL VARIABLES
//----------------------------------//
TLSClient* clientSocket = nullptr;

int SERVER_PORT = 8080;
std::string SERVER_IP = "127.0.0.1";
std::string CERT_FILE = "server.crt";

std::string username = "";
std::string cookie = "";
long nonce = 0;

std::string hashedPassword = "";


//----------------------------------//
// AUTHENTICATION FUNCTIONS
//----------------------------------//

std::string generateSalt(const std::string& input) {
    // Use a hash function (e.g., std::hash) to create a deterministic value
    std::hash<std::string> hasher;
    size_t hashValue = hasher(input);
    
    // Convert the hash value to a hexadecimal string
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hashValue;
    
    // Truncate or pad the salt to a fixed size if necessary
    std::string salt = ss.str().substr(0, 16); // Example: 16-byte salt
    return salt;
}

std::string saltPassword(const std::string& password, const std::string& salt) {
    return password + salt;
}

bool isAuthenticationCorrect(AuthenticatedMessage& message) {
    if (username == message.getUsername() && cookie == message.getSessionCookies() && nonce == message.getNonce()) {
        return true;
    }
    return false;
}


//----------------------------------//
// NETWORK 
//----------------------------------//

TLSClient createClient() {
    TLSClient clientSocket(SERVER_IP, SERVER_PORT, CERT_FILE);
    try
    {
        clientSocket.connectToServer();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(EXIT_FAILURE);
    }
    return clientSocket;
}

ActionType getActionType(const std::string& received_message) {
        // Get action type from the message
    size_t pos = received_message.find(':');
    if (pos == std::string::npos) {
        return ActionType::REJECT;
    }

    int action_int = std::stoi(received_message.substr(0, pos));
    ActionType action = static_cast<ActionType>(action_int);
    return action;
}

ActionType getAndPrintAuthResponse(const std::string& received_message) {
    // Get action type from the message
    try{
    ActionType acceptRejectAction = getActionType(received_message);
    
    if (acceptRejectAction == ActionType::REJECT) {
        RejectMessage m = RejectMessage::deserialize(received_message);
        std::cout << m.getMessage() << std::endl;

    } else if (acceptRejectAction == ActionType::ACCEPT_AUTH){
        AcceptAuthMessage m = AcceptAuthMessage::deserialize(received_message);
        std::cout << m.getMessage() << std::endl;
        username = m.getUsername();
        cookie = m.getSessionCookies();
        nonce = m.getNonce();

    } 
    return acceptRejectAction;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return ActionType::UNKNOWN;
    }
}

ActionType getAndPrintServerResponse(std::string received_message) {
    ActionType action = getActionType(received_message);

    // Only accept authenticated messages
    if (action == ActionType::ACCEPT_AUTH || action == ActionType::REJECT_AUTH) {
        AuthenticatedMessage m = AuthenticatedMessage::deserialize(received_message);

        // Check if the message is authenticated correctly
        if (isAuthenticationCorrect(m)) {
            
            // Print the message
            if (action == ActionType::ACCEPT_AUTH) {
                AcceptAuthMessage m = AcceptAuthMessage::deserialize(received_message);
                std::cout << m.getMessage() << std::endl;
            } else {
                RejectAuthMessage m = RejectAuthMessage::deserialize(received_message);
                std::cout << m.getMessage() << std::endl;
            }

            return action;
        }   
    }

    if (action == ActionType::REJECT){
        RejectMessage m = RejectMessage::deserialize(received_message);
        std::cout << m.getMessage() << std::endl;
        return action;
    }

    // Either the message is not authenticated or it is not correctly authenticated for this user
    std::cout << "Invalid message received. Ignored!" << std::endl;
    return ActionType::UNKNOWN;
}


//----------------------------------//
// KEY MANAGEMENT FUNCTIONS
//----------------------------------//

bool symmetricKeyExists() {
    std::string directory = "clients/" + username;
    std::string filePath = directory + "/SymKey.key";
    return std::filesystem::exists(filePath);
}

void generateSymetricKey() {
    // Construct the directory and file path
    std::string directory = "clients/" + username;
    std::string filePath = directory + "/SymKey.key";
    
    // Check if the file already exists
    if (std::filesystem::exists(filePath)) {
        return;
    }

    std::string key;
    if (!KeyGenerator::generateKey(key)) {
        std::cerr << "Error generating symmetric key." << std::endl;
        return;
    }


    // Create the directory if it doesn't exist
    std::filesystem::create_directories(directory);
    
    // Save the key to the file
    std::ofstream keyFile(filePath);
    if (!keyFile) {
        std::cerr << "Error creating key file: " << filePath << std::endl;
        return;
    }
    keyFile << key;
    keyFile.close();
    
}

std::string generateKDFKeyToSaveTicket(){
    std::string password;
    std::string salt;
    while(true){
        std::cout << "Enter password: ";
        std::cin >> password;
        salt = generateSalt(username);
        std::string salted_pwd = saltPassword(password, salt);
        std::string hashed_pwd = CryptographicLibrary::computeSHA256(salted_pwd);
        if (hashed_pwd == hashedPassword){
            break;
        } else {
            char choice;
            std::cout << "=========================================\n";
            std::cout << "Password do not match with account.\n";
            std::cout << "You can still save the ticket with a new password.\n";
            std::cout << "Do you want to use another password to save the ticket? (Y/N): ";
            std::cin >> choice;
            if (choice == 'Y' || choice == 'y'){
                break;
            }
            else if (choice == 'N' || choice == 'n'){
                std::cout << "Please enter the correct password to save the ticket" << std::endl;
            }
            else{
                std::cout << "Invalid input." << std::endl;
            }
        }

    }
    
    std::string salted_pwd = saltPassword(password, salt);
    std::string key = KeyGenerator::keyDerivationFunction(salted_pwd, 10000, 32);
    return key;
}

std::string generateKDFKeyToDecryptTicket(){
    std::string password;
    while(true){
        std::cout << "Please provide the password used to save the ticket: ";
        std::cin >> password;
        if (password.empty()) {
            std::cout << "Password cannot be empty. Please enter again." << std::endl;
            continue;
        }
        break;
    }

    std::string salted_pwd = saltPassword(password, generateSalt(username));
    return KeyGenerator::keyDerivationFunction(salted_pwd, 10000, 32);
}


//----------------------------------//
// TICKET MANAGEMENT FUNCTIONS
//----------------------------------//

void saveTicket(const std::string& received_message) {
    try {
        std::cout << "Saving ticket..." << std::endl;
        // Deserialize the received message
        AcceptAuthMessage m = AcceptAuthMessage::deserialize(received_message);

        // Get the username and ticket details
        std::string ticketData = m.getMessage();

        // Create the folder path: "clients/${username}"
        std::string folderPath = "clients/" + username + "/";

        // Check if the directory exists
        if (!std::filesystem::exists(folderPath)) {
            // Create the directory if it does not exist
            if (!std::filesystem::create_directories(folderPath)) {
                throw std::runtime_error("Failed to create directory: " + folderPath);
            }
        }
        
        // Generate the symmetric key if it does not exist
        if (!symmetricKeyExists()){
           throw std::runtime_error("Something went wrong. Please log in again.");
        }

        // Read the symmetric key from the file
        std::string symetric_key_path = "clients/" + username + "/SymKey.key";
        std::string sym_key = KeyGenerator::readKey(symetric_key_path);

        // Generate the KDF key to decrypt the symmetric key
        std::string kdf_key = generateKDFKeyToSaveTicket();

        std::string encrypted_ticket = CryptographicLibrary::protectSymetric(ticketData, sym_key);

        // Encrypt the ticket data
        std::string double_encrypted_ticket = CryptographicLibrary::protectSymetric(encrypted_ticket, kdf_key);
        
        // Open the file and write the ticket data

        // Ask user for file name 
        std::string file_name;
        while (true) {
            std::cout << "Enter the name of the ticket file: ";
            std::cin >> file_name;

            // Check if the file name is valid
            if (file_name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") == std::string::npos) {
            std::string filePath = folderPath + file_name + ".txt";
                if (!std::filesystem::exists(filePath)) {
                    break;
                } else {
                    std::cout << "File already exists. Please choose a different name." << std::endl;
                }
            } else {
            std::cout << "Invalid file name. Please use only alphanumeric characters, underscores, or hyphens." << std::endl;
            }
        }

        // Define the file path: "clients/${username}/name.txt"
        std::string filePath = folderPath + file_name + ".txt";
        std::ofstream ticketFile(filePath);
        if (!ticketFile.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + filePath);
        }

        ticketFile << double_encrypted_ticket; // Write the serialized ticket data
        ticketFile.close();

        std::cout << "Ticket saved to: " << filePath << std::endl;

    } catch (const std::exception& e) {

        std::cerr << "Error saving ticket: " << e.what() << std::endl;
    }
}

void getOwnedTickets() {
    std::string folderPath = "clients/" + username + "/";
    std::vector<std::string> ticketNames;

    try {
        // Check if the folder exists
        if (!std::filesystem::exists(folderPath)) {
            std::cerr << "Application Data Not Found" << std::endl;
            return;
        }

        // Iterate over files in the folder
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                std::string fileName = entry.path().filename().string();

                // Include only .txt files, exclude .key files
                if (fileName.find(".txt") != std::string::npos) {
                    ticketNames.push_back(fileName.substr(0, fileName.find_last_of('.')));
                }
            }
        }

        // Display the tickets in a pretty format
        std::cout << "You have " << ticketNames.size() << " ticket(s):" << std::endl;
        for (const auto& ticket : ticketNames) {
            std::cout << " - " << ticket << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

std::string getAndDecryptTicket(std::string file_path){
     //Read encrypted ticket data
    std::string ticketDataEncrypted;

    std::ifstream ticketFile(file_path, std::ios::in | std::ios::binary); // Open in binary mode for raw content
    if (!ticketFile.is_open()) {
        std::cerr << "Error: Could not open file: " << file_path << std::endl;
    }

    std::ostringstream buffer;
    buffer << ticketFile.rdbuf(); // Read the entire file into the buffer
    ticketFile.close();
    ticketDataEncrypted = buffer.str();

    // Read the symmetric key from the file
    std::string symetric_key_path = "clients/" + username + "/SymKey.key";
    std::string sym_key = KeyGenerator::readKey(symetric_key_path);

    // Generate the KDF key to decrypt the symmetric key
    std::string kdf_key = generateKDFKeyToDecryptTicket();
    std::string ticket_half_decrypted = CryptographicLibrary::unprotectSymetric(ticketDataEncrypted, kdf_key);

    // Decrypt the ticket data
    return CryptographicLibrary::unprotectSymetric(ticket_half_decrypted, sym_key);
}

std::string askFileName(){
    std::string filePath;
    while(true){
        std::cout << "Enter the name of the ticket file: ";
        std::string ticket_name;
        std::cin >> ticket_name;

        std::string folderPath = "clients/" + username + "/";
        filePath = folderPath + ticket_name + ".txt";

        if (!std::filesystem::exists(filePath)) {
            std::cout << "Ticket does not exist. Please enter a valid ticket name." << std::endl;
            getOwnedTickets();
            continue;
        }

        break;
    }
    return filePath;
}

void getTicketValCode(){
    std::string filePath = askFileName();

    try{
       std::string ticketData = getAndDecryptTicket(filePath);
       // Deserialize the ticket data
        Ticket ticket;
        if (!Ticket::deserialize(ticketData, ticket)) {
            throw std::runtime_error("Failed to deserialize ticket data.");
        }

        // Print the validation code
        std::cout << "Validation Code: " << ticket.getValidationCode() << std::endl;


    } catch (const std::exception& e) {
        std::cout << "Error: Password is incorrect. Please try again." << std::endl;
    }
}

void getIntegrityCode(){
    std::string filePath = askFileName();

    try{
        std::string ticketData = getAndDecryptTicket(filePath);
        std::string hash_ticket = CryptographicLibrary::computeSHA256(ticketData);

        std::cout << "Computing Integrity Code..." << std::endl;
        std::string firstFive = hash_ticket.substr(0, 5); // First 5 characters
        std::string lastFive = hash_ticket.substr(hash_ticket.size() - 5); // Last 5 characters
        std::string integrity_code = firstFive + lastFive;

    
        // Deserialize the ticket data
        Ticket ticket;
        if (!Ticket::deserialize(ticketData, ticket)) {
            throw std::runtime_error("Failed to deserialize ticket data.");
        }

        // Print val code and integrity code
        std::cout << "Validation Code: " << ticket.getValidationCode() << std::endl;
        std::cout << "Integrity Code: " << integrity_code << std::endl;

    } catch (const std::exception& e) {
        std::cout << "Error: Password is incorrect. Please try again." << std::endl;
    }

}

void getTicket(){
    std::string filePath = askFileName();

    try{
        std::string ticketData = getAndDecryptTicket(filePath);
        Ticket ticket;
        if (!Ticket::deserialize(ticketData, ticket)) {
            throw std::runtime_error("Failed to deserialize ticket data.");
        }

        ticket.printTicket();

    } catch (const std::exception& e) {
        std::cout << "Error: Password is incorrect. Please try again." << std::endl;
    }
}

//----------------------------------//
// DISPLAYS MENUS AND ASKS FOR INPUT
//----------------------------------//

void displayClientMenu() {
    std::cout << "\nMENU \n"
            << "1.GET AVAILABLE TICKETS \n"
            << "2.BUY TICKET \n"
            << "3.GET ALL OWNED TICKETS \n"
            << "4.SEE TICKET \n"
            << "5.GET TICKET VAL CODE \n"
            << "6.GET TICKET INTEGRITY CODE \n"
            << "7.SELL TICKET \n"
            << "8.LOG OUT \n"
            << "Please enter the number corresponding to your choice: ";
}

void askForEventName(std::string& event_name) {
    std::cout << "Enter event name: ";
    std::cin >> event_name;

    while (event_name.empty()) {
        std::cout << "Event name cannot be empty. Please enter again." << std::endl;
        std::cout << "Enter event name: ";
        std::cin >> event_name;
    }
}

void askForTicketId(int& ticketId) {
    std::cout << "Enter ticket ID: ";
    std::cin >> ticketId;

    while (ticketId < 0) {
        std::cout << "Ticket ID cannot be negative. Please enter again." << std::endl;
        std::cout << "Enter ticket ID: ";
        std::cin >> ticketId;
    }
}

void displayUnauthenticatedMenu() {
    std::cout << "1.SIGN UP \n"
        << "2.LOG IN\n"
        << "Please enter the number corresponding to your choice: ";
}

void askForCredentials(std::string& username, std::string& password) {
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    while (username.empty() || password.empty()) {
        std::cout << "Username and password cannot be empty. Please enter again." << std::endl;
        std::cout << "Enter username: ";
        std::cin >> username;
        std::cout << "Enter password: ";
        std::cin >> password;
    }
}



//dev only
void authentication_bypass(TLSClient& clientSocket) {
    auto username = "lenis";
    auto password = "morg";
    auto salted_pwd = saltPassword(password, generateSalt(username));
    auto message = std::make_unique<LogInMessage>(username, salted_pwd);
    clientSocket.sendData(message->serialize());
    std::string received_message = clientSocket.receiveData();
    ActionType acceptRejectAction = getAndPrintAuthResponse(received_message);
    hashedPassword = CryptographicLibrary::computeSHA256(salted_pwd);
}

void logout(TLSClient& clientSocket) {
    auto message = std::make_unique<LogOutMessage>(username, cookie, ++nonce);
    clientSocket.sendData(message->serialize());
    std::string received_message = clientSocket.receiveData();
    ActionType acceptRejectAction = getActionType(received_message);
    if (acceptRejectAction == ActionType::ACCEPT) {
        std::cout << "Logged out successfully." << std::endl;
        clientSocket.~TLSClient();
    }

    exit(EXIT_SUCCESS);
}

// Signal handler for SIGINT (Ctrl+C)
void signalHandler(int signal) {
    if (signal == SIGINT) {
        logout(*clientSocket);
    }
}

//----------------------------------//
// MAIN MENU AND APPLICATION FLOW
//----------------------------------//
bool authenticate(TLSClient& clientSocket) {
    // Create a message and send it to the server
    std::string username, password;

    while (true) {
        displayUnauthenticatedMenu();

        int action;
        if (!(std::cin >> action)) {
            std::cout << "Invalid input. Please select a number!" << std::endl;
            std::cin.clear(); // Reset the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            continue;
        }

        std::unique_ptr<Message> message;


        switch (action) {
            case 1:{// SIGN_UP
                std::cout << "SIGN UP" << std::endl;
                askForCredentials(username, password);
                std::string salted_pwd = saltPassword(password, generateSalt(username));
                hashedPassword = CryptographicLibrary::computeSHA256(salted_pwd);
                message = std::make_unique<SignUpMessage>(username, salted_pwd);
                break;
            }

            case 2:{ // LOG_IN
                std::cout << "LOG IN" << std::endl;
                askForCredentials(username, password);
                std::string salted_pwd = saltPassword(password, generateSalt(username));
                hashedPassword = CryptographicLibrary::computeSHA256(salted_pwd);
                message = std::make_unique<LogInMessage>(username, salted_pwd);
                break;
            }

            default:
                std::cout << "Invalid action. Please choose 1 or 2." << std::endl;
                continue;
        }

        // Send the message if it was created
        if (message) {
            try {
                clientSocket.sendData(message->serialize());

                // Receive and display the response from the server
                std::string received_message = clientSocket.receiveData();

                ActionType acceptRejectAction = getAndPrintAuthResponse(received_message);

                if (acceptRejectAction == ActionType::ACCEPT_AUTH) {
                    generateSymetricKey();
                    return true;
                }

            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
                return EXIT_FAILURE;
            }
        }
    }
    return false;
}

double askForPrice(){
    double price;
    while(true){
        std::cout << "Enter selling price: ";
        std::cin >> price;
        if (price < 0){
            std::cout << "Price cannot be negative. Please enter again." << std::endl;
            continue;
        }
        break;
    }
    return price;
}


void clientMenu(TLSClient& clientSocket) {
    ::clientSocket = &clientSocket;
    signal(SIGINT, signalHandler);

    while (true) {
        displayClientMenu();

        int action;
        if (!(std::cin >> action)) {
            if (std::cin.eof()) {
                std::cin.clear(); // Clear the error flags
                logout(clientSocket);
            }else{
                std::cout << "Invalid input. Please select a number!" << std::endl;
                std::cin.clear(); // Reset the error flag
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                continue;
            }
        }

        std::unique_ptr<Message> message;

        switch (action) {
            case 1:{ // GET_AVAILABLE_TICKETS
                std::cout << "GET AVAILABLE TICKETS" << std::endl;
                std::string event_name;
                askForEventName(event_name);
                message = std::make_unique<GetAvailableTicketsMessage>(event_name, username, cookie, ++nonce);
                break;
                }

            case 2:{ // BUY_TICKET
                std::cout << "BUY TICKET" << std::endl;
                int ticketId;
                askForTicketId(ticketId);
                std::string event_name;
                askForEventName(event_name);
                message = std::make_unique<BuyTicketMessage>(username, cookie, ++nonce, ticketId, event_name);
                break;
            }
            
            case 3: // GET_ALL_OWNED_TICKETS
                std::cout << "GET ALL OWNED TICKETS" << std::endl;
                getOwnedTickets();
                break;
            
            case 4: // SEE_TICKET 
                std::cout << "SEE TICKET" << std::endl;
                getTicket();
                break;
            
            case 5: // GET_TICKET_VAL_CODE
                std::cout << "GET TICKET VAL CODE" << std::endl;
                getTicketValCode();
                break;
            
            case 6: // GET_TICKET_INTEGRITY_CODE
                std::cout << "GET TICKET INTEGRITY CODE" << std::endl;
                getIntegrityCode();
                break;

            case 7:{ // SELL_TICKET 
                std::string filePath = askFileName();
                std::string ticketData = getAndDecryptTicket(filePath);
                double new_price = askForPrice();
                message = std::make_unique<SellTicketMessage>(username, cookie, ++nonce, ticketData, new_price);
                break;
            }
        
            case 8: // LOG_OUT
                logout(clientSocket);

            default:
                std::cout << "Invalid action. Please choose 1, 2, or 3." << std::endl;
                continue;
        }
        
        // Send the message if it was created
        if (message) {
            try {
                clientSocket.sendData(message->serialize());
                std::string received_message = clientSocket.receiveData();

                ActionType action = getActionType(received_message);
                if (message->get_action() == ActionType::BUY_TICKET && action == ActionType::ACCEPT_AUTH) {
                    saveTicket(received_message);
                }
                else{
                    ActionType acceptRejectAction = getAndPrintServerResponse(received_message);
                }

            } catch (const std::exception& e) {
                std::cout<< "Error: " << e.what() << std::endl;
                return;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // Default behavior is authenticate
    bool use_bypass = false;

    // Check for the `-bypass` flag
    if (argc > 1) {
        std::string flag = argv[1];
        if (flag == "-bypass") {
            use_bypass = true;
        } else {
            std::cerr << "Invalid flag. Use '-bypass' for bypass or no flags for authentication." << std::endl;
            return EXIT_FAILURE;
        }
    }

    TLSClient clientSocket = createClient();

    while (true) {
        if (use_bypass) {
            authentication_bypass(clientSocket);
        } else {
            if (!authenticate(clientSocket)) {
                std::cerr << "Something went wrong!" << std::endl;
                return EXIT_FAILURE;
            }
        }

        clientMenu(clientSocket);
    }

    return 0;
}
