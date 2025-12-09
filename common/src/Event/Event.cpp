#include "Event.h"

// Constructor
Event::Event(int id, std::string name) : id(id), name(name) {}

// Getters
std::string Event::getName() const {
    return name;
}

int Event::getId() const {
    return id;
}
