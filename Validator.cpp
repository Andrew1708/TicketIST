#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <csignal>
#include <memory>
#include "TLSClient.h"
#include "AuthenticatedMessage.h"
#include "LogInValidatorMessage.h"
#include "AcceptAuthMessage.h"
#include "RejectMessage.h"
#include "CryptographicLibrary.h"
#include "ValidateTicketMessage.h"
#include "RejectAuthMessage.h"
#include "CheckIntegrityMessage.h"
#include "LogOutMessage.h"

TLSClient* clientSocket = nullptr;

int SERVER_PORT = 8080;
std::string SERVER_IP = "127.0.0.1";
std::string CERT_FILE = "server.crt";

std::string username = "";
std::string cookie = "";
long nonce = 0;

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


TLSClient createValidator() {
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
        std::cout << "Invalid message format." << std::endl;
        return ActionType::REJECT;
    }

    int action_int = std::stoi(received_message.substr(0, pos));
    ActionType action = static_cast<ActionType>(action_int);
    return action;
}

bool authenticate(TLSClient& clientSocket) {
    std::string username;
    std::string password;
    std::string hashedPassword;
    while(true) {
        std::cout << "Please enter your username and password." << std::endl;
        std::cout << "Username: ";
        std::cin >> username;
        std::cout << "Password: ";
        std::cin >> password;

        if (username.empty() || password.empty()) {
            std::cerr << "Username and password cannot be empty." << std::endl;
            continue;
        }
        break;
    }

    // Create Log In Message
    std::string salted_pwd = saltPassword(password, generateSalt(username));
    LogInValidatorMessage m = LogInValidatorMessage(username, salted_pwd);

    // Send the message
    clientSocket.sendData(m.serialize());

    // Receive response from server, only move on uppon receiving accept authenticated messages
    std::string received_message = clientSocket.receiveData();
    ActionType action = getActionType(received_message);

    // Hack to get the hashed password
    std::string hash_pwd = CryptographicLibrary::computeSHA256(salted_pwd);


    if (action == ActionType::ACCEPT_AUTH) {
        AcceptAuthMessage m = AcceptAuthMessage::deserialize(received_message);
        std::cout << m.getMessage() << std::endl;
            ::username = m.getUsername();
            ::cookie = m.getSessionCookies();
            ::nonce = m.getNonce();
            return true;
    
    } else if (action == ActionType::REJECT) {
        RejectMessage m = RejectMessage::deserialize(received_message);
        std::cout << m.getMessage() << std::endl;
        
    } else {
        std::cerr << "Unexpected message received. Try again" << std::endl;
    }
    authenticate(clientSocket);
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

void validatorMenu(TLSClient& clientSocket){
    ::clientSocket = &clientSocket;
    signal(SIGINT, signalHandler);

    while(true){
        std::cout << "1. Validate Ticket\n2. Check Integrity\n3. Logout\nPlease enter the number corresponding to your choice: ";
        int choice;
        if(!(std::cin >> choice)){
            std::cerr << "Invalid input. Please enter a number." << std::endl;
            std::cin.clear(); // Reset the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            continue;
        }

        switch (choice){
        case 1:{
            // Validate Ticket
            std::cout << "Insert Validation Code: " << std::endl;
            std::string validationCode;
            std::cin >> validationCode;

            // Create message
            ValidateTicketMessage m = ValidateTicketMessage(username, cookie, ++nonce, validationCode);
            clientSocket.sendData(m.serialize());
            break;
        }
     
        case 2:{
            std::string val_code;
            std::string integrity_code;
            std::cout << "Insert Validation Code: " << std::endl;
            std::cin >> val_code;
            std::cout << "Insert Integrity Code: " << std::endl;
            std::cin >> integrity_code;

            // Create message
            CheckIntegrityMessage m = CheckIntegrityMessage(username, cookie, ++nonce, val_code, integrity_code);
            clientSocket.sendData(m.serialize());
            break;
        }

        case 3:
            // Logout
            logout(clientSocket);
            break;

        default:
            std::cerr << "Invalid input. Please enter a number." << std::endl;
            break;
        }

        // Receive and display the response from the server
        std::string received_message = clientSocket.receiveData();
        ActionType action = getActionType(received_message);

        // Only listen to authenticated messages
        if (action == ActionType::ACCEPT_AUTH || action == ActionType::REJECT_AUTH) {
            AuthenticatedMessage m = AuthenticatedMessage::deserialize(received_message);

            // Check if the message is authenticated correctly
            if (isAuthenticationCorrect(m)) {
    
                // Print the message -> Just declare the validaty of validation code or integrity code.
                if (action == ActionType::ACCEPT_AUTH) {
                    AcceptAuthMessage m = AcceptAuthMessage::deserialize(received_message);
                    std::cout << m.getMessage() << std::endl;
                } else {
                    RejectAuthMessage m = RejectAuthMessage::deserialize(received_message);
                    std::cout << m.getMessage() << std::endl;
                }
            } else{
                std::cout << "Invalid message received. Try again!" << std::endl;
            }   
        } else{
            std::cout << "Invalid message received. Try again!" << std::endl;
        }

    }


}


int main(int argc, char* argv[]) {
    TLSClient clientSocket = createValidator();

    // Blocking function, only returns when user is authenticated or something fails
    if (!authenticate(clientSocket)) {
        std::cerr << "Something went wrong!" << std::endl;
        return EXIT_FAILURE;
    }

    validatorMenu(clientSocket);

    return 0;
}