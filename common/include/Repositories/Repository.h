#ifndef REPOSITORY_H
#define REPOSITORY_H

#ifdef __APPLE__
// macOS includes
#include "/usr/local/mysql-connector-c++-9.1.0/include/jdbc/mysql_driver.h"
#include "/usr/local/mysql-connector-c++-9.1.0/include/jdbc/mysql_connection.h"
#include "/usr/local/mysql-connector-c++-9.1.0/include/jdbc/cppconn/statement.h"
#include "/usr/local/mysql-connector-c++-9.1.0/include/jdbc/cppconn/prepared_statement.h"
#include "/usr/local/mysql-connector-c++-9.1.0/include/jdbc/cppconn/resultset.h"
#include "/usr/local/mysql-connector-c++-9.1.0/include/jdbc/cppconn/exception.h"
#elif __linux__
// Linux includes
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#else
#error "Unsupported platform"

#endif
#include <string>

class Repository {
protected:
    static sql::Connection *conn; // Shared database connection

public:
    // Constructor to initialize with an existing connection
    explicit Repository(sql::Connection *connection);

    // Constructor for derived classes
    Repository();

    virtual ~Repository();
};

#endif // REPOSITORY_H
