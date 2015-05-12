# EE-CS120B-Project---Byte-iT
Similar Frogger Type Game with NES Controller

General
Our game is named Byte It. It will set on a grid of 7-by-13. The player will be initialized to the center of the screen. And when the game begins, obstacles will go across horizontally and vertically. The player must avoid these obstacles by moving up, down, left, or right. The game will end when the player is hit by one of the obstacles. The player will gain points by picking up apples and diamonds on the grid to gain points. Both the apple, diamond, and obstacles will appear as time progresses. Once enough rewards (apple and diamond) are collected, the player may choose to consume the power-up by pressing A, then he will be temporarily be immune to all obstacles. The score will also be displayed using 2 7-seg numeric display to show the score the player has accumulated thus far.

User Guide / Rules
You are left stranded in the woods. Obstacles are coming your way and it is your mission to dodge and evade them. There will be fresh fruit and gems that you can pick up on your journey. Apples give you 1 point and one star power, gems give you 3. Once you obtain 8 points, your are able to initiate star power with the ‘A’ button giving you the opportunity to fight back and eat the obstacles. Staying alive is the key to winning.

Objective
Collect as much fruits as possible while avoiding obstacles and trying to stay alive. Game ends once the player is hit by any obstacle.

Components / Build-Upons
Build-upon #1: using 2 micro controllers - one for controlling the main display (the LCD), and the other for keeping track of all other modules, such as scores, obstacles, etc using UART
Build-upon #2: Nokia 6100 LCD Screen - will be used to display the grid, which is the main UI of the game
Build-upon #3: NES Controller - to be used as the main controller of the game
Build-upon #4: 7-SEG display (use 3) - to be used as scoring
Build-upon #5: using advanced SM
Build-upon #6: Customize the 2x16 LCD screen and program it by pixels

Schematic:


![alt tag](http://i.imgur.com/4rCz0sE.jpg)
