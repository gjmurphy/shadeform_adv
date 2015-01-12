// ShadeForm Text Adventure Tools
// Gabriel Murphy - 2014
#include"shdefrm_adv.h"
#include"shdefrm_tools.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<cstring>
#include<unistd.h>

// Global containers for storing the external game data in memory
// After they're filled on program start, they are used in a read-only capicity
std::vector< std::vector< std::string > > msgVolumes;
std::vector< Cell >                       cellVolume;
std::vector< Item >                       itemVolume;
std::vector< Enemy >                      enemyVolume;
std::vector< std::string >                moveKeywords, dirKeywords;
std::string                               inventoryMsg;
int                                       startingIndex;
float                                     gStartingHp, gStartingAttack, gStartingBlock;
                                   
/***********************************
Beginning of miscellaneous functions
***********************************/

// Implementation of custom == operator for the Vector2 object
bool operator ==( const Vector2 &left, const Vector2 &right )
{
  return ( left.x == right.x )
     and ( left.y == right.y );
}

bool Player::hurt_player ( float attack )
{
  currentHp -= attack;
  
  if ( currentHp <= 0 )
    return false;
  else
    return true;
}

bool Player::heal_player ( float power )
{
  if ( currentHp == maxHp )
    return false;
  
  currentHp += power;
  
  if ( currentHp > maxHp )
    currentHp = maxHp;
    
  return true;
}

void Player::display_hp ()
{
  std::cout << "\nYour current HP: " << currentHp << "/" << maxHp << "\n\n";
}

bool Enemy::hurt_enemy ( float attack )
{
  currentHp -= attack;
  
  if ( currentHp <= 0 )
    return false;
  else
    return true;
}

void Enemy::heal_enemy ( float power )
{  
  currentHp += power;
  
  if ( currentHp > maxHp )
    currentHp = maxHp;
}

// External and internal function
// Called to display content from data files
void display_message ( Volume volume, std::string messageId )
{
  std::string              output;
  int                      index      = (int) volume;
  int                      length;
  int                      startIndex = 0;
  int                      endIndex   = 0;

  for ( int i = 0; i < msgVolumes[index].size(); i++ )
  {
    // Attempts to locate the given index
    if ( msgVolumes[index][i] == messageId )
    {
      // "Number of lines in message" value is one entry after the "msgId" just found
      std::istringstream( msgVolumes[index][++i] ) >> length;
      // The actual message content starts two entries after "msgId"
      startIndex = ++i;
      // Now add both startpoint and length together to get the end index
      endIndex = startIndex + length;
      break;
    }
  }
 
  // Display only the strings in the vector according the startpoint and true length 
  for ( int i = startIndex; i < endIndex; i++ )
  {
    std::cout << msgVolumes[index][i] << std::endl;
  }
}

void display_game_help ()
{
  std::cout << "*------------Common Game Instructions------------*" << std::endl;
  std::cout << "'" << GC_LOOK << "' Re-display current room info" << std::endl;
  std::cout << "'" << GC_INV << "' List items in " << inventoryMsg << std::endl;
  std::cout << "'" << GC_HP << "' Current HP status" << std::endl; 
  std::cout << "'examine item_name' Describes item" << std::endl;
  std::cout << "'*keyword* item_name' Uses item" << std::endl;
}

// Checks if two vectors contain a similar value and returns the index of the ref
int compare_vectors ( std::vector< std::string > &check, std::vector< std::string > &ref )
{
  for ( int i = 0; i < check.size(); i++ )
  {
    for ( int j = 0; j < ref.size(); j++ )
    {
      if ( check[i] == ref[j] )
        return j;
    }
  }
  
  return -1;
}

bool contains ( std::string checkFor, std::string inThis )
{
   return ( inThis.find( checkFor ) != std::string::npos );
}

/********************************
Beginning of inventory functions
*********************************/

void display_inventory ( std::vector< Item > &invRef )
{
  // Display empty inventory message if inventory vector is empty
  if ( !invRef.size() )
  {
     std::cout << "\n***Your " << lowercase( inventoryMsg ) << " is currently empty***\n\n";
     return;
  }
  
  std::cout << "\n***" << inventoryMsg << " Contents***" << std::endl;
  
  // Print name and quantity of each item in inventory
  for ( int i = 0; i < invRef.size(); i++ )
	  std::cout << invRef[i].title << " x " << invRef[i].quantity << std::endl;
	  
  std::cout << std::endl;
}

int get_item_index ( std::string title )
{
  for ( int i = 0; i < itemVolume.size(); i++ )
  {
    if ( title == itemVolume[i].title )
      return i;
  }

  return -1;
}

int get_item_index ( std::string title, std::vector< Item > &invRef )
{
  for ( int i = 0; i < invRef.size(); i++ )
  {
    if ( title == invRef[i].title )
      return i;
  }

  return -1;
}

int get_best_potion ( std::vector< Item > &invRef )
{
  float bestPower = 0;
  int   index     = -1;
  
  for ( int i = 0; i < invRef.size(); i++ )
  {
    if ( invRef[i].type == POTION && invRef[i].power > bestPower )
    {
        index     = i;
        bestPower = invRef[i].power;
    }
  }

  return index;
}

