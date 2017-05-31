// ShadeForm Login Tool - Header
// Gabriel Murphy - 2014
#ifndef _LOGIN_INCLUDED
#define _LOGIN_INCLUDED

#include<string>
#include<vector>

// Global constant to hold Null keyword
const std::string KYWRD_NL = "NULL";

class User
{

private:

	std::string username;
	std::string password;
	int         userId;

public:

	// Prototypes
	User(std::string u = "NULL", std::string p = "NULL", int uI = 2000);

	bool check_password(std::string);

	// Defintions
	std::string get_username() { return username; }

	std::string get_password() { return password; }

	int         get_userid() { return userId; }

	void        change_username(std::string input) { username = input; }

	void        change_password(std::string input) { password = input; }

};

User login(bool loginLimit = false, int numOfAttempts = 3);

void update_username(User &currentUser);

bool update_password(User &currentUser);

#endif
