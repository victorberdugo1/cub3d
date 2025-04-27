/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemy_movement_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 22:23:00 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 12:31:48 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Compute push vector to separate enemy and camera on collision overlap.   */
/*   - ax,ay: vector from camera to enemy.                                    */
/*   - dist_sq: squared distance.                                             */
/*   - min_dist: sum of radii for no-overlap.                                 */
/*   - If dist_sq < min_dist² and >0:                                         */
/*       • dist = sqrt(dist_sq).                                              */
/*       • push = unit(ax,ay)*(min_dist–dist+0.001).                          */
/*   - Else push = (0,0).                                                     */
/*                                                                            */
/* ************************************************************************** */
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

/* ************************************************************************** */
/*                                                                            */
/*   Resolve enemy vs camera collision by applying push with collision check. */
/*   - Get push_x,push_y from calculate_push.                                 */
/*   - If non-zero push:                                                      */
/*       • Save original pos.                                                 */
/*       • Move enemy by push.                                                */
/*       • If collides with map, revert enemy pos and push camera back.       */
/*                                                                            */
/* ************************************************************************** */
void	handle_enemy_collision(t_app *app, t_enemy *e, t_camera *cam)
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

/* ************************************************************************** */
/*                                                                            */
/*   Move enemy toward camera within range limits.                            */
/*   - Compute dx,dy and distance to camera.                                  */
/*   - Skip if distance <1 or >10 units.                                      */
/*   - Compute move_x,move_y using facing_angle and speed*dt.                 */
/*   - If new pos not colliding, apply move.                                  */
/*                                                                            */
/* ************************************************************************** */
void	move_towards_cam(t_app *app, t_enemy *e, t_camera *cam, double dt)
{
	double	dx;
	double	dy;
	double	distance;
	double	move_x;
	double	move_y;

	dx = cam->pos.x - e->pos_x;
	dy = cam->pos.y - e->pos_y;
	distance = sqrt(dx * dx + dy * dy);
	if (distance < 1.0 || distance > 10.0)
		return ;
	move_x = cos(e->facing_angle) * e->speed * dt;
	move_y = sin(e->facing_angle) * e->speed * dt;
	if (!collides(&app->game, e->pos_x + move_x, e->pos_y + move_y))
	{
		e->pos_x += move_x;
		e->pos_y += move_y;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Map a signed angle difference to one of eight facing directions.         */
/*   - FRONT if |angle_diff| ≤ π/8.                                           */
/*   - FRONT_RIGHT if in (π/8,3π/8], RIGHT if in (3π/8,5π/8],                 */
/*     BACK_RIGHT if in (5π/8,7π/8], BACK if |angle_diff|>7π/8,               */
/*     BACK_LEFT, LEFT, FRONT_LEFT otherwise in respective octants.           */
/*                                                                            */
/* ************************************************************************** */
static void	determine_enemy_direction(t_enemy *enemy, double angle_diff)
{
	if (angle_diff > -M_PI / 8 && angle_diff <= M_PI / 8)
		enemy->e_dir = FRONT;
	else if (angle_diff > M_PI / 8 && angle_diff <= 3 * M_PI / 8)
		enemy->e_dir = FRONT_RIGHT;
	else if (angle_diff > 3 * M_PI / 8 && angle_diff <= 5 * M_PI / 8)
		enemy->e_dir = RIGHT;
	else if (angle_diff > 5 * M_PI / 8 && angle_diff <= 7 * M_PI / 8)
		enemy->e_dir = BACK_RIGHT;
	else if (angle_diff > 7 * M_PI / 8 || angle_diff <= -7 * M_PI / 8)
		enemy->e_dir = BACK;
	else if (angle_diff <= -5 * M_PI / 8 && angle_diff > -7 * M_PI / 8)
		enemy->e_dir = BACK_LEFT;
	else if (angle_diff <= -3 * M_PI / 8 && angle_diff > -5 * M_PI / 8)
		enemy->e_dir = LEFT;
	else
		enemy->e_dir = FRONT_LEFT;
}

/* ************************************************************************** */
/*                                                                            */
/*   Update enemy facing angle and facing direction toward the camera.        */
/*   - target = atan2(cam.pos - enemy.pos).                                   */
/*   - On first call, set facing_angle = target and mark initialized.         */
/*   - max_turn = delta_time radians allowed this frame.                      */
/*   - Compute minimal angle_diff ∈(–π,π] via fmod adjustment.                */
/*   - Clamp angle_diff to ±max_turn and apply to facing_angle.               */
/*   - Recompute angle_diff and determine e_dir via helper.                   */
/*                                                                            */
/* ************************************************************************** */
void	update_enemy_dir(t_app *app, t_enemy *enemy, double delta_time)
{
	const double	target = atan2(app->cam.pos.y - enemy->pos_y,
			app->cam.pos.x - enemy->pos_x);
	double			angle_diff;
	double			max_turn;

	if (!enemy->initialized)
	{
		enemy->facing_angle = target;
		enemy->initialized = true;
	}
	max_turn = delta_time;
	angle_diff = fmod(target - enemy->facing_angle + M_PI, 2 * M_PI) - M_PI;
	if (angle_diff > max_turn)
		angle_diff = max_turn;
	else if (angle_diff < -max_turn)
		angle_diff = -max_turn;
	enemy->facing_angle += angle_diff;
	angle_diff = fmod(target - enemy->facing_angle + M_PI, 2 * M_PI) - M_PI;
	determine_enemy_direction(enemy, angle_diff);
}