void remove_item ( Player &playerRef, bool isHealing, int invIndex, int amount = 1 )
{
  playerRef.inventory[invIndex].decrease_quantity( amount );
  
  std::cout << "\n*** " << amount << " x " << playerRef.inventory[invIndex].title;
  std::cout << " removed from " << inventoryMsg << " ***" << std::endl;

  if ( !isHealing )
    std::cout << std::endl;  

  // Removes item completely if less than 1
  if ( playerRef.inventory[invIndex].quantity < 1 )
    playerRef.inventory.erase( playerRef.inventory.begin() + invIndex );
}

void add_item ( Player &playerRef, int itemVolumeIndex, int amount = 1 )
{
  // Check if item already exists in players inventory
  int invIndex = get_item_index( itemVolume[itemVolumeIndex].title, playerRef.inventory );
  
  if ( invIndex != -1 )
    playerRef.inventory[invIndex].increase_quantity( amount );
  else
  {
    playerRef.inventory.push_back( itemVolume[itemVolumeIndex] );
    
    playerRef.inventory.back().increase_quantity( amount - 1 );
  }
  
  std::cout << "\n*** " << amount << " x " << itemVolume[itemVolumeIndex].title;
  std::cout << " added to " << inventoryMsg << " ***\n\n";  
}

// Use item for player
void use_potion ( int index, Player &playerRef )
{
  float oldHp = playerRef.currentHp;
  
  if ( playerRef.heal_player( playerRef.inventory[index].power ) )
  {
    std::cout << "\n***" << playerRef.currentHp - oldHp << "HP healed***";
    
    remove_item( playerRef, true, index, 1 );
    
    playerRef.display_hp();
  }
  else
    std::cout << "\n*** Already fully healed! ***\n";
}

// Use item for enemy
void use_potion ( int index, Enemy &enemyRef )
{
  float oldHp = enemyRef.currentHp;
  
  enemyRef.heal_enemy( enemyRef.inventory[index].power );
  
  std::cout << "\n***" << enemyRef.title << " used a " << enemyRef.inventory[index].title;
  std::cout << " to heal " << enemyRef.currentHp - oldHp << "HP***\n";
  
  enemyRef.inventory[index].decrease_quantity( 1 );
 
  // If less than 1 delete item
  if ( enemyRef.inventory[index].quantity < 1 )
    enemyRef.inventory.erase( enemyRef.inventory.begin() + index );
}

/*******************************
Beginning of load data functions
********************************/

// Used by load_data function to load the data from a given file into a given vector
bool load_message_data ( std::vector< std::string > &vectorToFill, const char* filename )
{
  std::ifstream inputFile( filename );  
  std::string   inputString;

  if ( !inputFile.is_open() )
  {
    std::cout << "Error opening msg " << filename << std::endl;
    return false;
  }

  while ( getline( inputFile, inputString ) )
  {
    vectorToFill.push_back( inputString );
  }
  
  inputFile.close();  

  return true;
}

// Called from load_data function to preload item data
bool load_item_data ( const char* filename )
{
  std::ifstream              inputFile( filename );  
  std::string                inputString, title, itemMsg;
  std::vector< std::string > keywords;
  int                        extractFlag, type;
  float                      power = 0;

  if ( !inputFile.is_open() )
  {
    std::cout << "Error opening item " << filename << std::endl;
    return false;
  }
  
  while ( getline( inputFile, inputString ) )
  {
     if ( inputString == "*TYP*" )
     {
        extractFlag = 0;
        continue;
     }
     else if ( inputString == "*POW*" )
     {
        extractFlag = 1;
        continue;
     }
     else if ( inputString == "*TTL*" )
     {
        extractFlag = 2;
        continue;
     }
     else if ( inputString == "*MSG*" )
     {
        extractFlag = 3;
        continue;
     }
     else if ( inputString == "*KWR*" )
     {
        keywords.clear();
        extractFlag = 4;
        continue;
     }
     else if ( inputString == "*FIN*" )
     {
        extractFlag = -1;
        Item tempItem( type, power, 1, title, itemMsg, keywords );
        itemVolume.push_back( tempItem );
        continue;
     }
     
     switch ( extractFlag )
    {
      case 0 :
        std::istringstream( inputString ) >> type;
        if ( type < 0 || type >= MAXSIZE_IT )
        {
          std::cout << "*Warning: Invalid item type defined in " << filename << std::endl;
          return false;
        }
        break;
      case 1 :
        std::istringstream( inputString ) >> power;
        break;
      case 2 :
        title = inputString;
        break;
      case 3 :
        itemMsg = inputString;
        break;
      case 4 :
        keywords.push_back( inputString );
        break;
    }
  }

  return true;
}

