/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 20:51:23 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 12:11:12 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Iterates all doors and checks if the ray hits a closed or opening door.  */
/*   - Compares ray map coordinates with each door’s grid position.           */
/*   - If matched, returns 0 when the door is fully open (move_progress>=0.2) */
/*     else returns 1 to indicate collision.                                  */
/*   - If no door matches, returns 1 (no blocking door at this hit).          */
/*                                                                            */
/* ************************************************************************** */
int	check_door_collision(t_app *app, t_ray *ray)
{
	int	i;

	i = -1;
	while (++i < app->game.door_count)
	{
		if (app->game.doors[i].x == ray->map_x
			&& app->game.doors[i].y == ray->map_y)
			return (!(app->game.doors[i].is_open
					&& app->game.doors[i].move_progress >= 0.2));
	}
	return (1);
}

/* ************************************************************************** */
/*                                                                            */
/*   Advances door animations over time.                                      */
/*   - For each door with move_progress < DOOR_ANIM_DURATION:                 */
/*     • Increment move_progress by dt, clamped to duration.                  */
/*     • Compute t = move_progress / duration in [0,1].                       */
/*     • If is_open, open_offset = t; otherwise open_offset = 1 - t.          */
/*   - open_offset is used to interpolate door position in rendering.         */
/*                                                                            */
/* ************************************************************************** */
void	update_door_animation(t_app *app, double dt)
{
	int		i;
	double	t;
	t_door	*door;

	i = -1;
	while (++i < app->game.door_count)
	{
		door = &app->game.doors[i];
		if (door->move_progress < DOOR_ANIM_DURATION)
		{
			door->move_progress += dt;
			if (door->move_progress > DOOR_ANIM_DURATION)
				door->move_progress = DOOR_ANIM_DURATION;
			t = door->move_progress / DOOR_ANIM_DURATION;
			if (door->is_open)
				door->open_offset = t;
			else
				door->open_offset = 1.0 - t;
		}
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Marks a door as opening when activated.                                  */
/*   - Searches doors matching the ray’s grid coordinates.                    */
/*   - Sets is_open to true and resets move_progress to 0 to start animating. */
/*   - Breaks after the first matching door is toggled.                       */
/*                                                                            */
/* ************************************************************************** */
static void	try_open_door(t_app *app, t_ray *ray)
{
	int		i;
	t_door	*door;

	i = -1;
	while (++i < app->game.door_count)
	{
		door = &app->game.doors[i];
		if (door->x == ray->map_x && door->y == ray->map_y)
		{
			door->is_open = 1;
			door->move_progress = 0.0;
			break ;
		}
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Handles door opening input each frame.                                   */
/*   - Detects a new Space key press (debounced via last_space).              */
/*   - Casts a ray from screen center to find a door within 1.5 units.        */
/*   - If the hit tile is a door identifier ('2' or '3'), calls try_open_door.*/
/*   - Updates last_space to avoid repeated triggers while held.              */
/*                                                                            */
/* ************************************************************************** */
void	toggle_doors(t_app *app)
{
	static int	last_space = 0;
	int			current_space;
	t_ray		ray;

	current_space = mlx_is_key_down(app->mlx, MLX_KEY_SPACE);
	if (current_space && !last_space)
	{
		init_ray(app, WIDTH / 2, &ray);
		do_dda(app, &ray);
		if (ray.perpwalldist < 1.5
			&& (ray.hit_tile == '2' || ray.hit_tile == '3'))
			try_open_door(app, &ray);
	}
	last_space = current_space;
}
