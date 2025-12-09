#include "UserRepository.h"
#include <iostream>

UserRepository::UserRepository() : Repository() {}

bool UserRepository::createUser(const std::string &username, const std::string &password) {
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "INSERT INTO users (username, isValidator, password) VALUES (?, ?, ?)"
            );
            stmt->setString(1, username);
            stmt->setBoolean(2, false);
            stmt->setString(3, password);
            stmt->executeUpdate();

            delete stmt;
            return true;
        } else {
            std::cerr << "Database connection is not valid." << std::endl;
            return false;
        }

    } catch (sql::SQLException &e) {
        std::cerr << "Error creating user: " << e.what() << std::endl;
        return false;
    }
}

std::optional<User> UserRepository::getUserById(int userId) {
    User user;
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "SELECT id, username FROM users WHERE id = ?"
            );
            stmt->setInt(1, userId);
            
            sql::ResultSet *res = stmt->executeQuery();

            if (res->next()) {
                int id = res->getInt("id");
                std::string username = res->getString("username");

                user.setId(id);
                user.setUsername(username);

            } else {
                std::cout << "No user found with ID: " << userId << std::endl;
            }

            delete stmt;
            return user;
        } else {
            std::cerr << "Database connection is not valid." << std::endl;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error getting user: " << e.what() << std::endl;
    }
    return std::nullopt;
}

std::vector<User> UserRepository::getAllUsers() {
    std::vector<User> users; // Vector to store the results

    try {
        if (conn && conn->isValid()) {
            sql::Statement *stmt = conn->createStatement();
            sql::ResultSet *res = stmt->executeQuery("SELECT id, username, password FROM users");

            while (res->next()) {
                int id = res->getInt("id");
                std::string username = res->getString("username");
                std::string password = res->getString("password");

                // Create a User object and add it to the vector
                users.emplace_back(id, username, password);
            }

            delete res;
            delete stmt;
        } else {
            std::cerr << "Database connection is not valid." << std::endl;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error fetching all users: " << e.what() << std::endl;
    }

    return users; // Return the vector of users
}

std::optional<User> UserRepository::findUserByUsername(const std::string &username) {
    User user;
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "SELECT id, username FROM users WHERE username = ?"
            );
            stmt->setString(1, username);
            
            sql::ResultSet *res = stmt->executeQuery();

            if (res->next()) {
                int id = res->getInt("id");
                std::string username = res->getString("username");

                user.setId(id);
                user.setUsername(username);
                
                delete stmt;
                return user;

            } else {
                std::cout << "No user found with username: " << username << std::endl;
            }
    
          
        } else {
            std::cerr << "Database connection is not valid." << std::endl;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error getting user by username: " << e.what() << std::endl;
    }
    return std::nullopt;
}


bool UserRepository::checkPassword(std::string username, std::string password){
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "SELECT password FROM users WHERE username = ?"
            );
            stmt->setString(1, username);
            
            sql::ResultSet *res = stmt->executeQuery();

            if (res->next()) {
                std::string db_password = res->getString("password");
                if (db_password == password){
                    return true;
                }
            } else {
                std::cout << "No user found with username: " << username << std::endl;
                return false;
            }
    
            delete stmt;
            
        } else {
            std::cerr << "Database connection is not valid." << std::endl;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error getting user by username: " << e.what() << std::endl;
    }
    return false;
}

bool UserRepository::checkValidatorPassword(std::string username, std::string password){
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "SELECT password FROM validators WHERE username = ?"
            );
            stmt->setString(1, username);
            
            sql::ResultSet *res = stmt->executeQuery();

            if (res->next()) {
                std::string db_password = res->getString("password");
                if (db_password == password){
                    return true;
                }
            } else {
                std::cout << "No validator found with username: " << username << std::endl;
                return false;
            }
    
            delete stmt;
            
        } else {
            std::cerr << "Database connection is not valid." << std::endl;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error getting validator by username: " << e.what() << std::endl;
    }
    return false;
}

int UserRepository::getUserId(const std::string& username){
    try {
        if (conn && conn->isValid()) {
            sql::PreparedStatement *stmt = conn->prepareStatement(
                "SELECT id FROM users WHERE username = ?"
            );
            stmt->setString(1, username);
            
            sql::ResultSet *res = stmt->executeQuery();

            if (res->next()) {
                int id = res->getInt("id");
                return id;
            } else {
                std::cout << "No user found with username: " << username << std::endl;
                return -1;
            }
    
            delete stmt;
            
        } else {
            std::cerr << "Database connection is not valid." << std::endl;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error getting user by username: " << e.what() << std::endl;
    }
    return -1;
}