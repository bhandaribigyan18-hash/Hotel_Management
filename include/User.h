#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User {
protected:
    string username;
    string password;
    string role;

public:
    User();
    User(string username, string password, string role);
    virtual ~User();

    
    string getUsername() const;
    string getPassword() const;
    string getRole() const;

   
    void setUsername(const string& username);
    void setPassword(const string& password);
    void setRole(const string& role);

    
    bool login(const string& username, const string& password);
    bool registerUser(const string& username, const string& password, const string& role);
    void logout();

    virtual void displayInfo() const;
};

#endif 