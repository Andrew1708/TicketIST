#include "Controller.h"
#include "Message.h"
#include "AuthenticatedMessage.h"
#include "LogInValidatorMessage.h"

Controller::Controller()
    :event_service(), ticket_service(), user_service()  {}



void Controller::logoutUserBySSL(SSL* ssl) {
    std::cout << "Logging out user SSL." << std::endl;
    std::string username = ssl_to_username[ssl];
    logoutUser(username);
    ssl_to_username.erase(ssl);
}

void Controller::saveSSL(SSL* ssl, std::string username) {
    ssl_to_username[ssl] = username;
}

std::pair<std::string, long> Controller::authenticateValidator(std::string username){
    long nonce = AuthenticatedMessage::generateNonce();
    std::string cookie = AuthenticatedMessage::generateCookie();
    std::pair cookie_nonce_pair = std::make_pair(cookie, nonce);
    validator_map[username] = cookie_nonce_pair;
    return cookie_nonce_pair;
}

std::pair<std::string, long> Controller::authenticateUser(std::string username) {
    long nonce = AuthenticatedMessage::generateNonce();
    std::string cookie = AuthenticatedMessage::generateCookie();
    std::pair cookie_nonce_pair = std::make_pair(cookie, nonce);
    auth_map[username] = cookie_nonce_pair;
    return cookie_nonce_pair;
}

bool Controller::isValidatorAuthenticated(std::string username) {
    return validator_map.find(username) != validator_map.end();
}

bool Controller::isAuthenticated(std::string username) {
    return auth_map.find(username) != auth_map.end();
}

void Controller::logoutValidator(std::string username) {
    validator_map.erase(username);
}

void Controller::logoutUser(std::string username) {
    auth_map.erase(username);
}

bool Controller::isAuthenticationCorrect(std::string username, std::string cookie, long nonce, std::string message) {
    std::cout << "Checking authentication for " << username << std::endl;
    if (!isAuthenticated(username)) {
        message = "User not authenticated.";
        std::cout << message << std::endl;
        return false;
    }

    else if(auth_map[username].first != cookie) {
        message = "Cookie mismatch";
        std::cout << message << std::endl;
        return false;
    }

    else if(auth_map[username].second + 1 != nonce) {
        message = "Nonce mismatch";
        std::cout << message << std::endl;
        return false;
    }

    else {
        std::cout << "User authenticated." << std::endl;
        return true;
    }
}

bool Controller::isValidatorAuthenticationCorrect(std::string username, std::string cookie, long nonce, std::string message) {
    std::cout << "Checking authentication for " << username << std::endl;
    if (!isValidatorAuthenticated(username)) {
        message = "Validator not authenticated.";
        std::cout << message << std::endl;
        return false;
    }

    else if(validator_map[username].first != cookie) {
        message = "Cookie mismatch";
        std::cout << message << std::endl;
        return false;
    }

    else if(validator_map[username].second + 1 != nonce) {
        message = "Nonce mismatch";
        std::cout << message << std::endl;
        return false;
    }

    else {
        std::cout << "Validator authenticated." << std::endl;
        return true;
    }
}

std::string Controller::createAcceptMessage(const std::string& message) {
    AcceptMessage accept_message(message);
    return accept_message.serialize();
}

std::string Controller::createRejectMessage(const std::string& message) {
    RejectMessage reject_message(message);
    return reject_message.serialize();
}

std::string Controller::createValidatorAcceptAuthMessage(const std::string& message, const std::string& username) {
    std::string cookie = validator_map[username].first;
    long nonce = ++validator_map[username].second;
    AcceptAuthMessage accept_auth_message(message, username, cookie, nonce);
    return accept_auth_message.serialize();
}

std::string Controller::createValidatorRejectAuthMessage(const std::string& message, const std::string& username) {
    std::string cookie = validator_map[username].first;
    long nonce = ++validator_map[username].second;
    RejectAuthMessage reject_auth_message(message, username, cookie, nonce);
    return reject_auth_message.serialize();
}

std::string Controller::createAcceptAuthMessage(const std::string& message, const std::string& username) {
    std::string cookie = auth_map[username].first;
    long nonce = ++auth_map[username].second;
    AcceptAuthMessage accept_auth_message(message, username, cookie, nonce);
    return accept_auth_message.serialize();
}


