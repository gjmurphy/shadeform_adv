// Example Project for Shadeform Adventure Toolkit
// Gabriel Murphy - 2014
#include"shdefrm_adv.h"
#include"shdefrm_tools.h"
#include"shdefrm_login.h"
#include"shdefrm_save.h"
#include<iostream>
#include<iomanip>
#include<string>

// Keywords for system actions
#define SC_H  "help"
#define SC_SV "save"
#define SC_UU "update username"
#define SC_UP "update password"
#define SC_LO "logout"
#define SC_XT "exit"

using namespace std;

void display_help ()
{  
  cout << endl;
  display_game_help();
  cout << "*----------------System Commands-----------------*" << endl;
  cout << "'" << SC_SV << "'" << endl;
  cout << "'" << SC_UU << "'" << endl;
  cout << "'" << SC_UP << "'" << endl;
  cout << "'" << SC_LO << "'" << endl;
  cout << "'" << SC_XT << "'" << endl;
  cout << endl;
}

bool login_and_initialize ( User &currentUserRef, Player &playerRef )
{  
  // Get username
  currentUserRef = login( true );
  
  playerRef = load_save_data( currentUserRef.get_userid() );
  
  // If currentUser is equal to a predefined flag value, that means
  // authentication has failed and the program must end.
  if ( currentUserRef.get_username() == KYWRD_NL )
    return false;

  // Display player's current cell info to get the game started
  parse_instruction( GC_LOOK, playerRef );

  return true;
}

bool exit_check ()
{
  string input;
  
  cout << "\n***Warning: Any any unsaved progress will be lost!***" << endl;
  
  while ( true )
  {
    cout << "Are you sure you want to exit? (Yes or No) ";
    
    getline( cin, input );
    
    input = lowercase( input );
    
    if ( input == "yes" || input == "y" )
      return true;
    else if ( input == "no" || input == "n" )
      return false;
    else
      cout << "Invalid entry." << endl;
  }
}

int main()
{
  string input;
  User   currentUser;
  Player player;
  int    fightIndex;
  bool   didSurvive;

  if ( !load_data() )
  {
    cout << "Exiting program..." << endl;
    return 0;
  }

  // Welcome message
  display_message( MSG, "1.1" );
  display_message( PIC, "1.1" );
  display_message( MSG, "1.2" );
  
  // Initial login
  if ( !login_and_initialize( currentUser, player ) )
  {
    cout << "Exiting program..." << endl;
    return 0;
  }

  // Main program loop
  while ( true )
  {   
    fightIndex = check_for_fight( player );
    
    // Check for fight in current cell
    if ( fightIndex != -1 )
    {
      // Run the fight
      int fightResult = fight( fightIndex, player );
      
      // Check fight result
      if ( fightResult == 0 ) // Player lost
      {
        // Reload last save data if dead
         player = load_save_data( currentUser.get_userid() );
         
         // Display cell info again
         parse_instruction( GC_LOOK, player );
         
         continue;
      }
      else if ( fightResult == 1 ) // Player won
      {
        // Display cell info now
        parse_instruction( GC_LOOK, player );
        
        continue; 
      }
      else if ( fightResult == 2 ) // Player exited program
        return 0;
    }
    
    cout << "|--What do you want to do, " << currentUser.get_username() << "? ";
    getline( cin, input );

    input = lowercase( input );

    if ( input == SC_H )
      display_help();
    // Remove
    else if ( input == "map" )
      display_map( player.index );
    else if ( input == SC_SV )
      save_game( currentUser.get_userid(), player );
    else if ( input == SC_UU )
      update_username( currentUser );
    else if ( input == SC_UP )
    {
      if ( !update_password( currentUser ) )
      {
        cout << "Exiting program..." << endl;
        return 0;
      }	
    }
    else if ( input == SC_LO )
    {
      cout << endl;
      if ( !login_and_initialize( currentUser, player ) )
      {
        cout << "Exiting program..." << endl;
        return 0;
      }
    }
    else if ( input == SC_XT )
    {
       if ( exit_check() )
         return 0;
       else
         cout << endl;
    }
    else
    {
      int tempInt = parse_instruction( input, player );
      
      // tempInt 0: Not valid command tempInt 2: end of game reached
      if ( tempInt == 0 )
        cout << "\n*Instruction not recognized. Enter 'help' for a list of commands*\n\n";
      else if ( tempInt == 2 )
      {
        // Returns to login
        if ( !login_and_initialize( currentUser, player ) )
        {
          cout << "Exiting program..." << endl;
          return 0;
        }
      }
    }
  }  

  return 0;
}
