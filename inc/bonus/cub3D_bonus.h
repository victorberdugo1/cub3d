/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 11:48:08 by vberdugo          #+#    #+#             */
/*   Updated: 2025/04/26 22:01:28 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_BONUS_H
# define CUB3D_BONUS_H

# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include <math.h>
# include "libft.h"
# include "MLX42.h"

/*------------------------------------------------------------------------------
GAME CONSTANTS
------------------------------------------------------------------------------*/
# define WIDTH 1600
# define HEIGHT 900
# define COLLISION_RADIUS 0.2
# define DOOR_ANIM_DURATION 0.25
/*------------------------------------------------------------------------------
MINIMAP SETTINGS
------------------------------------------------------------------------------*/
# define MINI_SCALE 10
# define MINI_VIEW_DIST 9
# define MINI_X 110
# define MINI_Y 110
# define MINI_RADIUS 90
/*------------------------------------------------------------------------------
HUD & WEAPON SETTINGS
------------------------------------------------------------------------------*/
# define COLS      4
# define ROWS      4
# define SCALE     1.8f
# define Y_OFFSET  10

/*------------------------------------------------------------------------------
CORE STRUCTURES
------------------------------------------------------------------------------*/

// 2D vector for positions/directions
typedef struct s_vec2
{
	double	x;
	double	y;
}	t_vec2;

// Player camera and movement properties
typedef struct s_camera
{
	t_vec2	pos;		// World coordinates
	t_vec2	dir;		// Direction vector
	t_vec2	plane;		// Camera plane (FOV control)
	double	move_speed;	// Base movement speed
	double	view_z;		// Vertical view offset (jumping/crouching)
}	t_camera;

// Raycasting calculation results
typedef struct s_ray
{
	t_vec2	raydir;			// Direction of cast ray
	double	raydir_mod;		// Modified ray direction magnitude
	int		map_x;			// Current map X coordinate
	int		map_y;			// Current map Y coordinate
	t_vec2	deltadist;		// Distance between grid lines
	t_vec2	sidedist;		// Perpendicular wall distances
	t_vec2	step;			// X/Y step directions
	int		side;			// Wall side (0=X, 1=Y)
	double	perpwalldist;	// Perpendicular wall distance
	char	hit_tile;		// Type of tile hit ('1', '2', etc.)
}	t_ray;

/*------------------------------------------------------------------------------
RENDERING STRUCTURES
------------------------------------------------------------------------------*/

// Wall/door texture drawing parameters
typedef struct s_draw
{
	int				lh;		// Line height
	int				ds;		// Draw start Y
	int				de;		// Draw end Y
	int				tx;		// Texture X coordinate
	mlx_texture_t	*tex;	// Pointer to texture
}	t_draw;

// Door state tracking
typedef struct s_door
{
	int		x;				// Map X position
	int		y;				// Map Y position
	int		is_open;		// Open/closed state
	int		orientation;	// '2'=vertical, '3'=horizontal
	double	open_offset;	// Animation progress (0-1)
	double	move_progress;	// Current animation time
}	t_door;

/*------------------------------------------------------------------------------
ENEMY SYSTEM STRUCTURES
------------------------------------------------------------------------------*/

// Enemy states
typedef struct s_enemy
{
	// Behavior states
	enum
	{
		ENEMY_MOVE,
		ENEMY_ATTACK,
		ENEMY_COOLDOWN,
		ENEMY_HIT,
		ENEMY_DEAD
	} e_state;
	// Movement directions
	enum
	{
		FRONT,
		FRONT_RIGHT,
		RIGHT,
		BACK_RIGHT,
		BACK,
		BACK_LEFT,
		LEFT,
		FRONT_LEFT
	} e_dir;
	// Position and movement
	double	pos_x;
	double	pos_y;
	double	speed;
	int		is_active;
	// Animation control
	double	time_since_last_move;
	int		anim_frame;
	double	facing_angle;
	bool	initialized;
	// Combat properties
	double	radius;
	double	anim_timer;
	int		hit_count;
	double	attack_cooldown;
	int		hit_flash;
	double	knockback_time;
	t_vec2	knockback_dir;
}	t_enemy;

/*------------------------------------------------------------------------------
WEAPON SYSTEM STRUCTURES
------------------------------------------------------------------------------*/

typedef struct s_weapon
{
	mlx_texture_t	*texture;			// Weapon spritesheet
	int				current_frame;		// Current animation frame
	bool			is_attacking;		// Attack state flag
	double			frame_timer;		// Animation timing
	int				total_idle_frames;	// Idle animation frames
	int				total_attack_frames;// Attack animation frames
	bool			alt_animation;		// Alternative animation flag
}	t_weapon;

/*------------------------------------------------------------------------------
USER INTERFACE STRUCTURES
------------------------------------------------------------------------------*/

// Visual feedback systems
typedef struct s_hit_feedback
{
	bool	active;		// Activation flag
	double	timer;		// Effect duration tracker
	double	duration;	// Total effect time
}	t_hit_feedback;

typedef struct s_collision
{
	double	r;
	int		min_i;
	int		max_i;
	int		min_j;
	int		max_j;
	int		i;
	int		j;
	double	closest_x;
	double	closest_y;
	double	dx;
	double	dy;
}	t_collision;

