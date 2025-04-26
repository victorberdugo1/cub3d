/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 12:48:06 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 13:29:45 by victor           ###   ########.fr       */
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
	ft_memset(app->game.c_color, -1, sizeof(app->game.c_color));
	process_lines(lines, line_count, &app->game, &app->cam);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Prepares a ray for grid DDA:                                             */
/*   - camx = 2*x/WIDTH - 1: normalized device x in [-1,1].                   */
/*   - raydir = cam.dir + cam.plane * camx: compute ray direction.            */
/*   - map_x,map_y = integer cell of camera position.                         */
/*   - raydir_mod = length of raydir vector.                                  */
/*   - deltadist.x/y = abs(raydir_mod / raydir.x/y) for DDA stepping.         */
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
/*   Sets up collision bounds for a circle at (new_x,new_y):                  */
/*   - r = COLLISION_RADIUS for player circle.                                */
/*   - min_i/max_i, min_j/max_j = floor/ceil of pos ± r to cover tiles.       */
/*   - These define the grid cells to check for collisions.                   */
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

/* ************************************************************************** */
/*                                                                            */
/*   Adds a new enemy at map cell (i,j):                                      */
/*   - Increments enemy_count and reallocates the enemies array.              */
/*   - Sets pos to cell center (j+0.5,i+0.5), speed, active flag, radius,     */
/*     and resets animation timers.                                           */
/*   - Replaces map cell with '0' to clear spawn marker.                      */
/*                                                                            */
/* ************************************************************************** */
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

/* ************************************************************************** */
/*                                                                            */
/*   Adds a new door at map cell (i,j):                                       */
/*   - Increments door_count and reallocates the doors array.                 */
/*   - Initializes x, y, orientation, is_open=false, open_offset, and timer.  */
/*   - Sets map cell char to c to mark door in the map.                       */
/*                                                                            */
/* ************************************************************************** */
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
