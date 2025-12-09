#ifndef EVENTREPOSITORY_H
#define EVENTREPOSITORY_H

#include <vector>
#include <string>
#include <optional>
#include "Repository.h"
#include "Event.h"

class EventRepository : public Repository {
public:
    EventRepository();

    std::vector<Event> getAllEvents();
    std::optional<Event> getEventByName(const std::string& event_name);
    void addDummyEvents();


};

#endif // EVENTREPOSITORY_H