/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies_utils_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 17:29:36 by victor            #+#    #+#             */
/*   Updated: 2025/04/18 17:41:17 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

void	update_enemy_animation(t_enemy *e, double delta_time)
{
	e->time_since_last_move += delta_time;
	if (e->time_since_last_move > 0.2)
	{
		e->anim_frame = (e->anim_frame + 1) % 4;
		e->time_since_last_move = 0.0;
	}
}

static void	move_towards_cam(t_app *app, t_enemy *e, t_camera *cam, double dt)
{
	double	dx;
	double	dy;
	double	distance;
	double	move_x;
	double	move_y;

	dx = cam->pos.x - e->pos_x;
	dy = cam->pos.y - e->pos_y;
	distance = sqrt(dx * dx + dy * dy);
	move_x = 0.0;
	move_y = 0.0;
	if (distance > 1.0 && distance < 10.0)
	{
		move_x = (dx / distance) * e->speed * dt;
		move_y = (dy / distance) * e->speed * dt;
		if (!collides(&app->game, e->pos_x + move_x, e->pos_y + move_y))
		{
			e->pos_x += move_x;
			e->pos_y += move_y;
		}
	}
}

static void	calculate_push(t_enemy *e, t_camera *cam,
						double *push_x, double *push_y)
{
	double	ax;
	double	ay;
	double	dist_sq;
	double	min_dist;
	double	dist;

	ax = e->pos_x - cam->pos.x;
	ay = e->pos_y - cam->pos.y;
	dist_sq = ax * ax + ay * ay;
	min_dist = e->radius + COLLISION_RADIUS;
	if (dist_sq < (min_dist * min_dist) && dist_sq > 1e-8)
	{
		dist = sqrt(dist_sq);
		*push_x = (ax / dist) * (min_dist - dist + 0.001);
		*push_y = (ay / dist) * (min_dist - dist + 0.001);
	}
	else
	{
		*push_x = 0.0;
		*push_y = 0.0;
	}
}

static void	handle_enemy_collision(t_app *app, t_enemy *e, t_camera *cam)
{
	double	push_x;
	double	push_y;
	double	orig_x;
	double	orig_y;

	calculate_push(e, cam, &push_x, &push_y);
	if (push_x != 0.0 || push_y != 0.0)
	{
		orig_x = e->pos_x;
		orig_y = e->pos_y;
		e->pos_x += push_x;
		e->pos_y += push_y;
		if (collides(&app->game, e->pos_x, e->pos_y))
		{
			e->pos_x = orig_x;
			e->pos_y = orig_y;
			cam->pos.x -= push_x;
			cam->pos.y -= push_y;
		}
	}
}

void	update_enemy(t_app *app, t_enemy *e, t_camera *cam, double dt)
{
	update_enemy_dir(app, e, dt);
	update_enemy_animation(e, dt);
	move_towards_cam(app, e, cam, dt);
	handle_enemy_collision(app, e, cam);
}
