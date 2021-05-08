# Cosmic Capture

Cosmic Capture is a capture the flag driving game. It features local multiplayer and AI opponents, powerups, and a dynamic map with opening and closing doors. It was built over the course of the winter 2021 semester for CPSC 585 (Games Programming) at the University of Calgary.

## Gameplay

Return the flag to your base three times in order to win! The compass at the top of the screen points in the direction of the flag, or in the direction of your home base if yo are carrying the flag. Your speed is slowed when carrying the flag, and any opponents can instantly explode you by running into you.

Use the three available powerups to evade, trap, and outrun your opponents. Pickup powerups from the designated pickup boxes spread across the map. Available powerups are:

- The speed boost: greatly increase your speed and rocket yourself away from the opponents.
- The spike trap: place a large spike trap on the ground to stop you opponents dead in their tracks. Watch out though - once placed a spike trap can trap anybody, including yourself
- The projectile: fire a projectile at you opponents. On a hit they are instantly demolished and sent back to their starting location. Be careful - the projectile can bounce off objects and destroy your car as well.

Keep an eye and an ear out for the moving gates, as they will stop you dead in your tracks! Look for a different way around if a gate closes in front of you.

## Technical Details

The game is built using `C++`, `OpenGL` for graphics, and `NVIDIA PHYSX` for driving simulation. We also use a few other `C/C++` libraries, including `SDL2` for input and window management, `OpenAL` for audio, `assimp` for loading 3D files, and `glm` for math. The game runs on windows computers, and runs best if the computer has a dedicated graphics card. Note that the Microsoft Visual C++ for visual studio 2015, 2017, and 2019 is required in order to run the game, and can be obtained from here: [Visual C++ Download](https://support.microsoft.com/en-us/topic/the-latest-supported-visual-c-downloads-2647da03-1eea-4433-9aff-95f26a218cc0). 



## Screenshots

**Player vs AI Gameplay**

<img src="screenshots\gameplay.png" alt="Gameplay" style="zoom:75%;" />

**Local Multiplayer Gameplay**

<img src="screenshots\multiplayer.png" alt="Multiplayer" style="zoom:75%;" />

**Start Screen**

<img src="screenshots\start-screen.PNG" alt="Start Screen" style="zoom:75%;" />

**Player Select**

<img src="screenshots\player-select.PNG" alt="Player Select" style="zoom:100%;" />
