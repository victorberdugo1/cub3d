# cub3D

<p align="center">
  <img src="cub3d.jpg" alt="cub3D" style="width:60%; margin:auto;" />
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

