// ShadeForm Misc Tools
// Gabriel Murphy - 2014
#include"shdefrm_adv.h"
#include"shdefrm_tools.h"
#include<iostream>
#include<iomanip>                                                              
#include<sstream>
#include<fstream>
#include<stdlib.h>
#include<time.h>

// Converts all upper case letters to lowercase
std::string lowercase ( std::string input )
{
    std::string converted = input;

    for ( int i = 0; i < input.length(); i++ )
    {
      if ( (int) input[i] >= 65 && (int) input[i] <= 90 )
        converted[i] = (int) input[i] + 32;
    } 

    return converted;
}

// Gets a valid number
double validnumber ( std::string query )
{
  std::string input;
  float       tempF = 0;

  while ( true )
  {
    std::stringstream converter;
    
    std::cout << query;
    getline( std::cin, input );
    
    converter << input;
    converter >> tempF;

    if ( tempF == 0 && input != "0" )
      std::cout << "Invalid entry." << std::endl;
    else break;
  }
  return tempF;  
}

// Gets a valid number with inclusive limits
double validnumber ( double min, double max, std::string query, std::string error )
{
  std::string input;
  float       tempF = 0;

  while ( true )
  {
    std::stringstream converter;
    
    if ( query == "FLAG^" )
      std::cout << std::fixed << std::setprecision(0) << "Enter a number between " << min << " and " << max << ": "; 
    else
      std::cout << query;
    
    getline( std::cin, input );
    
    converter << input;
    converter >> tempF;

    if ( tempF == 0 && input != "0" )
    {  
      std::cout << "Invalid entry." << std::endl;
      continue;
    }
    
    if ( tempF < min || tempF > max )
    {
      if ( error == "FLAG^" )
      {
        std::cout << std::fixed << std::setprecision(0);
        std::cout << "Invalid entry. Number should be between " << min << " and " << max << ".\n";
      }
      else
        std::cout << error;
      continue;
    }

    break;
  }
  return tempF;
}

// Builds a map from the cell data, denoting player's current cell
void display_map ( int playerIndex )
{
  int                      indexTracker = -1, maxX = 0, maxY = 0;
  bool                     inThisRow, doorEast, doorWest;
  Vector2                  currentCord, playerCord;
  std::vector<int>         currentRowXs;
  std::vector<std::string> map, doorLocations;
  std::string              thisRow, cellBottom = "|_____|", cellMiddle = "|     |", cellTop = "|'''''|", currentCell = "| <o> |";
  std::string              cellDrWest = "      |", cellDrEast = "|      ", cellDrNorth = "|'' ''|", cellDrSouth = "|_   _|", noCell = "       ";
  std::string              curCellDrWest = "  <o> |", curCellDrEast = "| <o>  ", curCellDrEstWst = "  <o>  ";

  for ( int i = 0; i < cellVolume.size(); i++ )
  {
    if ( i == playerIndex )
      playerCord = cellVolume[i].cord; 

    if ( cellVolume[i].cord.y > maxY )
      maxY = cellVolume[i].cord.y;

    if ( cellVolume[i].cord.x > maxX )
      maxX = cellVolume[i].cord.x;
  }

  maxY++;
  maxX++;

  for ( int currentY = 0; currentY < maxY; currentY++ )
  {
    currentRowXs.clear();
    
    for ( int i = 0; i < cellVolume.size(); i++ )
    {
      if ( cellVolume[i].cord.y == currentY )
        currentRowXs.push_back( cellVolume[i].cord.x );
    }

    for ( int cell = 0; cell < 3; cell++ )
    {
      thisRow = "";

      for ( int currentX = 0; currentX < maxX; currentX++ )
      {                                                                                         
        currentCord.set_values( currentX, currentY );

        if ( cell == 1 && currentCord == playerCord )
        {  
          doorWest = ( check_for_cell( currentCord, "W" ) != 64 );
          doorEast = ( check_for_cell( currentCord, "E" ) != 64 );
        
          if ( doorWest && doorEast )
            thisRow += curCellDrEstWst;
          else if ( doorEast )
            thisRow += curCellDrEast;
          else if ( doorWest )
            thisRow += curCellDrWest;
          else
            thisRow += currentCell;
          
          continue;
        }

        inThisRow = ( check_for_cell( currentCord ) != 64 );
        
        switch ( cell )
        {
          case 0 : 
            if ( inThisRow )
            {
              if ( check_for_cell( currentCord, "S" ) != 64 )
                thisRow += cellDrSouth;
              else
                thisRow += cellBottom;
            }
            else
              thisRow += noCell;
            break;
          case 1 :
            if ( inThisRow )
            {
              doorWest = ( check_for_cell( currentCord, "W" ) != 64 );
              doorEast = ( check_for_cell( currentCord, "E" ) != 64 );

              if ( doorWest && doorEast )
                thisRow += noCell;
              else if ( doorEast )
                thisRow += cellDrEast;
              else if ( doorWest )
                thisRow += cellDrWest;
              else
                thisRow += cellMiddle;
            }
            else
              thisRow += noCell;
            break;
          case 2 :
            if ( inThisRow )
            {
              if ( check_for_cell( currentCord, "N" ) != 64 )
                thisRow += cellDrNorth;
              else
                thisRow += cellTop;
            }
            else
              thisRow += noCell;
            break;
        }

      }
    
      map.push_back( thisRow );
    }
  }

  std::cout << std::endl;

  for ( int i = map.size() - 1; i >= 0; i-- )
    std::cout << map[i] << std::endl; 

  std::cout << std::endl;

}

