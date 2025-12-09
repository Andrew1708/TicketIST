#ifndef EVENTSERVICE_H
#define EVENTSERVICE_H

#include <string>
#include <vector>
#include "EventRepository.h"
#include "Event.h"

class EventService {
public:
    EventService();

    std::map<std::string, Event> getAllEvents();
    Event getEventByName(const std::string& event_name);
    std::map<std::string, Event> getEventCache();

private:
    EventRepository eventRepository;
    std::map<std::string, Event>  eventCache;
};

#endif // EVENTSERVICE_H