std::string Controller::createRejectAuthMessage(const std::string& message, const std::string& username) {
    std::string cookie = auth_map[username].first;
    long nonce = ++auth_map[username].second;
    RejectAuthMessage reject_auth_message(message, username, cookie, nonce);
    return reject_auth_message.serialize();
}

std::string Controller::doSignUpAction(SignUpMessage sign_up_message, SSL* ssl) {
    if (user_service.signUp(sign_up_message.get_username(), sign_up_message.get_password())) {
        std::cout << "User authenticated." << std::endl;
        saveSSL(ssl, sign_up_message.get_username());
        std::pair cookie_nonce_pair = authenticateUser(sign_up_message.get_username());
        std::string message = "Hello, " + sign_up_message.get_username() + "!\n";
        return createAcceptAuthMessage(message, sign_up_message.get_username());
    } else {
        std::cout << "User not authenticated." << std::endl;
        std::string message = "Sign-up failed. Please try again.\n";
        return createRejectMessage(message);
    }
}

std::string Controller::doLogInAction(LogInMessage log_in_message, SSL* ssl) {
    if (user_service.loginUser(log_in_message.get_username(), log_in_message.get_password())) {
        std::cout << "User authenticated." << std::endl;
        saveSSL(ssl, log_in_message.get_username());
        std::pair cookie_nonce_pair = authenticateUser(log_in_message.get_username());
        std::string message = "Hello, " + log_in_message.get_username() + "!\n";
        return createAcceptAuthMessage(message, log_in_message.get_username());
    } else {
        std::cout << "User not authenticated." << std::endl;
        std::string message = "Log-in failed. Please try again.\n";
        return createRejectMessage(message);
    }
}

std::string Controller::doGetAvailableTicketsAction(GetAvailableTicketsMessage get_available_tickets_message){
    try{
        // Check if the user is authenticated
        std::string message;
        if (!isAuthenticationCorrect(get_available_tickets_message.getUsername(), get_available_tickets_message.getSessionCookies(), get_available_tickets_message.getNonce(), message)) {
            return createRejectMessage(message);
        }
        // Get the event object from the given name
        std::string event_name = get_available_tickets_message.getEventName();
        Event event = event_service.getEventByName(event_name);
        std::cout << "Event found: " << event.getName() << std::endl;

        // Get the available tickets for the event
        std::vector<TicketInfo> tickets = ticket_service.getAllAvailableTicketsByEventId(event.getId());
        if (tickets.empty()) {
            message = "No available tickets for " + event.getName() + ".\n";

        } else{
            message = "Available tickets for " + event.getName() + ":\n";
            // Add a header row
            message += "TicketID   | EventID    | SellerID   | Price       | Seat  \n";
            message += "-----------------------------------------------------------\n";
            for (const TicketInfo& ticket : tickets) {
                message += ticket.toString() + "\n";
            }
            message += "-----------------------------------------------------------\n";
        }

        return createAcceptAuthMessage(message, get_available_tickets_message.getUsername());
    } catch (const std::invalid_argument& e) {
        std::string message = handleGetAvailableTicketsError();
        return createRejectAuthMessage(message, get_available_tickets_message.getUsername());
    }

}

std::string Controller::handleGetAvailableTicketsError() {
    // Respond to the client with a reject message with all the available events name
    std::map<std::string, Event> events = event_service.getEventCache();
    std::string message = "Event not found. Available events are: ";
    for (const auto& [name, event] : events) {
        message += name + ", ";
    }
    // Remove the trailing comma and space
    if (!events.empty()) {
        message.pop_back();
        message.pop_back();
    }
    message += "\n";
    return message;
}

std::string Controller::doBuyTicketAction(BuyTicketMessage buy_ticket_message) {
    try{
        // Check if the user is authenticated
        std::string message;
        if (!isAuthenticationCorrect(buy_ticket_message.getUsername(), buy_ticket_message.getSessionCookies(), buy_ticket_message.getNonce(), message)) {
            return createRejectMessage(message);
        }
        // Translate event name to eventID
        int event_id = event_service.getEventByName(buy_ticket_message.getEventName()).getId();
        // Buy the ticket
        int user_id = user_service.getUserId(buy_ticket_message.getUsername());
        Ticket ticket = ticket_service.buyTicket(buy_ticket_message.getTicketId(), event_id, user_id);
        message = ticket.serialize();
        // Respond to the client with the ticket details
        return createAcceptAuthMessage(message, buy_ticket_message.getUsername());

    } catch (const std::invalid_argument& e) {
        std::string message = e.what();
        return createRejectAuthMessage(message, buy_ticket_message.getUsername());
    }

}

