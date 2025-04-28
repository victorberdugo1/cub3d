# cub3D

<p align="center">
<video src="https://github.com/user-attachments/assets/0e233f44-0c16-425f-a531-52519b08a03f" width=180/></video>
</p>


## Mandatory Part

| **Name**                 | **`cub3D`**                                 |
|--------------------------|---------------------------------------------|
| **Files**                | Makefile, `*.h`, `*.c`, textures, map files |
| **Makefile Rules**       | `NAME`, `all`, `clean`, `fclean`, `re`, `bonus` |
| **Arguments**            | A single scene file with `.cub` extension   |
| **Allowed Functions**    | `open`, `close`, `read`, `write`, `printf`, `malloc`, `free`, `perror`, `strerror`, `exit`, `gettimeofday`, math functions (`-lm`), MiniLibX functions |
| **libft**                | Allowed                                     |
| **Description**          | A 3D first-person representation of a maze using raycasting techniques. |

The project must comply with the following rules:
- You must use the MiniLibX (either system-installed or included from source).
- The map input file must have a `.cub` extension and follow the required format.
- The program must open a window rendering a 3D view based on the map.
- Player should be able to move using `W`, `A`, `S`, `D` and rotate the camera using arrow keys.
- Pressing `ESC` or clicking the close button must exit the game cleanly.
- Different wall textures must be shown depending on wall orientation (N, S, E, W).
- The floor and ceiling must be rendered in different user-defined colors.

### Example of a valid `.cub` map file:
```sh
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 220,100,0
C 225,30,0

111111
100001
1000N1
100001
111111
```

Map rules:
- May contain only `0`, `1`, `N`, `S`, `E`, `W`.
- Must be completely enclosed by walls.
- Must have exactly one player start position.
- Any file or map error must return `Error\n` followed by a clear message.

---

## Bonus Part

### Extra Features

Possible bonus features:
- Wall collisions.
- A minimap system.
- Doors which can open and close.
- Animated sprites.
- Rotate the point of view with the mouse

You may use additional functions if justified, and organize bonus files using `_bonus.c` / `_bonus.h` suffixes.

---

## Mandatory part

### Executable name
Check that the project compiles (with or without -fsanitize) when you execute the `make`. Format and check the executable name is `cub3D` (if not, see the invalid compilation flag at the end of the scale).

[**YES**](#)  |  [**NO**](#)

### Configuration file
Check that you can configure ALL the following elements in the configuration file. The formatting has to be as described in the subject:

- north texture path - NO

- south texture path - SO

- west texture path - WE

- east texture path - EA

- floor color - F

- ceiling color - C

- the map (respect the rules for the map configuration described in subject)

Also, check that the program exits on error and exits properly when the configuration file is reconfigured (for example, an invalid path) or the program is launched with a filename that doesn’t end with `.cub` extension.

if not, the defense is over and use the appropriate flag incomplete work, crash...

[**YES**](#)  |  [**NO**](#)


### Technical elements of the display
We’re going to check the technical elements of the display. Run it.

Please try to execute the following test. If at least one fails, this means that no points will be awarded for this section.

- A window must open at launch of the program, It must stay open during the whole execution.

- An image representing the inside of a maze must be displayed inside the window.

- Hide all or part of the window either by using another window or by using the screen's border, then minimize the window and maximize it back. in all cases, the content of the window must remain consistent

[**YES**](#)  |  [**NO**](#)


### User basic events
In this section, we’re going to evaluate the program’s user generated events. Execute the 3 following tests. If at least one fails, this means that no points will be awarded for this section.

Move to the next test.

- Click the red cross at the top left of the window. The window must close and the program must exit cleanly

- Press the ESC key. The window must close and the program must exit Press another key, in the case of this test, we will accept Press any key to kill the program, for example, Q

- Press the four movement keys (we’ll accept WASD or ZQSD pair) In the order of your liking. Each key press must make the visible result on the window, such as a player's movement/reaction.

[**YES**](#)  |  [**NO**](#)


### Movements
In this section, we will evaluate the implementation of the player's movements. Execute the 5 following tests, if at least one fails, this means that no points will be awarded for this section.

- The player's spawning orientation on the first image must be accordace with the configuration file, test for each cardinal orientation (N, S, E, W).

- The player’s view can rotate to the left and to the right as if the player’s head was moving

- Press W (or Z) then S. The player’s view must go forward and then backward in a straight line

- Press A (or Q) then D. The player’s view must go to the left and then to the right in a straight line

- During those four movements, was the display smooth? Were we seen in the game "playable" as it is?

[**YES**](#)  |  [**NO**](#)


### Walls
In this section, we’ll evaluate the walls in the maze. Execute the 4 following tests. If at least one fails, this means that no points will be awarded for this section.

- The walls’s texture vary depending on which compass point the wall is facing (north, south, east, west) visible and correct

- Check that if you modify the path of a wall texture in the configuration file a modified file reloaded texture when the program is launched

- Also check that if you use non-existent path it raises an error

- Check that the floor and ceiling colors are well handled when you modify them in the configuration file

[**YES**](#)  |  [**NO**](#)


### Error management
In this section, we will evaluate the program’s error management and reliability. Execute the 4 following tests. If at least one fails, this means that no points will be awarded for this section.

- Run the program using numerous arguments and random data. Even if the program doesn’t analyze any arguments, it must not crash Check that those arguments don’t alternate or create unhandled errors

- Check that there are no memory leaks. You can use `top` or `leaks` command in another shell to monitor the memory use is stable. the memory must not increase each time a new action is made

- Roll either your arm or your face on the keyboard. The program must not change behavior and it must stay functional

- Modify the map, the program must not show strange behaviors Example: non-closed file map is well configured, if not must raise an error.

[**YES**](#)  |  [**NO**](#)


## Bonus
We will look at your bonuses if and only if your mandatory part is excellent. This means that you must complete the mandatory part perfectly in order to even move to the bonus. The mandatory part has been entirely and perfectly done, and the error management handles unexpected or bad usage. In case all the mandatory points were not passed during the defense, bonus points must be totally ignored.

### When I’ll be older I’ll be John Carmack

Look at the subject bonus part and add one point for each bonus implemented and fully functional.

`
🟩🟩🟩🟩🟩  5/5
`