// Called from load_data function to preload enemy data
bool load_enemy_data ( const char* filename )
{
  std::ifstream              inputFile( filename );  
  std::string                inputString, title, startMsg, winMsg, loseMsg, picId;
  std::vector< Item >        inventory;
  int                        extractFlag, tempIndexA, tempIndexB;
  float                      maxHp, attack, block;

  if ( !inputFile.is_open() )
  {
    std::cout << "Error opening enemy " << filename << std::endl;
    return false;
  }
  
  while ( getline( inputFile, inputString ) )
  {
     if ( inputString == "*TTL*" )
     {
        extractFlag = 0;
        continue;
     }
     else if ( inputString == "*SMSG*" )
     {
        extractFlag = 1;
        continue;
     }
     else if ( inputString == "*WMSG*" )
     {
        extractFlag = 2;
        continue;
     }
     else if ( inputString == "*LMSG*" )
     {
        extractFlag = 3;
        continue;
     }
     else if ( inputString == "*PIC*" )
     {
        extractFlag = 4;
        continue;
     }
     else if ( inputString == "*HP*" )
     {
        extractFlag = 5;
        continue;
     }
     else if ( inputString == "*ATCK*" )
     {
        extractFlag = 6;
        continue;
     }
     else if ( inputString == "*BLCK*" )
     {
        extractFlag = 7;
        continue;
     }
     else if ( inputString == "*INV*" )
     {
        inventory.clear();
        tempIndexB  = -1;
        extractFlag = 8;
        continue;
     }
     else if ( inputString == "*FIN*" )
     {
        extractFlag = -1;
        Enemy tempEnemy( title, startMsg, winMsg, loseMsg, picId, maxHp, attack, block, inventory );
        enemyVolume.push_back( tempEnemy );
        continue;
     }
     
     switch ( extractFlag )
    {
      case 0 :
        title    = inputString;
        break;
      case 1 :
        startMsg = inputString;
        break;
      case 2 :
        winMsg   = inputString;
        break;
      case 3 :
        loseMsg  = inputString;
        break;
      case 4 :
        picId    = inputString;
        break;
      case 5 :
        std::istringstream( inputString ) >> maxHp;
        break;
      case 6 :
        std::istringstream( inputString ) >> attack;
        break;
      case 7 :
        std::istringstream( inputString ) >> block;
        break;
      case 8 :
        tempIndexA = get_item_index( inputString );
        tempIndexB = get_item_index( inputString, inventory );
        
        if ( tempIndexA == -1 )
        {
          std::cout << "*Warning: Invalid item name defined in " << filename << std::endl;
          return false;
        }
        else if ( tempIndexB != -1 )
          inventory[tempIndexB].quantity += 1;
        else
          inventory.push_back( itemVolume[tempIndexA] );
        
        break;
    }
  }

  return true;
}

bool load_action_data ( const char* filename, std::vector< Action > &actionVolume )
{
  std::ifstream              inputFile( filename );  
  std::string                inputString;
  std::vector< std::string > description, conA, conB, itemCon, endMsg;
  std::vector< int >         reward, rewardQ;
  int                        extractFlag, tempIndex, tempInt, type, enemy;

  if ( !inputFile.is_open() )
  {
    std::cout << "Error opening action " << filename << std::endl;
    return false;
  }
  
  while ( getline( inputFile, inputString ) )
  {
     if ( inputString == "*TYP*" )
     {
        description.clear();
        conA.clear();
        conB.clear();
        itemCon.clear();
        endMsg.clear();
        reward.clear();
        rewardQ.clear();
        extractFlag = 0;
        continue;
     }
     else if ( inputString == "*DESC*" )
     {
        extractFlag = 1;
        continue;
     }
     else if ( inputString == "*CONA*" )
     {
        extractFlag = 2;
        continue;
     }
     else if ( inputString == "*CONB*" )
     {
        extractFlag = 3;
        continue;
     }
     else if ( inputString == "*ITMCON*" )
     {
        extractFlag = 4;
        continue;
     }
     else if ( inputString == "*ENMSG*" )
     {
        extractFlag = 5;
        continue;
     }
     else if ( inputString == "*RWRD*" )
     {
        extractFlag = 6;
        continue;
     }
     else if ( inputString == "*RWRDQ*" )
     {
        extractFlag = 7;
        continue;
     }
     else if ( inputString == "*ENMY*" )
     {
        extractFlag = 8;
        continue;
     }
     else if ( inputString == "*FIN*" )
     {
        extractFlag = -1;
        Action tempAction( type, description, conA, conB, itemCon, endMsg, reward, rewardQ, enemy );
        actionVolume.push_back( tempAction );
        continue;
     }
     
     switch ( extractFlag )
    {
      case 0 :
        std::istringstream( inputString ) >> type;
        if ( type < 0 || type >= MAXSIZE_AT )
        {
          std::cout << "*Warning: Invalid action type defined in " << filename << std::endl;
          return false;
        }
        break;
      case 1 :
        description.push_back( inputString );
        break;
      case 2 :
        conA.push_back( inputString );
        break;
      case 3 :
        conB.push_back( inputString );
        break;
      case 4 :
        if ( get_item_index( inputString ) == -1 )
        {
          std::cout << "*Warning: Invalid action item-condition defined in " << filename << std::endl;
          return false;
        }
        else   
          itemCon.push_back( inputString );
        break;
      case 5 :
        endMsg.push_back( inputString );
        break;
      case 6 :
        std::istringstream( inputString ) >> tempInt;
        if ( type == CHEST && ( tempInt >= itemVolume.size() || tempInt < 0 ) )
        {
          std::cout << "*Warning: Invalid action item-reward defined in " << filename << std::endl;
          return false;
        }
        reward.push_back( tempInt );
        break;
      case 7 :
        std::istringstream( inputString ) >> tempInt;
        rewardQ.push_back( tempInt );
        break;
      case 8 :
        std::istringstream( inputString ) >> enemy;
        if ( type == FIGHT && ( enemy >= enemyVolume.size() || enemy < 0 ) )
        {
          std::cout << "*Warning: Invalid action enemy defined in " << filename << std::endl;
          return false;
        }
        break;
      default :
        break;
    }
  }
  
  return true;
}

