// ShadeForm Save Game extension for Text Adventure tools
// Gabriel Murphy - 2014
#include"shdefrm_adv.h"
#include"shdefrm_save.h"
#include"shdefrm_tools.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

const char* filename     = "savedata.wed";
const char* tempFilename = "tempsvdata.wed";        

// Called if a player who already has a save elects to "restart"
// rather than continue. This function will remove their current entry.
void remove_entry ( std::string &userId )
{
  std::string       inputString;
  std::ifstream     inputFile;
  std::ofstream     outputFile;
  bool              copyData = true;
  
  // Open savefile and create temp savefile
  do { inputFile.open( filename ); } while ( !inputFile.is_open() );
  do { outputFile.open( tempFilename ); } while ( !outputFile.is_open() );
  
  while ( getline( inputFile, inputString ) )
  {
    if ( inputString != userId && copyData )
    {
      outputFile << inputString << std::endl;
    }
    else if ( inputString == userId )
    {
      copyData = false;
      continue;
    }
    else if ( inputString == "*FIN*" )
    {
      copyData = true;
      continue;
    }
  }  
  
  inputFile.close();
  outputFile.close();

  copytemp( filename, tempFilename );
}

bool continue_game ( std::string &userId )
{
  std::string input;

  while ( true )
  {
    std::cout << "You have a game already in progress. Continue or restart? ";
    getline( std::cin, input );

    input = lowercase( input );
    
    if ( input == "continue" )
      return true;
    else if ( input == "restart" )
    {
      remove_entry ( userId );
      return false;
    }
    else
      std::cout << "Invalid entry." << std::endl;
  }
}

