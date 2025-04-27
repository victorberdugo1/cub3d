/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 13:50:51 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 13:28:38 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Determines the step direction and initial side distance.                 */
/*                                                                            */
/*   - The step `ret.x` determines whether the ray moves left (-1) or right   */
/*     (+1) in the x-direction, and similarly for `ret.y` in the y-direction. */
/*                                                                            */
/*   - The initial `sidedist` represents the distance from the ray's origin   */
/*     to the first x-side or y-side. It is calculated as:                    */
/*                                                                            */
/*       sidedist_x = (map_x + 1 - pos_x) * deltadist_x   (moving right)      */
/*       sidedist_x = (pos_x - map_x) * deltadist_x       (moving left)       */
/*                                                                            */
/*     and similarly for `sidedist_y`.                                        */
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
/*   Advances the ray using the DDA (Digital Differential Analyzer) method.   */
/*                                                                            */
/*   - At each step, the ray moves towards the next x-side or y-side,         */
/*     depending on which `sidedist` is smaller.                              */
/*                                                                            */
/*   - If `sidedist.x < sidedist.y`, the ray moves one square along x:        */
/*       sidedist.x += deltadist.x                                            */
/*       map_x += step_x                                                      */
/*                                                                            */
/*   - Otherwise, the ray moves one square along y:                           */
/*       sidedist.y += deltadist.y                                            */
/*       map_y += step_y                                                      */
/*                                                                            */
/*   - The side (0 for x, 1 for y) is stored to know later the wall direction.*/
/*                                                                            */
/* ************************************************************************** */
static void	update_ray_position(t_ray *ray)
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
}

/* ************************************************************************** */
/*                                                                            */
/*   Loops DDA steps until a wall or a door collision is found.               */
/*                                                                            */
/*   - Keeps updating the ray position with `update_ray_position()`.          */
/*   - Stops when hitting a wall ('1') or a valid door ('2' or '3').          */
/*                                                                            */
/* ************************************************************************** */
static void	dda_loop(t_app *app, t_ray *ray)
{
	while (1)
	{
		update_ray_position(ray);
		ray->hit_tile = safe_get_tile(&app->game, ray->map_x, ray->map_y);
		if (ray->hit_tile == '1')
			break ;
		if ((ray->hit_tile == '2' || ray->hit_tile == '3')
			&& check_door_collision(app, ray))
			break ;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*  Digital Differential Analyzer (DDA) for ray traversal.                    */
/*                                                                            */
/*   - Initializes step directions and side distances using `init_step()`.    */
/*   - Traces the ray through the map using `dda_loop()`.                     */
/*   - Calculates the perpendicular wall distance for correct perspective.    */
/*                                                                            */
/*   Perpendicular wall distance formula:                                     */
/*                                                                            */
/*     If the wall was hit along x:                                           */
/*       perpwalldist = (sidedist_x - deltadist_x) / |raydir|                 */
/*                                                                            */
/*     If the wall was hit along y:                                           */
/*       perpwalldist = (sidedist_y - deltadist_y) / |raydir|                 */
/*                                                                            */
/*   where:                                                                   */
/*     - sidedist is the accumulated distance to the wall side.               */
/*     - deltadist is the distance between sides in that axis.                */
/*     - |raydir| is the ray's original direction vector magnitude.           */
/*                                                                            */
/* ************************************************************************** */
void	do_dda(t_app *app, t_ray *ray)
{
	t_vec2	steps;

	steps = init_step(app->cam.pos.x, ray->map_x, ray->deltadist.x,
			ray->raydir.x);
	ray->step.x = steps.x;
	ray->sidedist.x = steps.y;
	steps = init_step(app->cam.pos.y, ray->map_y, ray->deltadist.y,
			ray->raydir.y);
	ray->step.y = steps.x;
	ray->sidedist.y = steps.y;
	dda_loop(app, ray);
	if (ray->side == 0)
		ray->perpwalldist = (ray->sidedist.x - ray->deltadist.x)
			/ ray->raydir_mod;
	else
		ray->perpwalldist = (ray->sidedist.y - ray->deltadist.y)
			/ ray->raydir_mod;
}
