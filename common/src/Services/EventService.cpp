#include "EventService.h"
#include <iostream>
#include <vector>

EventService::EventService() : eventRepository() {
    getAllEvents();
}

std::map<std::string, Event>EventService::getAllEvents() {
    std::vector<Event> eventList = eventRepository.getAllEvents();
    for (const Event& event : eventList) {
        eventCache.emplace(event.getName(), event);
    }
    return eventCache;
}

Event EventService::getEventByName(const std::string& event_name) {
    // Check if the event is already in the cache
    auto it = eventCache.find(event_name);
    if (it != eventCache.end()) {
        std::cout << "Event found in cache" << std::endl;
        return it->second; // Return the cached event
    }
    
    // If not in cache, retrieve it from the repository
    auto db_response = eventRepository.getEventByName(event_name);
    if (db_response.has_value()) {
        // Update the cache and return the event
        eventCache.emplace(event_name, db_response.value());
        return db_response.value();
    }

    // Throw an exception if the event is not found
    throw std::invalid_argument("Event not found");
}

std::map<std::string, Event> EventService::getEventCache() {
    return eventCache;
}