// External function
// Checks if a given user has saved data entry
// If so, extracts that data and sets it to Player object
// If not, sets "new game" data to Player object
Player load_save_data ( int uI )
{
  std::ifstream              inputFile;
  std::string                inputString, userId;
  std::vector< Item >        inventory;
  std::vector< CellOverlay > overlay;
  std::vector< std::string > inventoryItems;
  std::vector< bool >        tempBool;
  std::stringstream          ss;
  std::vector< int >         inventoryQuantities, saveData;
  bool                       tempLocked, playerHasSave        = false;
  int                        extractFlag, tempInt, tempIndex;
  int                        currentIndex                     = startingIndex;
  float                      maxHp                            = gStartingHp;
  float                      currentHp                        = maxHp;

  // Convert integer "uI" to string "userId"
  ss << uI;
  userId = ss.str();

  // Add pertinent data from default cell data into the cell overlay
  // This is done even before checking if the player already has a save in case 
  // A) The player doesn't a have a save or 
  // B) The player's saved overlay data is corrupt/incomplete which could lead to segfault later
  for ( int i = 0; i < cellVolume.size(); i++ )
  {    
     tempBool.clear();
     
     tempLocked = cellVolume[i].locked;
     
     // For now, all actions are set to active at game start
     // This may change later to introduce more complexity to games
     for ( int j = 0; j < cellVolume[i].actions.size(); j++ )
     {  
       tempBool.push_back( true );
     }
     CellOverlay tempCell( tempLocked, tempBool );
     
     overlay.push_back( tempCell );
  }
  
  // Open savedata.wed, or create if one doesn't exist
  do 
  {
    inputFile.open( filename );

    if ( !inputFile.is_open() )
    {  
      std::ofstream outputFile( filename );
      outputFile.close();
    }  
  } while ( !inputFile.is_open() );

  // Search for userId and extract save data from entry
  while ( getline( inputFile, inputString ) )
  {
    // First, search for userId entry
    if ( !playerHasSave )
    {
      if ( inputString == userId )
      {
        // Ask player if they wish to continue
        if ( continue_game( userId ) )
        {  
          playerHasSave = true;
          continue;
        }
        else
          break;
      }
      else
        continue;
    }

    // If save entry has been found, extract the data
    if ( inputString == "*CI*" )
    {
      extractFlag = 0;
      continue;
    }
    else if ( inputString == "*CHP*" )
    {  
      extractFlag = 1;
      continue;
    }
    else if ( inputString == "*MHP*" )
    {  
      extractFlag = 2;
      continue;
    }
    else if ( inputString == "*ITM*" )
    {  
      extractFlag = 3;
      continue;
    }
    else if ( inputString == "*QNT*" )
    {  
      extractFlag = 4;
      continue;
    }
    else if ( inputString == "*CEL*" )
    {
      extractFlag = 5;
      continue;
    }
    else if ( inputString == "*FIN*" )
      break;
    
    switch ( extractFlag )
    {
        case 0 :
          std::istringstream( inputString ) >> currentIndex;
          break;
        case 1 :
          std::istringstream( inputString ) >> currentHp;
          break;
        case 2 :
          std::istringstream( inputString ) >> maxHp;
          break;
        case 3 :
          inventoryItems.push_back( inputString );
          break;
        case 4 :
          std::istringstream( inputString ) >> tempInt;
          inventoryQuantities.push_back( tempInt );
          break;
        case 5 :
          saveData.clear(); // Delete previous contents of this vector
                ss.clear(); // Clear state flags for stringstream variable
          
          ss.str( inputString );
          
          // Seperate data and convert to integer
          while ( ss >> inputString )
          {
            std::istringstream( inputString ) >> tempInt;
            saveData.push_back( tempInt );
          }
          
          // Valid save data should have at least 2 elements
          if ( saveData.size() >= 2 )
            tempIndex = saveData[0]; // First element is cell overlay index
          else
            break;
          
          if ( tempIndex >= 0 && tempIndex < overlay.size() )
          {
            overlay[tempIndex].locked = saveData[1]; // Second element is cell locked status
            
            tempInt = 0;
            
            // Any elements after are the first two are action activated statuses
            for ( int i = 2; i < saveData.size(); i++, tempInt++ )
            {
              if ( tempInt < overlay[tempIndex].actionActive.size() )
                overlay[tempIndex].actionActive[tempInt] = saveData[i];  
            }
	      }
          break;
    }
  }

  inputFile.close();
  
  // Load items (if any)
  for ( int i = 0; i < inventoryItems.size(); i++ )
  {
     for ( int j = 0; j < itemVolume.size(); j++ )
     {
       if ( inventoryItems[i] == itemVolume[j].title )
       {
         inventory.push_back( itemVolume[j] );
         inventory.back().quantity = inventoryQuantities[i];
       }
     }  
  }
  
  return Player( currentIndex, inventory, overlay, currentHp, maxHp, gStartingAttack, gStartingBlock, uI );
}

