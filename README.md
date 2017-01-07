# shadeform_adv
During my first semester in a CS program, I was already familiar with the very basics of programming but not with C++.
About halfway through the semester, one of the assignments involved coding a very basic login type program that
saved the data to a file (to demonstrate basic file IO). I started building on it to create a text based adventure
game, and in subsequent assignments asked the professor if I could just incorporate the concepts of each assignment
and submit that instead. He agreed and this is the end result. It did get a bit out of hand towards the end in terms
of feature-creep, but I think it turned out pretty cool. 

The .wed files contain all the game data and currently hold a simple example game as an example..
Though not designer-friendly, this framework could be used to create any text based adventure without modifying
a single line of code--all game design is held in the specific .wed data files. Features include auto generated
maps based on the cells defined in the gamedat03.wed file, parsing user commands (actions) based on keywords
defined by the designer so that natural language can be used (rather than a strict command syntax), multiple users
and game saves system, an item inventory system, and a very basic turn-based battle system for enemy encounters.

Will possibly write full documentation and refactor......someday.

Basic file info:

gamedat00.wed : File name cannot be changed. Defines the file names for the other data filenames, basic keywords,
starting cell index, default HP (health points) etc..

The other file names can be changed but must match the name specified in gamedat00.wed
In the current example game:
Message data: gamedat01.wed and gamedat02.wed (first is for ascii art, second for intro message, story setup)
Game cell data: gamedat03.wed
Item data: gamedat04.wed
Actions (each cell contains 0 or more actions, can be reused) data: gamedat05.wed
Enemy data: gamedat06.wed
