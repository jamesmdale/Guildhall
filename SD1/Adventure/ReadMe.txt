
How to Use:

When Adventure begins on an attract screen. Once the game begins the player will  be spawned on a randomly generated map with enemies nearby.
Items will be spawned on the map that are interactible. Portals will also be on the map and the player may navigate through them to travel back and forth
between different maps.

The following keys will have the listed actions:

ESC: Playing->Pause, Pause->Attract, Attract->Quit, Dead->Attract
M: Full Mini Map View
W,A,S,D & UP Arrow, Left Arrow, Down Arrow, and Right arrow to move
H: +99999 health
E: pickup item - if item is of type of a slot you already have filled, drop the item currently occupying the slot and pickup and equip the new one
I: print current player inventory in debugger
P: print current player equipment items in inventory
F: Attempt to use potion. Will heal if player has one in inventory. Will not heal beyond maximum life total

Space: attack



______________________________________________________________________________________________________________________________________________________________

Enemies will shoot at the player if they have line of sight. 


KNOWN BUGGS:
	Incomplete from A8:
	Victory conditions are not in but nonfunctional.
	Attract screens and logic is in, but screens do not transition.
	Text is in but unused and in box is not in.
	
ITEMS COMPLETED:

glDrawArrays
Two Sprites per tile
Entity Sorted draw order
Actor-vs-Actor disc corrective physics with mass calculations
Tags on tiles and entities
TileExtraInfo Structure
Heatmap
Stats (includes a damage, strength, and armor modifiers)
Items also have stats that are accounted for in combat
Combat fully featured
Items (with affected stats)
Inventory and Equipment (get, drop, equip)
MapGenStep_SpawnActor
MapGenStep_SpawnItem
MapGenStep_CellularAutomata
MapGgenStep_Tags

Custom Features
- player actor sprites show visibily the gear that they are wearing.
- Made consummables that can be used via a hotkey. ex: if you have a potion in your inventory, you can use it to increase your health.
- Attack and movement animations


Deep Learning:

	For this assignment I felt less in the way of building a fully formed game so much as making a proof of concept technology.  There are many more features here I'd like to explore and there is still work from A8
that I need to go back and flesh out/polish more.  This assignment was the first where I felt comfortable across the board with the concepts and the code required to make everything.  I didn't experiment with UI, but 
everything else seemed very doable given a longer amount of time.

  I feel increasingly better about my code quality from previous assignments, although if I could start adventure over, I would certainly refactor large portions
after having my discussion with Squirrel in office hours for my code review.  Specifically, using erase(vector.begin() + index) was a very valuable tool that I wish I had known before for deleting data.  One experiment I did for this
assignment was put more input control in lower level classes (such as actor). This flow allowed me to more easily make sense of the input controls I was getting, and avoid using bools for flags for input in my game or map and having
to carry it forward or make it accessible to my entity classes.  this felt much more inline with Unity's input handling which I was familiar with.  I think there could still be some better ways to handle input than what I did, but it
still felt mmore natural to me than putting it all in the map and/or game.

Things to definitely take more care in:
	- using const reference more. Developing a bad habit of not using it.
	- use more private variables and accessors.  To be honest I just don't think about it, but it's a habir grown out of laziness.
	- really bad about giant nested if blocks. I need to focus on trying to improve readability and rapid understanding.  Even I get lost and that's bad.
	- More convenience functions.  I need to set a standard for myself in that if I write something twice, I just need to make a method for it.


My time management was still lacking.  I feel like there is always more I can do.  Being slightly behind from A8 was pretty damaging to my time for this assignment. I should have done better of either: evaluating what I was going
to drop from A8 that I shouldn't have even wasted time on for A9 OR busted my ass and just gotten the remainder of A8 done immediately following submission.  The difficulty around the Adventure definitions was very unforseen, and
showed a poor lack of vision on my part.

In terms of A9, I feel very achieved and incredibly dissappointed in myself simultaneously.  I am not particularly satisified with my game, and I don't feel like it's fun, pretty to look at, or even all that impressive. However,
there was a ton of work into getting the missing pieces from A8 and the new map generation and sprite animation work that has me excited to play with in the future.  The potential for this assignment seemed virtually limitless and exciting.
I hope to be able to work over the winter break to at least touch elements of the rest of the assignment so I can have an even broader understanding moving forward.
