#include "AcceptMessage.h"

AcceptMessage::AcceptMessage(const std::string& message)
    : Message(ActionType::ACCEPT), message(message) {}

AcceptMessage AcceptMessage::deserialize(const std::string& serialized) {
    size_t pos = serialized.find(':');
    if (pos != std::string::npos) {
        std::string message = serialized.substr(pos + 1);
        return AcceptMessage(message);
    } else {
        std::cerr << "Error: Invalid message format!" << std::endl;
        throw std::invalid_argument("Error: Invalid message format!");
    }
}



std::string AcceptMessage::serialize() const {
    return std::to_string(static_cast<int>(ActionType::ACCEPT)) + ":" + message;
}

std::string AcceptMessage::getMessage() const {
    return message;
}