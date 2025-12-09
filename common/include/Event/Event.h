#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <ctime>

class Event {
public:
    Event(int id, std::string name);

    int getId() const;
    std::string getName() const;
    
private:
    int id;
    std::string name;
};

#endif // EVENT_H