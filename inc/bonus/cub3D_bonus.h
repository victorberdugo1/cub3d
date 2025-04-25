/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 11:48:08 by vberdugo          #+#    #+#             */
/*   Updated: 2025/04/25 15:52:29 by victor           ###   ########.fr       */
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

# define WIDTH 1600
# define HEIGHT 900
# define COLLISION_RADIUS 0.2
# define DOOR_ANIM_DURATION 0.25
//minimap
# define MINI_SCALE 10
# define MINI_VIEW_DIST 10
# define MINI_X 100
# define MINI_Y 100
# define MINI_RADIUS 100

typedef struct s_vec2
{
	double	x;
	double	y;
}	t_vec2;

typedef struct s_camera
{
	t_vec2	pos;
	t_vec2	dir;
	t_vec2	plane;
	double	move_speed;
	double	view_z;
}	t_camera;

typedef struct s_ray
{
	t_vec2	raydir;
	double	raydir_mod;
	int		map_x;
	int		map_y;
	t_vec2	deltadist;
	t_vec2	sidedist;
	t_vec2	step;
	int		side;
	double	perpwalldist;
	char	hit_tile;
}	t_ray;

typedef struct s_draw
{
	int				lh;
	int				ds;
	int				de;
	int				tx;
	mlx_texture_t	*tex;
}	t_draw;

typedef struct s_door
{
	int				x;
	int				y;
	int				is_open;
	int				orientation;
	double			open_offset;
	double			move_progress;
}	t_door;

typedef struct s_enemy
{
	enum
	{
		ENEMY_MOVE,
		ENEMY_ATTACK,
		ENEMY_COOLDOWN,
		ENEMY_HIT,
		ENEMY_DEAD
	} e_state;
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
	double	pos_x;
	double	pos_y;
	double	speed;
	int		is_active;
	double	time_since_last_move;
	int		anim_frame;
	double	facing_angle;
	bool	initialized;
	double	radius;
	double	anim_timer;
	int		hit_count;
	double	attack_cooldown;
	int		hit_flash;
	double	knockback_time;
	t_vec2	knockback_dir;
}	t_enemy;

typedef struct s_weapon
{
	mlx_texture_t	*texture;
	int				current_frame;
	bool			is_attacking;
	double			frame_timer;
	int				total_idle_frames;
	int				total_attack_frames;
	bool			alt_animation;
}	t_weapon;

typedef struct s_game
{
	char			*texture_no;
	char			*texture_so;
	char			*texture_we;
	char			*texture_ea;
	int				floor_color[3];
	int				c_color[3];
	char			**map;
	int				map_height;
	mlx_texture_t	*tex_no;
	mlx_texture_t	*tex_so;
	mlx_texture_t	*tex_we;
	mlx_texture_t	*tex_ea;
	int				map_started;
	t_door			*doors;
	int				door_count;
	char			*texture_door;
	char			*texture_door_w;
	char			*texture_enemy;
	mlx_texture_t	*tex_door;
	mlx_texture_t	*tex_door_w;
	mlx_texture_t	*tex_enemy;
	t_enemy			*enemies;
	int				enemy_count;
	char			*texture_weapon;
	mlx_texture_t	*tex_weapon;
}	t_game;

typedef struct s_hit_feedback
{
	bool	active;
	double	timer;
	double	duration;
}	t_hit_feedback;

typedef struct s_app
{
	mlx_t			*mlx;
	mlx_image_t		*image;
	t_camera		cam;
	t_game			game;
	double			z_buffer[WIDTH];
	t_hit_feedback	player_hit_feedback;
	t_weapon		weapon;
}	t_app;

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

void		process_lines(char **lines, int c, t_game *game, t_camera *camera);
int			read_lines(const char *filename, char ***lines, int *line_count);
char		safe_get_tile(t_game *game, int x, int y);
int			collides(t_game *game, double new_x, double new_y);
void		render_scene(void *param);
void		move_camera(void *param);
void		close_window(void *param);
int			load_map(char *filename, char ***lines, int *line_count);
void		free_map_lines(char **lines, int line_count);
int			init_app_struct(t_app *app, char **lines, int line_count);
void		cleanup(t_app *app);
void		validate_map(t_game *game, t_camera *camera);
void		ft_draw_background(t_app *app);
void		draw_pixels(t_app *app, int x, t_draw *draw);
void		init_ray(t_app *app, int x, t_ray *ray);
void		do_dda(t_app *app, t_ray *ray);
char		*skip_spaces(char *s);
void		toggle_doors(t_app *app);
void		update_door_animation(t_app *app, double dt);
void		get_down(t_app *app, double *speed);
void		jump(t_app *app);
void		update_enemies(t_app *app, double delta_time);
void		render_enemy(t_app *app, t_enemy *e);
uint32_t	convert_pixel(uint32_t px);
void		render_minimap(t_app *app);
int			load_game_textures(t_app *app);
void		safe_free(t_app *app);
void		update_enemy(t_app *app, t_enemy *e, t_camera *cam, double dt);
void		update_enemy_dir(t_app *app, t_enemy *enemy, double delta_time);
void		draw_minimap_background(t_app *app);
void		draw_map_features(t_app *app);
int			check_door_collision(t_app *app, t_ray *ray);
void		compute_texture_params(t_app *app, t_ray *ray, t_draw *draw);
void		init_door(t_game *g, int i, int j, char c);
void		init_enemy(t_game *g, int i, int j);
void		init_collision(t_collision *c, double new_x, double new_y);
void		apply_hit_to_enemy(t_app *app);
void		check_enemy_attack_hit(t_app *app, t_enemy *e, t_camera *cam);
void		update_hit_feedback(t_app *app, double dt);
void		render_hit_feedback(t_app *app);
void		handle_enemy_collision(t_app *app, t_enemy *e, t_camera *cam);
void		move_towards_cam(t_app *app, t_enemy *e, t_camera *cam, double dt);
void		process_knockback(t_app *a, t_enemy *e, double dt);
void		apply_hit_flash(uint32_t *color, t_enemy *e);
int32_t		ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a);
void		init_background_tables(double *sx, double *cy,
				double *cameraX_table, bool light_panel_pattern[225]);
void		calculate_grid_coordinates(t_vec2 world, t_collision *col);
void		update_weapon_animation(t_app *app, double delta_time);
void		render_weapon(t_app *app);
uint32_t	fog(const int *color, double r, double fog_dens, double min_factor);

#endif