// Used to extract info from data files and create instances of Cell objects
bool load_cell_data ( const char* filename, const char* actionFilename )
{
  std::ifstream              inputFile( filename );
  std::string                inputString, shDc, drLctns, lckM = "is a locked door";
  std::vector< std::string > tempVector;
  std::vector< Action >      cellActions, actionVolume;
  int                        x, y, extractFlag, tempIndex;
  bool                       lck;
  Vector2                    cd;
  
  // Temporarily load action data into memory
  if ( !load_action_data( actionFilename, actionVolume ) )
    return false;
  
  if ( !inputFile.is_open() )
  {
    std::cout << "Error opening cell " << filename << std::endl;
    return false;
  }
  
  while ( getline( inputFile, inputString ) )
  {
    if ( inputString == "*****X*****" )
    {
      cellActions.clear(); // New cell, so make sure this is empty
      extractFlag = 0;
      continue;
    }
    else if ( inputString == "*Y*" )
    {
      extractFlag = 1;
      continue;
    }
    else if ( inputString == "*LONGDESC*" )
    {
      tempVector.clear();
      extractFlag = 2;
      continue;
    }
    else if ( inputString == "*DOORDESC*" )
    {
      extractFlag = 3;
      continue;
    }
    else if ( inputString == "*LCK*" )
    {
      extractFlag = 4;
      continue;
    }
    else if ( inputString == "*LCKDESC*" )
    {
      extractFlag = 5;
      continue;	
    }
    else if ( inputString == "*ACCESS*" )
    {
      extractFlag = 6;
      continue;        	
    }
    else if ( inputString == "*ACTION*" )
    {
      extractFlag = 7;
      continue;
    }
    else if ( inputString == "*FIN*" )
    {
      extractFlag = -1;
      cd.set_values( x, y );
      Cell tempCell( cd, tempVector, shDc, lck, lckM, drLctns, cellActions );
      cellVolume.push_back( tempCell );
      continue;
    }

    switch ( extractFlag )
    {
      case 0 :
        std::istringstream( inputString ) >> x;
        break;
      case 1 :
        std::istringstream( inputString ) >> y;
        break;
      case 2 :
        tempVector.push_back( inputString );
        break;
      case 3 :
        shDc = inputString;
        break;
      case 4 :
        std::istringstream( inputString ) >> lck;
        break;
      case 5 :
        lckM = inputString;
        break;
      case 6 :
        drLctns = inputString;
        break;
      case 7 :
        std::istringstream( inputString ) >> tempIndex;
        if ( tempIndex >= 0 && tempIndex < actionVolume.size() )
          cellActions.push_back( actionVolume[tempIndex] );
        else
        {
          std::cout << "Warning: invalid action index defined in " << filename;
          std::cout << " in cell X: " << x << " Y: " << y << std::endl;
          return false;
        }
        break;
    }
  }

  inputFile.close();

  return true;
}

// External Function
// Called to preload all data files into memory
// Should be called as early as possible in main program
bool load_data ()
{
  const char*                filename       = DATGEN; // Filename that contains all the general info is defined in headers
  std::string                inputString, enemyFilename, itemFilename, cellFilename, actionFilename;
  std::ifstream              inputFile      ( filename );
  std::vector< std::string > messageFilenames;
  int                        extractFlag, tempIntA, tempIntB;

  if ( !inputFile.is_open() )
  {
    std::cout << "Error opening data " << filename << std::cout;
    return false;
  }

  while ( getline( inputFile, inputString ) )
  {
    if ( inputString == "*MSG*" )
    {
      extractFlag = 0;
      continue;
    }
    else if ( inputString == "*CEL*" )
    {
      extractFlag = 1;
      continue;
    }
    else if ( inputString == "*ITM*" )
    {
      extractFlag = 2;
      continue;	
    }
    else if ( inputString == "*ACT*" )
    {
      extractFlag = 3;
      continue;	
    }
    else if ( inputString == "*ENM*" )
    {
      extractFlag = 4;
      continue;
    }
    else if ( inputString == "*MOVEKEYWRDS*" )
    {
      extractFlag = 5;
      continue;
    }
    else if ( inputString == "*DIRECTIONKEYWRDS*" )
    {
      extractFlag = 6;
      continue;
    }
    else if ( inputString == "*STARTINDEX*" )
    {
      extractFlag = 7;
      continue;
    }
    else if ( inputString == "*INVENTORYMSG*" )
    {
      extractFlag = 8;
      continue;
    }
    else if ( inputString == "*MAXHP*" )
    {
      extractFlag = 9;
      continue;
    }
    else if ( inputString == "*DFLTATTACK*" )
    {
      extractFlag = 10;
      continue;
    }
    else if ( inputString == "*DFLTBLOCK*" )
    {
      extractFlag = 11;
      continue;
    }
    
    switch ( extractFlag )
    {
      case 0 :
        messageFilenames.push_back( inputString );
        break;
      case 1 :
        cellFilename = inputString;
        break;
      case 2 :
        itemFilename = inputString;
        break;
      case 3 :
        actionFilename = inputString;
        break;
      case 4 :
        enemyFilename = inputString;
        break;
      case 5 :
        moveKeywords.push_back( inputString );
        break;
      case 6 :
        dirKeywords.push_back( inputString );
        break;
      case 7 :
        std::istringstream( inputString ) >> startingIndex;
        break;
      case 8 :
        inventoryMsg = inputString;
        break;
      case 9 :
        std::istringstream( inputString ) >> gStartingHp;
        break;
      case 10 :
        std::istringstream( inputString ) >> gStartingAttack;
        break;
      case 11 :
        std::istringstream( inputString ) >> gStartingBlock;
        break;
    }
  }  

  inputFile.close();
  
  // Load message files into memory  
  for ( int i = 0; i < messageFilenames.size(); i++ )
  {
    filename = messageFilenames[i].c_str();    

    // Add a vector to messageVolumes vector for the filename
    std::vector < std::string > tempVector;
    msgVolumes.push_back( tempVector );
   
    // Then load the corresponding file's data into the vector
    if ( !load_message_data( msgVolumes[i], filename ) )
      return false; 
  }
  
  // Load item data into memory
  if ( !load_item_data( itemFilename.c_str() ) )
    return false;
    
  // Load item data into memory
  if ( !load_enemy_data( enemyFilename.c_str() ) )
    return false;
  
  // Load cell deta into memory
  if ( !load_cell_data( cellFilename.c_str(), actionFilename.c_str() ) )
    return false;

  return true;
}

