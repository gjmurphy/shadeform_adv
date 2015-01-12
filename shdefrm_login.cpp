// ShadeForm Portable Login Tool
// Gabriel Murphy - 2014
#include"shdefrm_login.h"
#include"shdefrm_tools.h"
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<stdio.h>

// Login Keywords
#define KYWRD_N "NEW"
#define KYWRD_L "LIST"
#define KYWRD_X "EXIT"

// Implementation of User class constructor
User::User ( std::string u, std::string p, int uI )
{
  username   = u;
  password   = p;
  userId     = uI;
}

// Implementation of check_password function from User class
bool User::check_password ( std::string passwordToCheck )
{
  if ( passwordToCheck == password )
    return true;
  else
    return false;
}

// Updates userdata.wed when changing User attributes
void update_records ( bool changeUsername, std::string recordToChange, std::string newValue )
{
  const char*   filename      = "userdata.wed";
  const char*   tempFilename  = "tempuserd.wed";
  int           tempNumber;
  std::ifstream inputFile;
  std::ofstream outputFile;
  std::string   tempU, tempP;
  int           tempUi;

  do { inputFile.open( filename ); } while ( !inputFile.is_open() );
  do { outputFile.open( tempFilename ); } while ( !outputFile.is_open() );

  inputFile >> tempNumber;
  outputFile << tempNumber;

  if ( changeUsername )
  {
    for ( int i = 0; i < tempNumber; i++ )
    {
      inputFile >> tempU >> tempP >> tempUi;
      
      if ( tempU == recordToChange )
        tempU = newValue;
      
      outputFile << "\n" << tempU << " " << tempP << " " << tempUi; 
    }
  }
  else
  {
    for ( int i = 0; i < tempNumber; i++ )
    {
      inputFile >> tempU >> tempP >> tempUi;
      
      if ( tempU == recordToChange )
        tempP = newValue;
      
      outputFile << "\n" << tempU << " " << tempP << " " << tempUi; 
    }
  }

  inputFile.close();
  outputFile.close();
  
  copytemp( filename, tempFilename );  

  return;
}

// Asks user for username when creating new username or changing existing one
std::string get_valid_username ( std::vector<User> &tempVector, std::string query )
{
   std::string input;

   while ( true )
   {
     bool isUnique = true;
 
     std::cout << query;
     getline( std::cin, input );     

     if ( has_spaces(input) || input == "" || input == KYWRD_N || input == KYWRD_L || input == KYWRD_NL || input == KYWRD_X )
     {  
       std::cout << "Invalid username." << std::endl;
       continue;
     } 

     for ( int i = 0; i < tempVector.size(); i++ )
     {
       if ( input == tempVector[i].get_username() )
       {
         std::cout << "Username already taken." << std::endl;
         isUnique = false;
         break;
       }
     }
     
     if ( isUnique )
       break;
   }
   
   return input;
}

// Gets a valid password entry when creating new user or changing password
std::string get_valid_password ( std::string query )
{
  std::string input;

  while ( true )
  {
    std::cout << query;
    getline( std::cin, input );
    
    if ( input == "" || has_spaces(input) )
    {
      std::cout << "Invalid password." << std::endl;
      continue;
    }
   
    break;
  }

  return input;
}

// Creates a new user and adds it to userdata.wed
void create_user ( std::vector< User > &tempVector )
{
  // Ask user for valid username and password
  std::string username = get_valid_username( tempVector, "Choose a username: " );
  std::string password = get_valid_password( "Choose a password: " );
  
  // Write new data to userdata.wed
  const char*   filename     = "userdata.wed";
  const char*   tempFilename = "tempuserd.wed";
  int           tempNumber   = 0;
  std::ifstream inputFile;
  std::ofstream outputFile;
  std::string   tempU, tempP;
  int           tempUi, userId = 2000;
 
  do { inputFile.open( filename ); } while ( !inputFile.is_open() );
  do { outputFile.open( tempFilename ); } while ( !outputFile.is_open() );
  
  inputFile >> tempNumber;
 
  tempNumber++;

  // Generate unique userId number
  for ( int i = 0; i < tempNumber; i++ )
    userId += 7;

  // Write number of users to temp file
  outputFile << tempNumber;

  // Write all previous users to temp file
  for ( int i = 0; i < tempVector.size(); i++ )
  {
    inputFile >> tempU >> tempP >> tempUi;
    outputFile << "\n" << tempU << " " << tempP << " " << tempUi;
  }
  
  // Write new user data to the end of the tempfile
  outputFile << "\n" << username << " " << password << " " << userId;

  inputFile.close();
  outputFile.close();
  
  // Copy the temp file to userdata.wed
  copytemp( filename, tempFilename );

  // Add newly created user data to the back of the referenced vector
  tempVector.push_back( User( username, password, userId ) );

  std::cout << "User '" << username << "' successfully created!" << std::endl; 
}

