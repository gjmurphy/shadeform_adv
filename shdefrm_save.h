// ShadeForm Save Tool - Header
// Gabriel Murphy - 2014
#ifndef _SAVE_INCLUDED
#define _SAVE_INCLUDED

#include<string>

Player load_save_data(int);

void   save_game(int, Player &playerRef);

void remove_entry(std::string &userId);

#endif