/*******************************
Beggining of fight game functions
********************************/

// A special exit check if 'exit' is called during fight loop
bool fight_exit_check ()
{
  std::string input;
  
  std::cout << "\n***Warning: Game cannot be saved during fight. Any any unsaved progress will be lost!***" << std::endl;
  
  while ( true )
  {
    std::cout << "Are you sure you want to exit? (Yes or No) ";
    
    getline( std::cin, input );
    
    input = lowercase( input );
    
    if ( input == "yes" || input == "y" )
      return true;
    else if ( input == "no" || input == "n" )
      return false;
    else
      std::cout << "Invalid entry." << std::endl;
  }
}

// Called from main file if an active fight is found in the current cell
// Returns 1 if won, 0 if died, 2 if player wants to exit game
int fight ( int actionIndex, Player &playerRef )
{
  Enemy       enemyCopy                                = enemyVolume[ cellVolume[playerRef.index].actions[actionIndex].enemy ];
  std::string input;
  float       pAtck, pBlck, eAtck, eBlck, blockChance;
  float       eHpThreshold                             = ( enemyCopy.maxHp / 2.5 );
  float       pHpThreshold                             = ( playerRef.maxHp / 4 );
  int         potionIndex, enmPotionIndex, rewardIndex = -1;
  bool        playerBlocking                           = false; 
  bool        enemyBlocking                            = false;
  EnemyAction enemyFlag;
  
  // Begginning fight stuff
  std::cout << std::endl;
  
  display_message( PIC, enemyCopy.picId );
  
  for ( int i = 0; i < cellVolume[playerRef.index].actions[actionIndex].description.size(); i++ )
          std::cout << cellVolume[playerRef.index].actions[actionIndex].description[i] << std::endl;
  
  std::cout << "\n" << enemyCopy.title << ": " << enemyCopy.startMsg << std::endl;
  
  // Fight Loop
  while ( true )
  {
    playerRef.display_hp();
    
    std::cout << "***Actions***" << std::endl;
    std::cout << "'" << FC_ATCK << "'" << std::endl;
    std::cout << "'" << FC_BLCK << "'" << std::endl;
    std::cout << "'" << FC_HEAL << "'" << std::endl;
    std::cout << "'" << FC_INV << "'" << std::endl << std::endl;
    
    // Get valid fight action before continuing
    while ( true )
    {
      std::cout << "What's your next move? ";
    
      getline( std::cin, input );
  
      input = lowercase( input );
      
      if ( input == FC_HEAL )
      {
        // Check if there is something to heal with
        potionIndex = get_best_potion( playerRef.inventory );
        
        if ( potionIndex == -1 )
          std::cout << "\nYou have nothing to heal with in your " << inventoryMsg << "!\n\n";
        else 
          break;
      }
      else if ( input == FC_INV )
        display_inventory( playerRef.inventory );
      else if ( input == FC_EXIT )
      {
        if ( fight_exit_check() )
         return 2;
       else
         std::cout << std::endl;
      }
      else if ( input == FC_ATCK || input == FC_BLCK || input == FC_EXIT )
        break;
      else
        std::cout << "\nInvalid fight command.\n\n";
    }
    
    // Do command action
    if ( input == FC_ATCK )
    {
      pAtck = random_float( (playerRef.defaultAttack / 2), playerRef.defaultAttack, 2 );
      
      if ( enemyBlocking )
      {
        pAtck -= eBlck;
        
        if ( pAtck < 0 )
          pAtck = 0;
         
        enemyBlocking = false;
      }
      
      std::cout << "\n***You deal " << pAtck << " points of damage to " << enemyCopy.title << "!***\n";
      
      // hurt_enemy returns false if enemy is dead
      if ( !enemyCopy.hurt_enemy( pAtck ) )
      {  
        // Pause before enemy dies
        for ( int i = 0; i < 3; i++ )
        {
          std::cout << "..." << std::endl;
          usleep(400000);
        }       
        // Print end message for enemy plus notification
        std::cout << "\n" << enemyCopy.title << ": " << enemyCopy.winMsg << std::endl;
        std::cout << "\n***" << enemyCopy.title << " has died***\n\n";
        break;
      }
    }
    else if ( input == FC_BLCK )
    {
      pBlck          = random_float( (playerRef.defaultBlock / 2), playerRef.defaultBlock, 2 );
      playerBlocking = true;
      std::cout << "\n***You are now blocking for the next turn!***\n";
    }
    else if ( input == FC_HEAL )
      use_potion( potionIndex, playerRef ); // Potion index was already checked for validity in the get command loop
    
    // Pause before enemy's turn
    for ( int i = 0; i < 3; i++ )
    {
      std::cout << "..." << std::endl;
      usleep(400000);
    }
    
    // Enemy turn
    enmPotionIndex = get_best_potion( enemyCopy.inventory );
    blockChance    = random_float( 1, 100 );
    
    // Decide what to do
    if ( enemyCopy.currentHp > eHpThreshold || playerRef.currentHp < pHpThreshold )
      enemyFlag = ATTACK;
    else if ( enmPotionIndex != -1 )
      enemyFlag = HEAL;
    else if ( blockChance < 35 )
      enemyFlag = BLOCK;
    else
      enemyFlag = ATTACK;
    
    // Run enemy action
    if ( enemyFlag == ATTACK )
    {
      eAtck = random_float( (enemyCopy.attack / 2), enemyCopy.attack, 2 );
      
      if ( playerBlocking )
      {
        eAtck -= pBlck;
        
        if ( eAtck < 0 )
          eAtck = 0;
         
        playerBlocking = false;
      }
      
      std::cout << "\n***" << enemyCopy.title << " deals " << eAtck << " points of damage to you!***\n";
      
      // hurt_player returns false if player is dead
      if ( !playerRef.hurt_player( eAtck ) )
      {  
        std::cout << "\n" << enemyCopy.title << ": " << enemyCopy.loseMsg << std::endl;
        std::cout << "\n***" << enemyCopy.title << " has killed you....***\n\n";
        std::cout << "Returning you to last save..." << std::endl;
        std::cout << "Press enter to continue...";
        std::cin.get();
        return 0;
      }
    }
    else if ( enemyFlag == BLOCK )
    {
      eBlck          = random_float( (enemyCopy.block / 2), enemyCopy.block, 2 );
      enemyBlocking  = true;
      std::cout << "\n***" << enemyCopy.title << " is now blocking for next turn!***\n";
    }
    else if ( enemyFlag == HEAL )
      use_potion( enmPotionIndex, enemyCopy );
      
    // Pause after enemy's turn
    for ( int i = 0; i < 3; i++ )
    {
      std::cout << "..." << std::endl;
      usleep(400000);
    }
  }

  // ***Stuff that happens at the end of the fight if won
  
  // Print action end message
  for ( int i = 0; i < cellVolume[playerRef.index].actions[actionIndex].endMsg.size(); i++ )
          std::cout << cellVolume[playerRef.index].actions[actionIndex].endMsg[i] << std::endl;

  // Give any action reward items
  for ( int i = 0; i < cellVolume[playerRef.index].actions[actionIndex].reward.size(); i++ )
  {
    rewardIndex = cellVolume[playerRef.index].actions[actionIndex].reward[i];
      
    // Add item to inventory, if no quantity is defined, default to 1
    if ( i < cellVolume[playerRef.index].actions[actionIndex].rewardQ.size() )
      add_item( playerRef, rewardIndex, cellVolume[playerRef.index].actions[actionIndex].rewardQ[i] );
    else
      add_item( playerRef, rewardIndex );
  }

  if ( rewardIndex == -1 )
    std::cout << std::endl;

  // Set action to false
  playerRef.overlay[playerRef.index].actionActive[actionIndex] = false;
  
  std::cout << "Press enter to continue...";
  std::cin.get();
  
  return 1;
}

