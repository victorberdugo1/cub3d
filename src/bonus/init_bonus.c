/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 12:48:06 by victor            #+#    #+#             */
/*   Updated: 2025/04/24 20:41:47 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Initializes the t_app structure, setting default values and processing   */
/*   the map lines to extract game and camera data.                           */
/*                                                                            */
/* ************************************************************************** */
int	init_app_struct(t_app *app, char **lines, int line_count)
{
	ft_memset(app, 0, sizeof(*app));
	ft_memset(app->game.floor_color, -1, sizeof(app->game.floor_color));
	ft_memset(app->game.ceiling_color, -1, sizeof(app->game.ceiling_color));
	process_lines(lines, line_count, &app->game, &app->cam);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Initializes a ray for the given screen column (x).                       */
/*                                                                            */
/*   - Computes `camx`, which represents the camera's x-coordinate in        */
/*     normalized device coordinates (ranging from -1 to 1).                  */
/*     Formula: camx = 2 * x / WIDTH - 1                                      */
/*                                                                            */
/*   - Calculates the ray direction `raydir` using the camera direction       */
/*     (`dir`) and plane (`plane`):                                           */
/*       raydir.x = dir.x + plane.x * camx                                    */
/*       raydir.y = dir.y + plane.y * camx                                    */
/*                                                                            */
/*   - Determines `map_x` and `map_y`, which are the integer coordinates      */
/*     of the current grid cell (truncated from `pos`).                       */
/*                                                                            */
/*   - Computes `deltadist`, the distance the ray travels between grid        */
/*     lines in each axis. It is calculated as:                               */
/*       deltadist.x = |raydir_modulus / raydir.x|                            */
/*       deltadist.y = |raydir_modulus / raydir.y|                            */
/*                                                                            */
/* ************************************************************************** */
void	init_ray(t_app *app, int x, t_ray *ray)
{
	double	camx;

	camx = 2 * x / (double)WIDTH - 1;
	ray->raydir.x = app->cam.dir.x + app->cam.plane.x * camx;
	ray->raydir.y = app->cam.dir.y + app->cam.plane.y * camx;
	ray->map_x = (int)app->cam.pos.x;
	ray->map_y = (int)app->cam.pos.y;
	ray->raydir_mod = sqrt(pow(ray->raydir.x, 2) + pow(ray->raydir.y, 2));
	ray->deltadist.x = fabs(ray->raydir_mod / ray->raydir.x);
	ray->deltadist.y = fabs(ray->raydir_mod / ray->raydir.y);
}

/* ************************************************************************** */
/*                                                                            */
/*   Initializes a collision structure (`t_collision`) for a given position.  */
/*                                                                            */
/*   - The collision system considers the player as a circular shape with a   */
/*     radius `COLLISION_RADIUS`.                                             */
/*   - This function calculates the bounding box around the circle to check   */
/*     for potential collisions with walls.                                   */
/*   - The bounding box is defined by:                                        */
/*       - `min_i` and `max_i`: vertical range of tiles to check.             */
/*       - `min_j` and `max_j`: horizontal range of tiles to check.           */
/*                                                                            */
/* ************************************************************************** */
void	init_collision(t_collision *c, double new_x, double new_y)
{
	double	r;

	r = COLLISION_RADIUS;
	c->r = r;
	c->min_i = (int)floor(new_y - r);
	c->max_i = (int)ceil(new_y + r);
	c->min_j = (int)floor(new_x - r);
	c->max_j = (int)ceil(new_x + r);
}

void	init_enemy(t_game *g, int i, int j)
{
	g->enemy_count++;
	g->enemies = realloc(g->enemies, sizeof(t_enemy) * g->enemy_count);
	g->enemies[g->enemy_count - 1] = (t_enemy){
		.pos_x = j + 0.5,
		.pos_y = i + 0.5,
		.speed = 0.6,
		.is_active = 1,
		.anim_frame = 0.0,
		.time_since_last_move = 0.0,
		.radius = 0.7
	};
	g->map[i][j] = '0';
}

void	init_door(t_game *g, int i, int j, char c)
{
	g->door_count++;
	g->doors = realloc(g->doors, sizeof(t_door) * g->door_count);
	g->doors[g->door_count - 1] = (t_door){
		.x = j,
		.y = i,
		.is_open = 0,
		.orientation = c,
		.open_offset = 0.0,
		.move_progress = DOOR_ANIM_DURATION
	};
	g->map[i][j] = c;
}
