#ifndef CHECKINTEGRITYMESSAGE_H
#define CHECKINTEGRITYMESSAGE_H

#include <string>
#include "AuthenticatedMessage.h"

class CheckIntegrityMessage : public AuthenticatedMessage{
public:
    CheckIntegrityMessage(const std::string& user, const std::string& session_cookie, long nonce, const std::string& val_code, const std::string& integrity_code);

    std::string serialize() const;
    static CheckIntegrityMessage deserialize(const std::string& data);

    const std::string& getValCode() const;
    void setValCode(const std::string& val_code);

    const std::string& getIntegrityCode() const;
    void setIntegrityCode(const std::string& integrity_code);


private:
    std::string val_code;
    std::string integrity_code;
};

#endif // CHECKINTEGRITYMESSAGE_H