// Counts the number of lines in a file
int linecount ( const char* filename )
{
  std::ifstream inputFile;
  int           numOfLines = 0;
  std::string   tempString;

  do { inputFile.open( filename ); } while ( !inputFile.is_open() );

  while ( getline( inputFile, tempString ) )
    ++numOfLines;

  do { inputFile.close(); } while ( inputFile.is_open() );  

  return numOfLines;
}

// Used for copying a temporary file into a a permanant file
void copytemp ( const char* to, const char* from )
{
  std::ifstream inputFile;
  std::ofstream outputFile;

  // Deletes content of "To" file
  do {
   inputFile.open( to, std::ofstream::out | std::ofstream::trunc );
  } while ( !inputFile.is_open() );

  do { inputFile.close(); } while ( inputFile.is_open() );

  // Open files in binary mode
  do { inputFile.open( from, std::ios::binary ); } while ( !inputFile.is_open() );
  do { outputFile.open( to, std::ios::binary ); } while ( !outputFile.is_open() );
  
  // Copy the contents of the temp file to userdata.wed
  outputFile << inputFile.rdbuf();

  inputFile.close();
  outputFile.close();

  // Delete temp file from disk
  remove( from );
}

// Multipurpose function that checks if a string contains any spaces
bool has_spaces ( std::string toCheck  )
{
  for ( int i = 0; i < toCheck.size(); i++ )
  {
    if ( toCheck[i] == ' ' )
      return true;
  }
  
  return false;
}

// Random Float Generator
float random_float ( float min, float max )
{  
  float number;

  srand( time(NULL) );

  // TODO this needs work. Min and max lose any decimal parts sent in args
  number = min + ( rand() % ( (int) max - (int) min + 1 ));
  
  return number;
}

// Random Float Generator with ability to specify number of decimal
float random_float ( float min, float max, int decimalPlaces )
{  
  float number, power = 1;
 
  for ( int i = 0; i < decimalPlaces; i++ )
    power *= 10;
  
  max *= power;
  min *= power;
  
  srand( time(NULL) );
  
  // TODO this needs work. Min and max lose any decimal parts sent in args
  number = min + ( rand() % ( (int) max - (int) min + 1 ));

  return number / power;
}