// Retrieves data from userdata.wed and adds it as User class objects to a vector for return
std::vector< User > get_data ()
{
  std::vector<User> tempVector;
  std::ifstream     inputFile;
  std::string       tempU, tempP;
  const char*       filename    = "userdata.wed";  
  int               tempUi, numOfUsers  = 0;

  // Attempt to open userdata file, if it doesn't exist, create file  
  do
  {
    inputFile.open( filename );
    
    if ( !inputFile.is_open() )
    {  
      std::ofstream createFile( filename );
      createFile.close();
    }
  } while ( !inputFile.is_open() );
  
  inputFile >> numOfUsers;  // First entry in file is number of users
  
  for ( int i = 0; i < numOfUsers; i++ )
  {
    inputFile >> tempU >> tempP >> tempUi;
    tempVector.push_back( User( tempU, tempP, tempUi ) );
  }  

  inputFile.close();
   
  return tempVector;
}

// Secondary function for login, gets a registered username from the user
int login_username ( std::vector< User > &tempVector )
{
  std::string input;
  int         tempIndex = -42;

  while ( true )
  {
    std::cout << "Enter Username (or enter '" << KYWRD_N << "' to register): ";
    getline ( std::cin, input );

    for ( int i = 0; i < tempVector.size(); i++ )
    {
      if ( input == tempVector[i].get_username() )
      {
        tempIndex = i;
        break;
      }
    }
    
    if ( input == KYWRD_N )
    {
      create_user ( tempVector );
      std::cout << "Returning to login..." << std::endl;
      continue;
    }
    else if ( input == KYWRD_L )
    {
      std::cout << "List of users: " << std::endl;
      
      for ( int i = 0; i < tempVector.size(); i++ )
      {
        std::cout << "'" << tempVector[i].get_username() << "'" << std::endl;  
      }
      continue;
    }
    else if ( input == KYWRD_X )
      return -42;
    else if ( tempIndex == -42 )
    {
      std::cout << "User doesn't exist." << std::endl;
      std::cout << "*Tip: Enter '" << KYWRD_L << "' to list currently registered usernames*" << std::endl; 
      continue;
    }

    break;
  }
  
  return tempIndex;
}

// Gets a password from user and checks if valid
bool login_password ( User tmpUsr, bool lL, int noA, std::string query )
{
  std::string input;
  bool        auth;
  int         counter = 0;

  while ( true )
  {
    std::cout << query;
    getline( std::cin, input );

    if ( tmpUsr.check_password( input ) )
    {
      auth = true;
      break;
    }
    
    std::cout << "Invalid password." << std::endl;
    counter++;

    if ( lL && counter < noA )
    {
      std::cout << "*" << noA - counter << " attempts remaining*" << std::endl;
    }
    else if ( lL && counter == noA )
    {
      auth = false;
      break;
    }
  }

  return auth;
}

// Main function for login - This can be called from other code
User login ( bool loginLimit, int numOfAttempts  )
{
  std::vector<User> usersVector;
  int               index;  
  User              tempUser ( KYWRD_NL, KYWRD_NL );  

  usersVector = get_data();

  // Check if any users exist, if not, ask to create new user
  if ( usersVector.size() == 0 )
  {
    std::cout << "No users currently exist." << std::endl;
    std::cout << "Creating new user..." << std::endl;
    create_user( usersVector );
    std::cout << "Returning to login..." << std::endl;
  }
  
  index         = login_username( usersVector );
  
  if ( index == -42 )
    return tempUser;

  if ( login_password( usersVector[index], loginLimit, numOfAttempts, "Password: " ) )  
   tempUser = usersVector[index];
  else
    std::cout << "*Authentication failed*" << std::endl;

  return tempUser;
}

// For changing current user's username - Can be called from other code
void update_username ( User &currentUser )
{
  std::vector<User> usersVector;
  std::string       newUsername;
  std::string       oldUsername = currentUser.get_username();

  usersVector = get_data();

  newUsername = get_valid_username( usersVector, "Choose a new username: " );

  update_records( true, oldUsername, newUsername );
  currentUser.change_username( newUsername );
  std::cout << "Username successfully changed to " << newUsername << "!" << std::endl;

  return; 
}

// For changing current user's username - Can be called from other code
bool update_password ( User &currentUser )
{
  std::vector<User> usersVector;
  std::string       newPassword;
  std::string       username = currentUser.get_username();
  
  // Gets password for current user, ends function if fails
  if ( !login_password ( currentUser, true, 3, "Old password: " ) )
    return false;
 
  newPassword = get_valid_password ( "New password: " );  

  update_records( false, username, newPassword );
  currentUser.change_password( newPassword );
  std::cout << "Password successfully changed!" << std::endl;

  return true;
}
