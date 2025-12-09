#ifndef ACCEPTMESSAGE_H
#define ACCEPTMESSAGE_H

#include "Message.h"

class AcceptMessage : public Message {
private:
    std::string message;
public:
    AcceptMessage(const std::string& message);

    static AcceptMessage deserialize(const std::string& serialized);

    std::string serialize() const;

    std::string getMessage() const;

};

#endif // ACCEPTMESSAGE_H