// External function
// Called from the main file to save a player's progress
void save_game ( int uI, Player &playerRef )
{
  std::stringstream stringStream;
  std::string       userId, inputString;
  std::ifstream     inputFile;
  std::ofstream     outputFile;
  bool              playerHasSave = false; 
  bool              copyData      = true;
 
  // Convert integer Ui to string userId
  stringStream << uI;
  userId        = stringStream.str();

  // Open savefile and create temp savefile
  do { inputFile.open( filename ); } while ( !inputFile.is_open() );
  do { outputFile.open( tempFilename ); } while ( !outputFile.is_open() );

  while ( getline( inputFile, inputString ) )
  {
    if ( inputString != userId && copyData )
    {
      outputFile << inputString << std::endl;
    }
    else if ( inputString == userId )
    {
      // Overwrite old save entry for user
      playerHasSave = true;
      copyData      = false;
      
      outputFile << userId << std::endl;
      outputFile << "*CI*" << std::endl;
      outputFile << playerRef.index << std::endl;
      outputFile << "*CHP*" << std::endl;
      outputFile << playerRef.currentHp << std::endl;
      outputFile << "*MHP*" << std::endl;
      outputFile << playerRef.maxHp << std::endl;
      
      outputFile << "*ITM*" << std::endl;     
      
      // Dump names of items in inventory
      for ( int i = 0; i < playerRef.inventory.size(); i++ )
      {
        outputFile << playerRef.inventory[i].title << std::endl;
      }
      
      outputFile << "*QNT*" << std::endl;
      
      // Dump quantity of each item in inventory
      for ( int i = 0; i < playerRef.inventory.size(); i++ )
      {
        outputFile << playerRef.inventory[i].quantity << std::endl;
      }
      
      // For each cell overlay that has changed, make a save entry
      for ( int i = 0; i < playerRef.overlay.size(); i++ )
      {        
        // Checks if the locked status of the overlay is different than default
        bool lockHasChanged = ( playerRef.overlay[i].locked != cellVolume[i].locked );
        
        // Checks if any of the cell's actions has been deactivated (completed)
        bool actionHasChanged = false;
        
        for ( int j = 0; j < playerRef.overlay[i].actionActive.size(); j++ )
        {
          if ( !playerRef.overlay[i].actionActive[j] )
          {
            actionHasChanged = true;
            break;
          }
        }
        
        // If either the cell locked status or any actions have been deactivated, write a save for it
        if ( lockHasChanged || actionHasChanged )
        {
          outputFile << "*CEL*" << std::endl;
          outputFile << i << " " << (int) playerRef.overlay[i].locked;
          for ( int j = 0; j < playerRef.overlay[i].actionActive.size(); j++ )
            outputFile << " " << (int) playerRef.overlay[i].actionActive[j];
          outputFile << std::endl;
        }
      }
      
      outputFile << "*FIN*" << std::endl;
    }
    else if ( inputString == "*FIN*" )
      copyData = true;
  }

  // If no previous save was found, make an entry
  if ( !playerHasSave )
  {
    outputFile << userId << std::endl;
    outputFile << "*CI*" << std::endl;
    outputFile << playerRef.index << std::endl;
    outputFile << "*CHP*" << std::endl;
    outputFile << playerRef.currentHp << std::endl;
    outputFile << "*MHP*" << std::endl;
    outputFile << playerRef.maxHp << std::endl;
    outputFile << "*ITM*" << std::endl;     
      
    // Dump names of items in inventory
    for ( int i = 0; i < playerRef.inventory.size(); i++ )
    {
      outputFile << playerRef.inventory[i].title << std::endl;
    }
      
    outputFile << "*QNT*" << std::endl;
      
    // Dump quantity of each item in inventory
    for ( int i = 0; i < playerRef.inventory.size(); i++ )
    {
      outputFile << playerRef.inventory[i].quantity << std::endl;
    }
    
    // For each cell overlay that has changed, make a save entry
    for ( int i = 0; i < playerRef.overlay.size(); i++ )
    {
      // Checks if the locked status of the overlay is different than default
      bool lockHasChanged = ( playerRef.overlay[i].locked != cellVolume[i].locked );
        
      // Checks if any of the cell's actions has been deactivated (completed)
      bool actionHasChanged = false;
        
      for ( int j = 0; j < playerRef.overlay[i].actionActive.size(); j++ )
      {
        if ( !playerRef.overlay[i].actionActive[j] )
        {
          actionHasChanged = true;
          break;
        }
      }
      
      // If either the cell locked status or any actions have been deactivated, write a save for it
      if ( lockHasChanged || actionHasChanged )
      {
        outputFile << "*CEL*" << std::endl;
        outputFile << i << " " << (int) playerRef.overlay[i].locked;
        for ( int j = 0; j < playerRef.overlay[i].actionActive.size(); j++ )
          outputFile << " " << (int) playerRef.overlay[i].actionActive[j];
        outputFile << std::endl;
      }
    }
    
    outputFile << "*FIN*" << std::endl;
  }

  std::cout << "\nProgress successfully saved!\n\n";

  inputFile.close();
  outputFile.close();

  copytemp( filename, tempFilename );
}



