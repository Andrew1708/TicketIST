#ifndef REJECTMESSAGE_H
#define REJECTMESSAGE_H

#include "Message.h"

class RejectMessage : public Message {
private:
    std::string message; 

public:
    RejectMessage(const std::string& message);

    static RejectMessage deserialize(const std::string& serialized);

    std::string serialize() const;

    std::string getMessage() const;

};

#endif // REJECTMESSAGE_H