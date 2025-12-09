#include "RejectMessage.h"

RejectMessage::RejectMessage(const std::string& message)
    : Message(ActionType::REJECT), message(message) {}

RejectMessage RejectMessage::deserialize(const std::string& serialized) {
    size_t pos = serialized.find(':');
    if (pos != std::string::npos) {
        std::string message = serialized.substr(pos + 1);
        return RejectMessage(message);
    } else {
        std::cerr << "Error: Invalid message format!" << std::endl;
        throw std::invalid_argument("Error: Invalid message format!");
    }
}

std::string RejectMessage::serialize() const {
    return std::to_string(static_cast<int>(ActionType::REJECT)) + ":" + message;
}

std::string RejectMessage::getMessage() const {
    return message;
}

