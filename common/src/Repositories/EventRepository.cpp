#include "EventRepository.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>


EventRepository::EventRepository() : Repository()  {}

std::vector<Event> EventRepository::getAllEvents() {
    std::vector<Event> events;
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "SELECT * FROM events"
            );
            sql::ResultSet *res = stmt->executeQuery();
            while (res->next()) {
                int id = res->getInt("id");
                std::string name = res->getString("name");
                Event event(id, name);
                events.push_back(event);
            }
            delete res;
            delete stmt;
        } else {
            std::cerr << "Database connection is not valid." << std::endl;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error getting events: " << e.what() << std::endl;
    }
    return events;
}

std::optional<Event> EventRepository::getEventByName(const std::string& event_name) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "SELECT * FROM events WHERE name = ?"
            );
            stmt->setString(1, event_name);
            sql::ResultSet *res = stmt->executeQuery();
            if (res->next()) {
                int id = res->getInt("id");
                std::string name = res->getString("name");
                Event event(id, name);
                delete res;
                delete stmt;
                return event;
            } else {
                delete res;
                delete stmt;
                return std::nullopt;
            }
        } else {
            std::cerr << "Database connection is not valid." << std::endl;
            return std::nullopt;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error getting event by name: " << e.what() << std::endl;
        return std::nullopt;
    }
}

void EventRepository::addDummyEvents() {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "INSERT INTO events (name) VALUES (?)"
            );
            stmt->setString(1, "websummit");
            stmt->execute();
            stmt->setString(1, "taylorswift");
            stmt->execute();
            stmt->setString(1, "calculo");
            stmt->execute();
            stmt->setString(1, "fisica2");
            stmt->execute();
            delete stmt;
        } else {
            std::cerr << "Database connection is not valid." << std::endl;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error adding dummy events: " << e.what() << std::endl;
    }
}