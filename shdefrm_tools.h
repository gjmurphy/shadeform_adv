// ShadeForm Misc Tools - Header
// Gabriel Murphy - 2014
#ifndef _TOOLS_INCLUDED
#define _TOOLS_INCLUDED

#include<string>

// Convert any upercase letters to lowercase
std::string lowercase ( std::string );

// Get a valid number from user
double validnumber ( std::string query = "Enter a number: " );
// With limits, inclusive
double validnumber ( double, double, std::string query = "FLAG^", std::string error = "FLAG^" );

void display_map ( int );

// Counts the number of lines in a file
int linecount ( const char* );

// Used for copying a temporary file into a a permanant file
void copytemp ( const char* to, const char* from );

// Multipurpose function that checks if a string contains any spaces
bool has_spaces ( std::string );

// Random Float Generator
float random_float ( float, float );

// Random Float Generator with ability to specify number of decimal
float random_float ( float, float, int );

#endif