std::string Controller::doLogInValidatorAction(LogInValidatorMessage log_in_validator_message, SSL* ssl){
    if (user_service.loginValidator(log_in_validator_message.get_username(), log_in_validator_message.get_password())) {
        std::cout << "Validator authenticated." << std::endl;
        saveSSL(ssl, log_in_validator_message.get_username());
        std::pair cookie_nonce_pair = authenticateValidator(log_in_validator_message.get_username());
        std::string message = "Hello, " + log_in_validator_message.get_username() + "!\n";
        return createValidatorAcceptAuthMessage(message, log_in_validator_message.get_username());
    } else {
        std::cout << "Validator not authenticated." << std::endl;
        std::string message = "Log-in failed. Please try again.\n";
        return createRejectMessage(message);
    }
}

std::string Controller::doValidateTicketAction(ValidateTicketMessage validate_ticket_message){
    try{
        // Check if the user is authenticated
        std::string message;
        if (!isValidatorAuthenticationCorrect(validate_ticket_message.getUsername(), validate_ticket_message.getSessionCookies(), validate_ticket_message.getNonce(), message)) {
            // Problem with authentication reject message without disclosing authentication details
            return createRejectMessage(message);
        }
        // Validate the ticket
        std::string info; // info of the seat of if the ticket is inuse
        if(ticket_service.validateTicket(validate_ticket_message.getValCode(), info)){
            // Respond to the client with a success message
            message = "Ticket validated successfully.\n";
            message += "Ticket for seat:" + info + "\n";
            return createValidatorAcceptAuthMessage(message, validate_ticket_message.getUsername());
        } else {
            // Respond to the client with a failure message
            message = "Ticket validation failed.\n";
            message += info + "\n";
            return createValidatorRejectAuthMessage(message, validate_ticket_message.getUsername());
        }

    } catch (const std::invalid_argument& e) {
        std::string message = e.what();
        return createValidatorRejectAuthMessage(message, validate_ticket_message.getUsername());
    }
}

std::string Controller::doCheckIntegrityAction(CheckIntegrityMessage check_integrity_message){
    try{
        // Check if the user is authenticated
        std::string message;
        if (!isValidatorAuthenticationCorrect(check_integrity_message.getUsername(), check_integrity_message.getSessionCookies(), check_integrity_message.getNonce(), message)) {
            // Problem with authentication reject message without disclosing authentication details
            return createRejectMessage(message);
        }

        if(ticket_service.checkIntegrity(check_integrity_message.getValCode(), check_integrity_message.getIntegrityCode(), message)){
            return createValidatorAcceptAuthMessage(message, check_integrity_message.getUsername());
        } else {
            return createValidatorRejectAuthMessage(message, check_integrity_message.getUsername());
        }

    } catch (const std::invalid_argument& e) {
        std::string message = e.what();
        return createValidatorRejectAuthMessage(message, check_integrity_message.getUsername());
    }
}
std::string Controller::doLogoutAction(LogOutMessage log_out_message) {
    std::string message;
    if (isAuthenticationCorrect(log_out_message.getUsername(), log_out_message.getSessionCookies(), log_out_message.getNonce(), message)) {
        // Its a regular user
        logoutUser(log_out_message.getUsername());
        return createAcceptMessage("Log-out successful.\n");
    }

    if (isValidatorAuthenticationCorrect(log_out_message.getUsername(), log_out_message.getSessionCookies(), log_out_message.getNonce(), message)) {
        // Its a validator
        logoutValidator(log_out_message.getUsername());
        return createAcceptMessage("Log-out successful.\n");
    }

    // Respond to the client with a reject message
    return createRejectMessage(message);
}

