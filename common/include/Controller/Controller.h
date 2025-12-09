#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <utility>
#include "EventService.h"
#include "TicketService.h"
#include "UserService.h"
#include "Message.h"
#include "GetAvailableTicketsMessage.h"
#include "AcceptMessage.h"
#include "RejectMessage.h"
#include "LogInMessage.h"
#include "SignUpMessage.h"
#include "AuthenticatedMessage.h"
#include "RejectAuthMessage.h"
#include "AcceptAuthMessage.h"
#include "BuyTicketMessage.h"
#include "LogInValidatorMessage.h"
#include "ValidateTicketMessage.h"
#include "CheckIntegrityMessage.h"
#include "LogOutMessage.h"
#include "SellTicketMessage.h"
#include <openssl/ssl.h>


class Controller {
public:
    Controller();

    bool isAuthenticated(std::string username);
    bool isValidatorAuthenticated(std::string username);
    void logoutUser(std::string username);
    void logoutValidator(std::string username);
    void logoutUserBySSL(SSL* ssl);
    void saveSSL(SSL* ssl, std::string username);
    std::pair<std::string, long> authenticateUser(std::string username);
    std::pair<std::string, long> authenticateValidator(std::string username);
    bool isAuthenticationCorrect(std::string username, std::string cookie, long nonce, std::string message);
    bool isValidatorAuthenticationCorrect(std::string username, std::string cookie, long nonce, std::string message);

    std::string createAcceptMessage(const std::string& message);
    std::string createRejectMessage(const std::string& message);
    std::string createValidatorAcceptAuthMessage(const std::string& message, const std::string& username);
    std::string createValidatorRejectAuthMessage(const std::string& message, const std::string& username);
    std::string createAcceptAuthMessage(const std::string& message, const std::string& username);
    std::string createRejectAuthMessage(const std::string& message, const std::string& username);

    std::string doGetAvailableTicketsAction(GetAvailableTicketsMessage get_available_tickets_message);
    std::string handleGetAvailableTicketsError();
    std::string doSignUpAction(SignUpMessage sign_up_message, SSL* ssl);
    std::string doLogInAction(LogInMessage log_in_message, SSL* ssl);
    std::string doBuyTicketAction(BuyTicketMessage buy_ticket_message);
    std::string doLogInValidatorAction(LogInValidatorMessage log_in_validator_message, SSL* ssl);
    std::string doValidateTicketAction(ValidateTicketMessage validate_ticket_message);
    std::string doCheckIntegrityAction(CheckIntegrityMessage check_integrity_message);
    std::string doLogoutAction(LogOutMessage log_out_message);
    std::string doSellTicketAction(SellTicketMessage sell_ticket_message);

    std::string handleAction(ActionType action, SSL* ssl, const std::string& received_message);
    ActionType getActionType(const std::string& received_message);
    std::string messageHandler(SSL* ssl, const std::string& received_message);



private:
    EventService event_service;
    TicketService ticket_service;
    UserService user_service;

    // Map to store the authenticated users <username, <cookie, nonce>>
    std::map<std::string, std::pair<std::string, long>> auth_map;

    // Map to store authenticated validators   
    std::map<std::string, std::pair<std::string, long>>  validator_map;

    std::map<SSL*, std::string> ssl_to_username;
};


#endif // CONTROLLER_H