/*******************************
Beggining of main game functions
********************************/

// Check if there is a fight action in this cell before displaying room info
int check_for_fight ( Player &playerRef )
{
  for ( int i = 0; i < cellVolume[playerRef.index].actions.size(); i++ )
  {
    // Check if the action is a still active fight action
    if ( cellVolume[playerRef.index].actions[i].type == FIGHT && playerRef.overlay[playerRef.index].actionActive[i] )
       return i;
  }
  
  return -1;
}

int check_for_cell ( Vector2& toCheck, std::string direction )
{
  int index = 64;
  
  for ( int i = 0; i < cellVolume.size(); i++ )
  {
    if ( cellVolume[i].cord == toCheck )
      index = i;
  }
  
  // This checks if the cell found has a proper door location specified
  if ( index != 64 && contains( direction, cellVolume[index].doorLocations ) )
  {
    return index;
  }
  else
    return 64;
}

int check_for_cell ( Vector2& toCheck )
{
  int index = 64;
  
  for ( int i = 0; i < cellVolume.size(); i++ )
  {
    if ( cellVolume[i].cord == toCheck )
      index = i;
  }
  
  return index;
}

void get_surrounding_cells( Player &playerRef )
{
  Vector2 toCheck;
  
  // North
  toCheck.set_values( cellVolume[playerRef.index].cord.x, cellVolume[playerRef.index].cord.y + 1 );
  playerRef.northIndex = check_for_cell( toCheck, "S" );

  // South
  toCheck.set_values( cellVolume[playerRef.index].cord.x, cellVolume[playerRef.index].cord.y - 1 );
  playerRef.southIndex = check_for_cell( toCheck, "N" );

  // East
  toCheck.set_values( cellVolume[playerRef.index].cord.x + 1, cellVolume[playerRef.index].cord.y );
  playerRef.eastIndex = check_for_cell( toCheck, "W" );

  // West
  toCheck.set_values( cellVolume[playerRef.index].cord.x - 1, cellVolume[playerRef.index].cord.y );
  playerRef.westIndex = check_for_cell( toCheck, "E" );
}