std::string Controller::doSellTicketAction(SellTicketMessage sell_ticket_message){
    try{
        // Check if the user is authenticated
        std::string message;
        if (!isAuthenticationCorrect(sell_ticket_message.getUsername(), sell_ticket_message.getSessionCookies(), sell_ticket_message.getNonce(), message)) {
            return createRejectMessage(message);
        }
        // Sell the ticket
        ticket_service.sellTicket(sell_ticket_message.getTicket(), sell_ticket_message.getNewPrice());
        message = "Ticket was sucessfully put on sale.\n";

        // Respond to the client with the ticket details
        return createAcceptAuthMessage(message, sell_ticket_message.getUsername());

    } catch (const std::invalid_argument& e) {
        std::string message = e.what();
        return createRejectAuthMessage(message, sell_ticket_message.getUsername());
    }
}

std::string Controller::handleAction(ActionType action, SSL* ssl,const std::string& received_message) {
    switch (action) {
        case ActionType::TICKET_SELL: {
            std::cout << "Received a ticket sell message." << std::endl;
            //auto sell_ticket_message = SellTicketMessage::deserialize(received_message);
            //TODO: Implement the ticket sell action
            break;
        }

        case ActionType::GET_AVAILABLE_TICKETS: {
            std::cout << "Received a get available tickets message." << std::endl;
            auto get_available_tickets_message = GetAvailableTicketsMessage::deserialize(received_message);
            std::cout << "Serialization successful." << std::endl;
            return doGetAvailableTicketsAction(get_available_tickets_message);
            break;
        }

        case ActionType::BUY_TICKET: {
            std::cout << "Received a buy ticket message." << std::endl;
            auto buy_ticket_message = BuyTicketMessage::deserialize(received_message);
            std::cout << "Serialization successful." << std::endl;
            return doBuyTicketAction(buy_ticket_message);
            break;
        }

        case ActionType::LOG_IN_VALIDATOR:{
            std::cout << "Received a log-in validator message." << std::endl;
            auto log_in_validator_message = LogInValidatorMessage::deserialize(received_message);
            return doLogInValidatorAction(log_in_validator_message, ssl);
        }

        case ActionType::VALIDATE_TICKET:{
            std::cout << "Received a validate ticket message." << std::endl;
            auto validate_ticket_message = ValidateTicketMessage::deserialize(received_message);
            return doValidateTicketAction(validate_ticket_message);
        }

        case ActionType::CHECK_INTEGRITY:{
            std::cout << "Received a check integrity message." << std::endl;
            auto check_integrity_message = CheckIntegrityMessage::deserialize(received_message);
            return doCheckIntegrityAction(check_integrity_message);
        }   

        case ActionType::LOG_OUT: {
            std::cout << "Received a log-out message." << std::endl;
            auto log_out_message = LogOutMessage::deserialize(received_message);
            return doLogoutAction(log_out_message);
            break;
        }

        case ActionType::SELL_TICKET:{
            std::cout << "Received a sell ticket message." << std::endl;
            auto sell_ticket_message = SellTicketMessage::deserialize(received_message);
            return doSellTicketAction(sell_ticket_message);
        }

        case ActionType::ACCEPT: {
            std::cout << "Received an accept message." << std::endl;
            auto accept_message = AcceptMessage::deserialize(received_message);
            break;
        }
        case ActionType::REJECT: {
            std::cout << "Received a reject message." << std::endl;
            auto reject_message = RejectMessage::deserialize(received_message);
            break;
        }

        case ActionType::SIGN_UP: {
            std::cout << "Received a sign-up message." << std::endl;
            auto sign_up_message = SignUpMessage::deserialize(received_message);
            return doSignUpAction(sign_up_message, ssl);
            break;
        }
        case ActionType::LOG_IN: {
            std::cout << "Received a log-in message." << std::endl;
            auto log_in_message = LogInMessage::deserialize(received_message);
            return doLogInAction(log_in_message, ssl);
            break;
        }
        default:
            std::cout << "Received an unknown message. Ignored!" << std::endl;
            std::cout << received_message << std::endl;
            break;
    
    }
    return "";
}

ActionType Controller::getActionType(const std::string& message) {
    size_t pos = message.find(':');
    std::cout << "Received" << message << std::endl;
    if (pos == std::string::npos) {
        std::cerr << "Invalid message format." << std::endl;
        return ActionType::UNKNOWN;
    }
    int action_int = std::stoi(message.substr(0, pos));
    return static_cast<ActionType>(action_int);
}

std::string Controller::messageHandler(SSL* ssl, const std::string& received_message){
    // Get action type from the message
    ActionType action = getActionType(received_message);

    // Handle the action
    return handleAction(action, ssl, received_message);
}
