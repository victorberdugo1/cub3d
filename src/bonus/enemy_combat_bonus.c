/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemy_combat_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 22:50:54 by victor            #+#    #+#             */
/*   Updated: 2025/04/25 13:43:58 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

void	process_knockback(t_app *a, t_enemy *e, double dt)
{
	double			new_x;
	double			new_y;
	const double	force = 2.5 * dt;

	new_x = e->pos_x + e->knockback_dir.x * force;
	new_y = e->pos_y + e->knockback_dir.y * force;
	if (!collides(&a->game, new_x, new_y))
	{
		e->pos_x = new_x;
		e->pos_y = new_y;
	}
}

static void	apply_knockback(t_app *app, t_enemy *e, t_vec2 hit_dir)
{
	double	length;
	double	new_x;
	double	new_y;

	length = sqrt(hit_dir.x * hit_dir.x + hit_dir.y * hit_dir.y);
	if (length > 0)
	{
		hit_dir.x /= length;
		hit_dir.y /= length;
	}
	new_x = e->pos_x + hit_dir.x * 0.3;
	new_y = e->pos_y + hit_dir.y * 0.3;
	if (!collides(&app->game, new_x, new_y))
	{
		e->pos_x = new_x;
		e->pos_y = new_y;
	}
	e->hit_flash = 5;
	e->knockback_time = 0.2;
	e->knockback_dir = hit_dir;
}

static void	process_hit_on_enemy(t_app *app, t_enemy *e)
{
	t_vec2	d;
	t_vec2	hit_dir;
	double	dist;

	d.x = e->pos_x - app->cam.pos.x;
	d.y = e->pos_y - app->cam.pos.y;
	dist = sqrt(d.x * d.x + d.y * d.y);
	if (dist >= 1.2)
		return ;
	hit_dir.x = d.x;
	hit_dir.y = d.y;
	apply_knockback(app, e, hit_dir);
	e->hit_count++;
	if (e->hit_count >= 3 && e->e_state != ENEMY_DEAD)
	{
		e->e_state = ENEMY_DEAD;
		e->anim_frame = 2;
		e->anim_timer = 0;
	}
	else
	{
		e->e_state = ENEMY_HIT;
		e->anim_frame = 2;
		e->anim_timer = 0.0;
	}
}

void	apply_hit_to_enemy(t_app *app)
{
	static double	last_hit_time = 0;
	static bool		prev_1 = false;
	int				i;

	if (mlx_is_key_down(app->mlx, MLX_KEY_Q))
	{
		if (!app->weapon.is_attacking)
		{
			app->weapon.is_attacking = true;
			app->weapon.current_frame = 3;
		}
		if ((mlx_get_time() - last_hit_time) > 0.5)
		{
			last_hit_time = mlx_get_time();
			i = -1;
			while (++i < app->game.enemy_count)
				if (app->game.enemies[i].is_active
					&& app->game.enemies[i].e_state != ENEMY_DEAD)
					process_hit_on_enemy(app, &app->game.enemies[i]);
		}
	}
	if (mlx_is_key_down(app->mlx, MLX_KEY_1) && !prev_1)
		app->weapon.alt_animation = !app->weapon.alt_animation;
	prev_1 = mlx_is_key_down(app->mlx, MLX_KEY_1);
}

void	check_enemy_attack_hit(t_app *app, t_enemy *e, t_camera *cam)
{
	double	dx;
	double	dy;
	double	dist;

	if (e->e_state != ENEMY_ATTACK)
		return ;
	dx = cam->pos.x - e->pos_x;
	dy = cam->pos.y - e->pos_y;
	dist = sqrt(dx * dx + dy * dy);
	if (dist < 1.2)
	{
		app->player_hit_feedback.active = true;
		app->player_hit_feedback.timer = 0;
		app->player_hit_feedback.duration = 0.4;
	}
}
