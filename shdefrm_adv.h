// ShadeForm Adventure Toolkit
// Gabriel Murphy - 2014
#ifndef _ADV_INCLUDED
#define _ADV_INCLUDED

#include<string>
#include<vector>

// Common game commands
#define GC_LOOK "look around"
#define GC_INV  "inventory"
#define GC_HP   "check health"

// Fight commands
#define FC_ATCK "attack"
#define FC_BLCK "block"
#define FC_HEAL "heal"
#define FC_INV  "inventory"
#define FC_EXIT "exit"

// General data, this file holds misc setup prefs and filenames
#define DATGEN "gamedat00.wed"

/****************************************************
Enumeration for selecting which message volume to use.
This MUST match the ORDER and QUANTITY of the files
denoted under *MSG* in your data solutions file.
(gamedat00.wed unless changed otherwise)
****************************************************/
enum Volume { PIC, MSG };

// Enumeration for item type
enum ItemType { TOOL, POTION, WPN, COLLECTABLE, MAXSIZE_IT };

// Enumeration for action type
enum ActionType { UNLOCK, CHEST, FIGHT, END, MAXSIZE_AT };

// Enumeration used for enemy turn in fight
enum EnemyAction { ATTACK, BLOCK, HEAL };

class Vector2
{

public:

	int x;
	int y;

	Vector2(int x_value = 0, int y_value = 0)
	{
		x = x_value;
		y = y_value;
	}

	void set_values(int x_value = 0, int y_value = 0)
	{
		x = x_value;
		y = y_value;
	}

	friend bool operator ==(const Vector2 &left, const Vector2 &right);

	Vector2 operator =(Vector2 toSet)
	{
		x = toSet.x;
		y = toSet.y;

		return *this;
	}
};

class Item
{

public:

	ItemType                   type;
	int                        quantity;
	float                      power;
	std::string                title, itemMsg;
	std::vector< std::string > keywords;

	Item(int tp, float p, int q, std::string tl, std::string m, std::vector< std::string > &k)
	{
		type = (ItemType)tp;
		power = p;
		quantity = q;
		title = tl;
		itemMsg = m;
		keywords = k;
	}

	Item() {};

	void increase_quantity(int amount) { quantity += amount; }

	void decrease_quantity(int amount) { quantity -= amount; }

};

class Enemy
{

public:

	std::string          title, startMsg, winMsg, loseMsg, picId;
	float                currentHp, maxHp, attack, block;
	std::vector< Item >  inventory;

	Enemy(std::string t, std::string sm, std::string wm, std::string lm,
		std::string pid, float mhp, float ak, float bk, std::vector< Item > inv)
	{
		title = t;
		startMsg = sm;
		winMsg = wm;
		loseMsg = lm;
		picId = pid;
		maxHp = mhp;
		currentHp = maxHp;
		attack = ak;
		block = bk;
		inventory = inv;
	}

	Enemy() {};

	bool hurt_enemy(float);

	void heal_enemy(float);

	Enemy operator =(Enemy toSet)
	{
		title = toSet.title;
		startMsg = toSet.startMsg;
		winMsg = toSet.winMsg;
		loseMsg = toSet.loseMsg;
		maxHp = toSet.maxHp;
		currentHp = maxHp;
		inventory = toSet.inventory;

		return *this;
	}
};

class Action
{

public:

	ActionType                 type;
	std::vector< std::string > description, conditionA, conditionB, itemCon, endMsg;
	std::vector< int >         reward, rewardQ;
	int                        enemy;

	Action(int tp, std::vector< std::string > dc, std::vector< std::string > ca, std::vector< std::string > cb, // And 
		std::vector< std::string > ic, std::vector< std::string > em, std::vector< int > r, // And
		std::vector< int > rq, int en)
	{
		type = (ActionType)tp;
		description = dc;
		conditionA = ca;
		conditionB = cb;
		itemCon = ic;
		endMsg = em;
		reward = r;
		rewardQ = rq;
		enemy = en;
	}

	Action() {};

};

class Cell
{

public:

	Vector2                    cord;
	std::vector< std::string > longDesc;
	std::string                shortDesc, lockedMsg, doorLocations;
	bool                       locked;
	std::vector< Action >      actions;

	Cell(Vector2 cd, std::vector< std::string > ld, std::string sd, bool lk,
		std::string lm, std::string drLc, std::vector< Action > ac)
	{
		cord = cd;
		longDesc = ld;
		shortDesc = sd;
		locked = lk;
		lockedMsg = lm;
		doorLocations = drLc;
		actions = ac;
	}

	Cell() {};

};

class CellOverlay
{

public:

	bool                locked;
	std::vector< bool > actionActive;

	CellOverlay(bool lck, std::vector< bool > aa)
	{
		locked = lck;
		actionActive = aa;
	}

	CellOverlay() {};

};

class Player
{

public:

	int index, northIndex, southIndex, eastIndex, westIndex, userId;
	float                      currentHp, maxHp, defaultAttack, defaultBlock;
	std::vector< Item >        inventory;
	std::vector< CellOverlay > overlay;

	Player(int idx, std::vector< Item > inv, std::vector< CellOverlay > ovr, float chp, float mhp, float da, float db, int id)
	{
		index = idx;
		inventory = inv;
		overlay = ovr;
		currentHp = chp;
		maxHp = mhp;
		defaultAttack = da;
		defaultBlock = db;
		userId = id;
	}

	Player() {};

	bool hurt_player(float);

	bool heal_player(float);

	void display_hp();

};

extern std::vector< Cell >   cellVolume;
extern std::vector< Item >   itemVolume;
extern int                   startingIndex;
extern float                 gStartingHp, gStartingAttack, gStartingBlock;

void display_message(Volume, std::string);

void display_game_help();

void display_inventory(std::vector< Item > &invRef);

bool load_data();

int fight(int, Player &playerRef);

int check_for_fight(Player &playerRef);

int check_for_cell(Vector2&, std::string);

int check_for_cell(Vector2&);

int parse_instruction(std::string, Player &playerRef);

#endif
