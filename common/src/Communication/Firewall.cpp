#include "Firewall.h"
#include <iostream>

Firewall::Firewall()
    : rate_limit_seconds_(1) { // Default rate limit to 1 request per second
}

Firewall::~Firewall() {}

// Add an IP to the allowed list
void Firewall::addAllowedIP(const std::string& ip) {
    allowed_ips_.insert(ip);
}

// Add an IP to the blocked list
void Firewall::addBlockedIP(const std::string& ip) {
    blocked_ips_.insert(ip);
}

// Check if an IP is allowed
bool Firewall::isAllowed(const std::string& ip) {
    if (blocked_ips_.find(ip) != blocked_ips_.end()) {
        std::cerr << "Blocked IP attempted connection: " << ip << std::endl;
        return false;
    }
    if (!allowed_ips_.empty() && allowed_ips_.find(ip) == allowed_ips_.end()) {
        std::cerr << "Unauthorized IP attempted connection: " << ip << std::endl;
        return false;
    }
    return true;
}

// Enforce rate limiting
bool Firewall::isRateLimited(const std::string& ip) {
    auto now = std::chrono::steady_clock::now();

    if (client_last_request_.find(ip) != client_last_request_.end()) {
        auto last_request = client_last_request_[ip];
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_request).count() < rate_limit_seconds_) {
            std::cerr << "Rate limit exceeded for IP: " << ip << std::endl;
            return true;
        }
    }

    client_last_request_[ip] = now;
    return false;
}
