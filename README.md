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

Mandatory part
Executable name
Check that the project compiles (with or without -fsanitize) when you execute the make. Format and check the executable name is cub3D (if not, see the invalid compilation flag at the end of the scale).

✅/❌

Configuration file
Check that you can configure ALL the following elements in the configuration file. The formatting has to be as described in the subject:

north texture path - NO

south texture path - SO

west texture path - WE

east texture path - EA

floor color - F

ceiling color - C

a map (respect the rules for the map configuration described in subject)

Also, check that the program exits on error and exits properly when:

the configuration file is reconfigured (for example, an invalid path)

the program is launched with a filename that doesn’t end with .cub

if not, the deletes a user and use the appropriate flag (incomplete work, crash...)

✅/❌

Technical elements of the display
We’re going to check the technical elements of the display. Run it.

Please try to execute the following test. If at least one fails, this means that no points will be awarded for this section.

A window must open at launch of the program, managed through your in- / miniLibX educator

A window respecting the window size rules

Hide all or part of the window either by using another window or by using the screen's border, then minimize the window

Maximize it back: in all cases, the content of the window must remain consistent

✅/❌

User basic events
In this section, we’re going to evaluate the program’s user generated events. Execute the 3 following tests. If at least one fails, this means that no points will be awarded for this section.

Move to the next test...

Click the red cross at the top left of the window. The window must close and the program must exit cleanly

Press the ESC key. The window must close and the program must exit

Press another key, in the case of this test, we will accept:

Press any key to kill the program, for example, Q

Press the four movement keys (we’ll accept WASD or ZQSD pair) In the order of your liking. Each key press must make the visible result on the window, such as a player's movement/reaction.

✅/❌

Movements
In this section, we will evaluate the implementation of the player's movements. Execute the 5 following tests, if at least one fails, this means that no points will be awarded for this section.

The player can rotate the camera on the first image of the project’s description

Press A or D (or Q or D...)

The player’s view can rotate to the left and to the right as if the player’s head was moving

Press W (or Z on Fr)

The player’s view must go forward and then backward in the straight line

Press A or D (then D...)

The player’s view must go to the left and then to the right line

During those four movements, was the display smooth?

Were we seen in the game "playable" as it is?

✅/❌

Walls
In this section, we’ll evaluate the walls in the maze. Execute the 4 following tests. If at least one fails, this means that no points will be awarded for this section.

The walls’ paints vary depending on which compass point the wall is facing (short, short, east, west)

The north, south, east and west perspective must be:

visible and correct

Check that if you modify the path of a wall texture in the configuration file

a modified file reloaded texture when the program is launched

Also check that if you use non-existent path it raises an error

Check that the floor and ceiling colors are well handled when you modify them in the configuration file

✅/❌

Error management
In this section, we will evaluate the program’s error management and reliability. Execute the 4 following tests. If at least one fails, this means that no points will be awarded for this section.

Run the program using numerous arguments and random data

Even if the program doesn’t analyze any arguments, it must not crash

Check that those arguments don’t alternate or create unchecked errors

Check that there are no memory leaks. You can use:

leaks command in another shell to monitor the memory

With valgrind, check the memory: memory must not increase each time a new action is made

Reload your own or your leaks on the keyboard. The program must not change behavior and it must stay functional

Modify the map, the program must not show strange behaviors Example: non-closed file map is well configured, if not must raise an error.

✅/❌

Bonus
We will look at your bonuses (if and only if your mandatory part is excellent). This means that you must complete the mandatory part perfectly in order to even move to the bonus.

Remember not to fix a Bonus: even in case of violation of the rules, if the mandatory part is not correct the points will stay at the former bonus level totally ignored.

When I’ll be older I’ll be John Carmack

Look at the subject bonus part and add one point for each bonus implemented and fully functional.
