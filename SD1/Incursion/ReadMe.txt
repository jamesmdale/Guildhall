INCURSION


How to Use:

	When Incursion begins. You are loaded into a main  menu screen. You can press Start to begin playing the game or escape from this menu to quit.
	
	When you begin the game the player is spawned as a tank on a map with randomized tiles.  The goal is for the player to survive the level and navigate to the
	exit portal.  When they arrive at the exit portal, they are transported to the next map.  After several maps, they player arrives on a manually generated boss map.
	In order to win the game the player must kill the boss.

	The following keys will have the listed actions:

	- Pressing and holding the 'T' key slows down time to 1/10th of normal speed.

	- Pressing the 'F1' key enables debug mode
	- Pressing the 'F2' key sets the player health to  99999.f  (functionally giving them invincibility)

	- Pressing the P key or START button on the XBOX controller will pause the game. Pressing the P key again START button on the XBOX controller will unpause the game.
	- Pressing the 'Escape' key will exit the game.

	- Manipulating the LEFT ANALOG stick on a connected XBOX controller will also move the tank in the direction of the analog according to the magnitude
	of the stick up to a max speed.

	- Manipulating the RIGHT ANALOG stick on a connected XBOX controller will rotate the turret on top of the tank in the direction of the stick by a degree amount per second.
	- Pulling the RIGHT TRIGGER on a connected XBOX controller will fire bullets if the trigger is pulled greater than 50%.

	- Enemy tanks and turrets spawn on the beginning of the map when the game starts.  Tanks will move and rotate towards they player if the player is in line of sight.
	Otherwise, tanks will patrol the map and change directions randomly.

	- Turrets will rotate their turret towards the player if the player is in line of sight, but they remain stationary. Otherwise they rotate randomly similar to tanks.

	- Turrets have 1 health.
	- Enemy tanks have 3 health.
	- The player tank has 10 health.
	- Boss tanks have ??? health. 0.0
	
	
	- If an enemy tank or turret's health goes below zero they are destroyed and removed from the map.

	- If the player tank's health goes below zero, they are destroyed and after 5 seconds a respawn screen will appear.  While the player is dead and before
	the respawn screen appears, the player cannot pause the screen.  To respawn, the player may press P or the START button on the XBOX controller.

	- While the player is dead, tanks will stop firing at the last position of the player and they will continue to roam randomly.


KNOWN BUGS:
	
	No victory screen.

Deep Learning:

	For this assignment there was a lot more care taken in the structure of my code than for Asteroids.  I felt like, the overall composition of my code
is more clear and easier to access than that of Asteroids.  I felt a lot less chaotic.  This assignment felt much less about LEARNING C++ and principles and more about applying my new
knowledge gained . I also did more in terms of just trying to get things working and apply and add new classes and properties as I went along rather
than adding a lot of features and code up front. As an example my Map is currently modular enough for me to add arrays of more objects including enemy tanks.
One refactor that I will have to do is if I add more players I will need to revisit my array of PlayerTanks, but other than that, my code feels very simplified. I do need to do better about
taking time to refactor and add helper functions as I go on.  As it normally occurs, the more cool juice I added the more my Map.cpp got messy. Because of it's increasing messiness I began
minding less and less doing some hacky code or copying and pasting a section instead of just making another function.  For adventure, I really need to take the extra few minutes as I go
to keep my code cleaner and simpler.

	As far as my goals that I set from A3, I felt better about being more ahead of class for most of Incursion and engaged based on the work I had done for the assignment throughout the week,
but I am still lacking in terms of time management for some of the more complicated functions.




