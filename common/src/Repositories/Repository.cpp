#include "Repository.h"
#include <iostream>

// Define static member
sql::Connection *Repository::conn = nullptr;

Repository::Repository(sql::Connection *connection) {
    if (conn == nullptr) {
        conn = connection; // Initialize the shared connection
    } 
}

Repository::Repository() {
    if (conn == nullptr) {
        throw std::runtime_error("Database connection not initialized. Pass a connection to Repository or call initializeConnection first.");
    }
}

Repository::~Repository() {
    // Do not clean up the connection here; it's managed externally
}
