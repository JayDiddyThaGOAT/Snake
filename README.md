# Snake

This is my version of the old school game, **Snake**.  I drew the sprites of the cells using GIMP 2.1.0 & developed the game in the Visual
Studio IDE programmed in C++ using the SFML Engine. The goal of Snake is to eat the apple in the grid so that the snake can grow without 
the snake touching itself. A new apple is generated onto any empty cell in the grid once the snake eats the apple. After I created the 
original game, I implemented artifical intelligence for the snake by calculating the closest cell to the apple by using the Manhattan
Distance between the snake's head and its neighbors in each move, then moves to it.

## Getting Started
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 
See deployment for notes on how to deploy the project on a live system.

### Prerequistes
```
Operating System: Windows 10
Must have .dll files inside folder where .exe is placed  in
Must have Visual Studio installed if you want to edit the code and debug it yourself
```
### Installing
A step by step series of examples that tell you how to get a development env running
```
Clone this repo or download the ZIP
```
## Running the Game
```
1. Open the Debug folder
2. Click on the Snake.exe
3. Press SPACE to play/pause the game
```

## Testing the Game
```
1. Open Visual Studio
2. Make sure the target environment is x86
3. Run the test by pressing Ctrl+F5, F5, or by clicking the play button
```