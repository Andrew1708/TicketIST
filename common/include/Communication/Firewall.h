#ifndef FIREWALL_H
#define FIREWALL_H

#include <string>
#include <unordered_map>
#include <chrono>
#include <set>
#include <netinet/in.h>

class Firewall {
public:
    Firewall();
    ~Firewall();

    // Add an IP to the allowed list
    void addAllowedIP(const std::string& ip);

    // Add an IP to the blocked list
    void addBlockedIP(const std::string& ip);

    // Check if an IP is allowed
    bool isAllowed(const std::string& ip);

    // Enforce rate limiting
    bool isRateLimited(const std::string& ip);

private:
    std::set<std::string> allowed_ips_;
    std::set<std::string> blocked_ips_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> client_last_request_;
    int rate_limit_seconds_;
};

#endif // FIREWALL_H