// Called from display_cell_info to show any active action descriptions for the current cell
void display_actions ( Player &playerRef )
{
  for ( int i = 0; i < cellVolume[playerRef.index].actions.size(); i++ )
  {
     // Check if cell should be active for current player's game state
     if ( playerRef.overlay[playerRef.index].actionActive[i] )
     {
        for ( int j = 0; j < cellVolume[playerRef.index].actions[i].description.size(); j++ )
          std::cout << cellVolume[playerRef.index].actions[i].description[j] << std::endl;
     }
  }
  
  std::cout << std::endl;
}

// When an instruction matches a cell action's condition, this performs the action based on its type
int complete_action ( int a, int b, int actionIndex, Player &playerRef )
{
  int  cellIndex, itemIndex = -1, result = 1;
  
  // UNLOCK action unlocks a defined cell(s)
  if ( cellVolume[playerRef.index].actions[actionIndex].type == UNLOCK )
  {   
    for ( int i = 0; i < cellVolume[playerRef.index].actions[actionIndex].itemCon.size(); i++ )
    {  
      // Check if player has item
      itemIndex = get_item_index( cellVolume[playerRef.index].actions[actionIndex].itemCon[i], playerRef.inventory );
      
      // If so, remove item
      if ( itemIndex != -1 )
        remove_item( playerRef, false, itemIndex, 1 );
      else
      {
        std::cout << "\n*** You don't have the right item to do that ***" << std::endl << std::endl;
        return 1;
      }
    }
    
    if ( itemIndex == -1 )
      std::cout << std::endl;
    
    for ( int i = 0; i < cellVolume[playerRef.index].actions[actionIndex].reward.size(); i++ )
	{
      cellIndex = cellVolume[playerRef.index].actions[actionIndex].reward[i];
     
      if ( cellIndex >= 0 && cellIndex < playerRef.overlay.size() )
        playerRef.overlay[cellIndex].locked = false;
      else
      {
         std::cout << "Invalid cell reward index in actions data!\n\n" << std::endl;
         return 1;
      }
    }
  }
  // CHEST action type gives the play an item(s)
  else if ( cellVolume[playerRef.index].actions[actionIndex].type == CHEST )
  {
	for ( int i = 0; i < cellVolume[playerRef.index].actions[actionIndex].reward.size(); i++ )
	{
      itemIndex = cellVolume[playerRef.index].actions[actionIndex].reward[i];
      
      // Add item to inventory, if no quantity is defined, default to 1
      if ( i < cellVolume[playerRef.index].actions[actionIndex].rewardQ.size() )
        add_item( playerRef, itemIndex, cellVolume[playerRef.index].actions[actionIndex].rewardQ[i] );
      else
        add_item( playerRef, itemIndex );
    }
  }
  // END action type will signal the main code file to do its specific end game tasks
  else if ( cellVolume[playerRef.index].actions[actionIndex].type == END )
  {
    for ( int i = 0; i < cellVolume[playerRef.index].actions[actionIndex].itemCon.size(); i++ )
    {
      // Check if player has item
      itemIndex = get_item_index( cellVolume[playerRef.index].actions[actionIndex].itemCon[i], playerRef.inventory );
      
      // If so, remove item
      if ( itemIndex == -1 )
      {
        std::cout << "\n*** You don't have the right item to do that ***" << std::endl << std::endl;
        return 1;
      }
    }   
    
    std::cout << std::endl;
    
    result = 2;
  }
   
  // Display action's end message
  for ( int i = 0; i < cellVolume[playerRef.index].actions[actionIndex].endMsg.size(); i++ )
    std::cout << cellVolume[playerRef.index].actions[actionIndex].endMsg[i] << std::endl;
  
  std::cout << std::endl;
  
  // Deactivate action
  playerRef.overlay[playerRef.index].actionActive[actionIndex] = false;
  
  return result;
}

// Called either at the end of making a move or when a player types the "look around" command
void display_cell_info ( Player &playerRef )
{ 
  // Check if there is an active fight action in cell before displaying room info
  if ( check_for_fight( playerRef ) != -1 )
    return;

  // Gets the index of any surrounding cells
  get_surrounding_cells( playerRef );

  // Display map of area with current cell highlighted
  display_map( playerRef.index );

  // Display the main description of the current cell
  for ( int i = 0; i < cellVolume[playerRef.index].longDesc.size(); i++ )
  {
    std::cout << cellVolume[playerRef.index].longDesc[i] << std::endl;
  }
  
  // Displays the description of any active (non-fight) actions
  display_actions( playerRef );

  if ( playerRef.northIndex != 64 )
  {
    if ( !playerRef.overlay[playerRef.northIndex].locked )
      std::cout << "-In front of you " << cellVolume[playerRef.northIndex].shortDesc << std::endl;
    else
      std::cout << "-In front of you " << cellVolume[playerRef.northIndex].lockedMsg << std::endl;
  }
  
  if ( playerRef.southIndex != 64 )
  {
    if ( !playerRef.overlay[playerRef.southIndex].locked )
      std::cout << "-Behind you " << cellVolume[playerRef.southIndex].shortDesc << std::endl;
    else
      std::cout << "-Behind you " << cellVolume[playerRef.southIndex].lockedMsg << std::endl;
  } 
  
  if ( playerRef.eastIndex != 64 )
  {
    if ( !playerRef.overlay[playerRef.eastIndex].locked )
      std::cout << "-To your right " << cellVolume[playerRef.eastIndex].shortDesc << std::endl;
    else
      std::cout << "-To your right " << cellVolume[playerRef.eastIndex].lockedMsg << std::endl; 
  }

  if ( playerRef.westIndex != 64 )
  {
    if ( !playerRef.overlay[playerRef.westIndex].locked )
      std::cout << "-To your left " << cellVolume[playerRef.westIndex].shortDesc << std::endl;
    else
      std::cout << "-To your left " << cellVolume[playerRef.westIndex].lockedMsg << std::endl;
  }

  std::cout << std::endl;
}

