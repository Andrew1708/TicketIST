#include "AuthenticatedMessage.h"
#include <random>
#include <sstream>

// Constructor
AuthenticatedMessage::AuthenticatedMessage(const std::string& username, const std::string& sessionCookies, long nonce)
    : Message(ActionType::UNKNOWN), username(username), session_cookies(sessionCookies), nonce(nonce) {}

// Getter for session cookies
const std::string& AuthenticatedMessage::getSessionCookies() const {
    return session_cookies;
}

// Setter for session cookies
void AuthenticatedMessage::setSessionCookies(const std::string& sessionCookies) {
    this->session_cookies = sessionCookies;
}

// Getter for nonce
const long AuthenticatedMessage::getNonce() const {
    return nonce;
}

// Setter for nonce
void AuthenticatedMessage::setNonce(long nonce) {
    this->nonce = nonce;
}

// Getter for username
const std::string& AuthenticatedMessage::getUsername() const {
    return username;
}

// Setter for username
void AuthenticatedMessage::setUsername(const std::string& username) {
    this->username = username;
}


// Static method to generate a random cookie
const std::string& AuthenticatedMessage::generateCookie() {
    static std::string cookie;
    std::stringstream ss;

    // Generate a random alphanumeric string (32 characters)
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t length = 32;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    ss << "COOKIE_";
    for (size_t i = 0; i < length; ++i) {
        ss << charset[dist(gen)];
    }

    cookie = ss.str();
    return cookie;
}

// Static method to generate a random nonce
const long AuthenticatedMessage::generateNonce() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long> dist(1000000000, 9999999999); // Random 10-digit number
    return dist(gen);
}

// Serialize the object into a string
std::string AuthenticatedMessage::serialize() const {
    std::stringstream ss;

    // Format: "Username:SessionCookies:Nonce"
    ss << username << ":"
       << session_cookies << ":"
       << nonce << ":";

    return ss.str();
}

// Deserialize a string into an AuthenticatedMessage object
AuthenticatedMessage AuthenticatedMessage::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string action, username, sessionCookies, nonceStr;

    // Parse the serialized string
    if (std::getline(ss, action, ':') &&
        std::getline(ss, username, ':') &&
        std::getline(ss, sessionCookies, ':') &&
        std::getline(ss, nonceStr)){
        // Convert nonce from string to long
        long nonce = std::stol(nonceStr);

        // Create the object
        AuthenticatedMessage message(username, sessionCookies, nonce);

        return message;
    } else {
        throw std::invalid_argument("Error: Invalid message format!");
    }
}