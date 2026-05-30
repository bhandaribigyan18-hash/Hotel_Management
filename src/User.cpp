#include "../include/User.h"
#include <iostream>
using namespace std;



User::User() : username(""), password(""), role("") {}

User::User(string username, string password, string role)
    : username(username), password(password), role(role) {}

User::~User() {}


string User::getUsername() const { return username; }
string User::getPassword() const { return password; }
string User::getRole()     const { return role; }



void User::setUsername(const string& u) { username = u; }
void User::setPassword(const string& p) { password = p; }
void User::setRole(const string& r)     { role = r; }


bool User::login(const string& uname, const string& pwd) {
    if (username == uname && password == pwd) {
        cout << "Login successful! Welcome, " << username << " (" << role << ")." << endl;
        return true;
    }
    cout << "Invalid username or password." << endl;
    return false;
}

bool User::registerUser(const string& uname, const string& pwd, const string& r) {
    if (uname.empty() || pwd.empty()) {
        cout << "Username and password cannot be empty." << endl;
        return false;
    }
    username = uname;
    password = pwd;
    role     = r;
    cout << "User '" << username << "' registered successfully as " << role << "." << endl;
    return true;
}

void User::logout() {
    cout << "User '" << username << "' has logged out." << endl;
}

void User::displayInfo() const {
    cout << "=== User Info ===" << endl;
    cout << "Username : " << username << endl;
    cout << "Role     : " << role     << endl;
}