/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 13:50:51 by victor            #+#    #+#             */
/*   Updated: 2025/04/17 12:34:06 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

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
	ray->raydir.x = app->camera.dir.x + app->camera.plane.x * camx;
	ray->raydir.y = app->camera.dir.y + app->camera.plane.y * camx;
	ray->map_x = (int)app->camera.pos.x;
	ray->map_y = (int)app->camera.pos.y;
	ray->raydir_mod = sqrt(pow(ray->raydir.x, 2) + pow(ray->raydir.y, 2));
	ray->deltadist.x = fabs(ray->raydir_mod / ray->raydir.x);
	ray->deltadist.y = fabs(ray->raydir_mod / ray->raydir.y);
}

/* ************************************************************************** */
/*                                                                            */
/*   Determines the step direction and initial side distance.                 */
/*                                                                            */
/*   - The step `ret.x` determines whether the ray moves left (-1) or right   */
/*     (+1) in the x-direction, and similarly for `ret.y` in the y-direction. */
/*                                                                            */
/*   - The initial `sidedist` is calculated using:                            */
/*       sidedist_x = (map_x + 1 - pos_x) * deltadist_x  (if moving right)    */
/*       sidedist_x = (pos_x - map_x) * deltadist_x  (if moving left)         */
/*     Similarly for `sidedist_y`.                                            */
/*                                                                            */
/* ************************************************************************** */
static t_vec2	init_step(double pos, int map, double deltadist, double raydir)
{
	t_vec2	ret;

	if (raydir < 0)
	{
		ret.x = -1;
		ret.y = (pos - map) * deltadist;
	}
	else
	{
		ret.x = 1;
		ret.y = ((map + 1.0) - pos) * deltadist;
	}
	return (ret);
}

/* ************************************************************************** */
/*                                                                            */
/*   Implements the Digital Differential Analyzer (DDA) algorithm.            */
/*                                                                            */
/*   - This algorithm finds the first intersection of the ray with a wall.    */
/*   - The ray moves in discrete steps, choosing either the x or y direction  */
/*     depending on which `sidedist` is smaller.                              */
/*                                                                            */
/*   - If `sidedist.x < sidedist.y`, the ray moves in the x-direction:        */
/*       sidedist.x += deltadist.x                                            */
/*       map_x += step_x                                                      */
/*                                                                            */
/*   - Otherwise, it moves in the y-direction:                                */
/*       sidedist.y += deltadist.y                                            */
/*       map_y += step_y                                                      */
/*                                                                            */
/*   - This process repeats until a wall (`'1'`) is found or out of bounds.   */
/*                                                                            */
/* ************************************************************************** */
/*
static void	dda_loop(t_app *app, t_ray *ray)
{
	while (1)
	{
		if (ray->sidedist.x < ray->sidedist.y)
		{
			ray->sidedist.x += ray->deltadist.x;
			ray->map_x += ray->step.x;
			ray->side = 0;
		}
		else
		{
			ray->sidedist.y += ray->deltadist.y;
			ray->map_y += ray->step.y;
			ray->side = 1;
		}
		if (ray->map_y < 0
			|| ray->map_y >= app->game.map_height
			|| ray->map_x < 0
			|| ray->map_x >= (int)ft_strlen(app->game.map[ray->map_y])
			|| app->game.map[ray->map_y][ray->map_x] == '1')
			break ;
	}
}*/
static void	dda_loop(t_app *app, t_ray *ray)
{
	while (1)
	{
		if (ray->sidedist.x < ray->sidedist.y)
		{
			ray->sidedist.x += ray->deltadist.x;
			ray->map_x += ray->step.x;
			ray->side = 0;
		}
		else
		{
			ray->sidedist.y += ray->deltadist.y;
			ray->map_y += ray->step.y;
			ray->side = 1;
		}
		ray->hit_tile = safe_get_tile(&app->game, ray->map_x, ray->map_y);

        // Verificar si es una puerta cerrada y no se pinta 
        if (ray->hit_tile == '2' || ray->hit_tile == '3') {
            int is_closed = 1;
            for (int i = 0; i < app->game.door_count; i++) {
                t_door *door = &app->game.doors[i];
                if (door->x == ray->map_x && door->y == ray->map_y) {
					if(door->is_open && door->move_progress >= 0.2)
                    	is_closed = !door->is_open;
                    break;
                }
            }
            if (is_closed) break;
        }
        else if (ray->hit_tile == '1') {
            break;
        }
	}
}
/* ************************************************************************** */
/*                                                                            */
/*   Executes the full DDA (raycasting) routine for a single ray.             */
/*                                                                            */
/*   - Calls `init_step()` to determine stepping direction and initial        */
/*     side distances.                                                        */
/*                                                                            */
/*   - Runs `dda_loop()` to trace the ray through the grid until it hits      */
/*     a wall.                                                                */
/*                                                                            */
/*   - Computes the perpendicular distance to the wall (`perpwalldist`),      */
/*     which is essential for perspective projection.                         */
/*                                                                            */
/*     Formula:                                                               */
/*       perpwalldist = (sidedist_x - deltadist_x)/raydir_mod  (if hit		  */
/*       was on x-side)														  */
/*       perpwalldist = (sidedist_y - deltadist_y)/raydir_mod  (if hit		  */
/*       was on y-side)														  */
/*                                                                            */
/* ************************************************************************** */
void	do_dda(t_app *app, t_ray *ray)
{
	t_vec2	steps;

	steps = init_step(app->camera.pos.x, ray->map_x, ray->deltadist.x,
			ray->raydir.x);
	ray->step.x = steps.x;
	ray->sidedist.x = steps.y;
	steps = init_step(app->camera.pos.y, ray->map_y, ray->deltadist.y,
			ray->raydir.y);
	ray->step.y = steps.x;
	ray->sidedist.y = steps.y;
	dda_loop(app, ray);
	if (ray->side == 0)
		ray->perpwalldist = (ray->sidedist.x - ray->deltadist.x) / ray->raydir_mod;
	else
		ray->perpwalldist = (ray->sidedist.y - ray->deltadist.y) / ray->raydir_mod;
	//printf("sidedist.y: %lf, sidedist.x: %lf\n", ray->sidedist.y/ray->deltadist.y, ray->sidedist.x/ray->deltadist.x);
	//printf("raydir.y: %lf, raydir.x: %lf\n", ray->raydir.y, ray->raydir.x);
	//printf("product: %lf, normal: %lf\n", ray->perpwalldist*ray->raydir.y, ray->perpwalldist);
}

/* ************************************************************************** */
/*                                                                            */
/*   Computes the vertical drawing boundaries for the wall slice.             */
/*                                                                            */
/*   - Uses `perpwalldist` to determine the height of the projected wall:     */
/*       line_height = HEIGHT / (perpwalldist / raydir_modulus)               */

/*                                                                            */
/*   - Calculates the top (`draw_start`) and bottom (`draw_end`) positions    */
/*     on the screen, centering the wall slice:                               */
/*       draw_start = -line_height / 2 + HEIGHT / 2                           */
/*       draw_end = line_height / 2 + HEIGHT / 2                              */
/*                                                                            */
/*   - Ensures the values stay within screen limits.                          */
/*                                                                            */
/* ************************************************************************** */