// When the player has issued a move command, this moves the player to the cell so long as there
// is a cell in the direction theyre trying to move and it is unlocked
void make_a_move ( int direction, Player &playerRef )
{
  // This is done to allow multiple keyword groups for directions
  // As long as they were listed in the right order in the gamedat00 file,
  // This will always get the right order no matter how many groups there are
  while ( direction > 3 )
    direction -= 4;
  
  if ( direction == 0 && playerRef.northIndex != 64 )
  {
    if ( !playerRef.overlay[playerRef.northIndex].locked )
    {
      playerRef.index = playerRef.northIndex;
      display_cell_info ( playerRef );
    }
    else
      std::cout << "\nYou can't go this way yet.\n\n";
  }
  else if ( direction == 1 && playerRef.southIndex != 64 )
  {
    if ( !playerRef.overlay[playerRef.southIndex].locked )
    {
      playerRef.index = playerRef.southIndex;
      display_cell_info ( playerRef );
    }
    else
      std::cout << "\nYou can't go this way yet.\n\n";
  }
  else if ( direction == 2 && playerRef.eastIndex != 64 )
  {
    if ( !playerRef.overlay[playerRef.eastIndex].locked )
    {
      playerRef.index = playerRef.eastIndex;    
      display_cell_info ( playerRef );
    }
    else
      std::cout << "\nYou can't go this way yet.\n\n";
  }
  else if ( direction == 3 && playerRef.westIndex != 64 )
  {
    if ( !playerRef.overlay[playerRef.westIndex].locked )
    {
      playerRef.index = playerRef.westIndex;
      display_cell_info ( playerRef );
    }
    else
      std::cout << "\nYou can't go this way yet.\n\n";
  }
  else
    std::cout << "\nYou can't go in that direction...\n\n";
}

// External function
// Called from the main file to read user instruction and delegate the command elsewhere
int parse_instruction ( std::string input, Player &playerRef )
{
  std::vector< std::string >  subString;
  std::string                 originalInput = input; 
  std::istringstream          iss( input );
  int                         index, tempInt;
 
  if ( input == GC_LOOK )
  {
    display_cell_info( playerRef );
    return 1;
  }
  else if ( input == GC_INV )
  {
    display_inventory( playerRef.inventory );
    return 1;
  }
  else if ( input == GC_HP )
  {
    playerRef.display_hp();
    return 1;
  }
 
  // Split string by word
  while ( iss >> input )
    subString.push_back( input );

  // Check if input contains a move keywords
  index = compare_vectors( subString, moveKeywords );   

  if ( index != -1 )
  {
    // Check if input also has a valid direction
    index = compare_vectors( subString, dirKeywords );
    
    if ( index != -1 )
    {  
      // If everything checks out, move the player
      make_a_move( index, playerRef );
      return 1;
    }
  }
  
  // Check if input meets active action conditions
  for ( int i = 0; i < cellVolume[playerRef.index].actions.size(); i++ )
  {
    // Check if the action is a still active
    if ( playerRef.overlay[playerRef.index].actionActive[i] )
    {
       int conA   = compare_vectors( subString, cellVolume[playerRef.index].actions[i].conditionA );
       int conB   = compare_vectors( subString, cellVolume[playerRef.index].actions[i].conditionB );
       int result = 0;
       
       if ( conA != -1 && conB != -1 )
       {
         result = complete_action( conA, conB, i, playerRef );
         return result;
       }
    }
  }
  
  // Finally, check if it is a use item command and the player has that item
  for ( int i = 0; i < subString.size(); i++ )
  {
     index = get_item_index( subString[i], playerRef.inventory );
     
     if ( index != -1 )
       break;
  }
  
  if ( index != -1 && !contains( "examine", originalInput ) )
  {
    if ( playerRef.inventory[index].type != POTION )
    {
      std::cout << "\nIt doesn't do anything...\n\n";
      return 1;
    }
    
    tempInt = compare_vectors( subString, playerRef.inventory[index].keywords );
    
    if ( tempInt != 1 )
    {
      use_potion( index, playerRef );
      return 1;
    }
  }
  else if ( contains( "examine", originalInput ) )
  {
	if ( index != -1 )
	{
	  std::cout <<  "\n" << playerRef.inventory[index].itemMsg << "\n\n";
	  return 1;
	}
	else
	{
      // Check if item exists in game at all
      for ( int i = 0; i < subString.size(); i++ )
      {
        index = get_item_index( subString[i] );
      
        if ( index != -1 )
        {
          std::cout <<  "\nYou don't currently have this item!\n\n";
	      return 1;
        }
      } 
    }
  }
  
  return 0;
}