typedef struct s_ceiling
{
	t_vec2		wrd;
	t_vec2		frac;
	t_collision	col;
	int			x;
	int			y;
}	t_ceiling;

typedef struct s_draw_data
{
	int		sprite_height;
	int		sprite_width;
	int		st_x;
	int		end_x;
	int		st_y;
	int		end_y;
	int		tex_width;
	int		tex_height;
	int		width;
	int		hgt;
	int		offset_x;
	int		offset_y;
	double	transform_y;
}	t_draw_data;

/*------------------------------------------------------------------------------
GAME STATE STRUCTURES
------------------------------------------------------------------------------*/

// Main game configuration
typedef struct s_game
{
	// Texture paths
	char			*texture_no;
	char			*texture_so;
	char			*texture_we;
	char			*texture_ea;
	// Color configurations
	int				floor_color[3];
	int				c_color[3];
	// Map data
	char			**map;
	int				map_height;
	// Loaded textures
	mlx_texture_t	*tex_no;
	mlx_texture_t	*tex_so;
	mlx_texture_t	*tex_we;
	mlx_texture_t	*tex_ea;
	// Door system
	int				map_started;
	t_door			*doors;
	int				door_count;
	char			*texture_door;
	char			*texture_door_w;
	char			*texture_enemy;
	mlx_texture_t	*tex_door;
	mlx_texture_t	*tex_door_w;
	// Enemy system
	mlx_texture_t	*tex_enemy;
	t_enemy			*enemies;
	int				enemy_count;
	// Weapon system
	char			*texture_weapon;
	mlx_texture_t	*tex_weapon;
}	t_game;

// Main application container
typedef struct s_app
{
	mlx_t			*mlx;				// MLX graphics context
	mlx_image_t		*image;				// Main render target
	t_camera		cam;				// Player camera
	t_game			game;				// Game state
	double			z_buffer[WIDTH];	// Depth buffer
	t_hit_feedback	player_hit_feedback;// Damage effect
	t_weapon		weapon;				// Weapon state
}	t_app;

/*------------------------------------------------------------------------------
FUNCTION DECLARATIONS
------------------------------------------------------------------------------*/

// Core game loop
void		render_scene(void *param);
void		move_camera(void *param);

//Raycast system
void		do_dda(t_app *app, t_ray *ray);
void		compute_texture_params(t_app *app, t_ray *ray, t_draw *draw);
void		calculate_grid_coordinates(t_vec2 world, t_collision *col);
char		safe_get_tile(t_game *game, int x, int y);
void		draw_pixels(t_app *app, int x, t_draw *draw);

// Map parsing
void		process_lines(char **lines, int c, t_game *game, t_camera *camera);
int			read_lines(const char *filename, char ***lines, int *line_count);
void		validate_map(t_game *game, t_camera *camera);
int			load_map(char *filename, char ***lines, int *line_count);
char		*skip_spaces(char *s);

// Initialization
int			init_app_struct(t_app *app, char **lines, int line_count);
int			load_game_textures(t_app *app);
void		init_ray(t_app *app, int x, t_ray *ray);
void		init_door(t_game *g, int i, int j, char c);
void		init_enemy(t_game *g, int i, int j);
void		init_collision(t_collision *c, double new_x, double new_y);
void		weapon_initialize(t_weapon *w, bool *inited);
void		init_background_tables(double *sx, double *cy,
				double *cameraX_table, bool light_panel_pattern[225]);

// Rendering systems
void		ft_draw_background(t_app *app);
void		render_minimap(t_app *app);
void		render_enemy(t_app *app, t_enemy *e);
void		render_weapon(t_app *app);
void		render_hit_feedback(t_app *app);
int32_t		ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a);
uint32_t	convert_pixel(uint32_t px);
void		draw_minimap_background(t_app *app);
void		draw_map_features(t_app *app);
void		update_weapon_animation(t_app *app, double delta_time);
void		update_door_animation(t_app *app, double dt);
uint32_t	fog(const int *color, double r, double fog_dens, double min_factor);

// Collision detection
int			collides(t_game *game, double new_x, double new_y);
int			check_door_collision(t_app *app, t_ray *ray);

// Player systems
void		get_down(t_app *app, double *speed);
void		jump(t_app *app);
void		toggle_doors(t_app *app);

// Enemies
void		update_enemies(t_app *app, double delta_time);
void		update_enemy(t_app *app, t_enemy *e, t_camera *cam, double dt);
void		update_enemy_dir(t_app *app, t_enemy *enemy, double delta_time);
void		handle_enemy_collision(t_app *app, t_enemy *e, t_camera *cam);
void		move_towards_cam(t_app *app, t_enemy *e, t_camera *cam, double dt);
void		apply_hit_to_enemy(t_app *app);
void		process_knockback(t_app *a, t_enemy *e, double dt);
void		check_enemy_attack_hit(t_app *app, t_enemy *e, t_camera *cam);
void		update_hit_feedback(t_app *app, double dt);
void		apply_hit_flash(uint32_t *color, t_enemy *e);

// Utility functions
void		safe_free(t_app *app);
void		free_map_lines(char **lines, int line_count);
void		cleanup(t_app *app);
void		close_window(void *param);

#endif
