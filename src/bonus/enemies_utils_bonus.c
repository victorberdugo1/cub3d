/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies_utils_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 17:29:36 by victor            #+#    #+#             */
/*   Updated: 2025/04/20 23:34:52 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

static void	handle_special_states(t_enemy *e)
{
	if (e->e_state == ENEMY_ATTACK && e->anim_timer > 0.15)
	{
		e->anim_frame = (e->anim_frame + 1) % 3;
		e->anim_timer = 0.0;
		if (e->anim_frame == 2)
		{
			e->e_state = ENEMY_COOLDOWN;
			e->attack_cooldown = 1.0;
		}
	}
	else if (e->e_state == ENEMY_DEAD)
	{
		if (e->anim_frame == 2 && e->anim_timer > 0.175)
		{
			e->anim_frame = 3;
			e->anim_timer = 0.0;
		}
		else if (e->anim_frame == 3 && e->anim_timer > 0.175)
		{
			e->anim_frame = 4;
			e->anim_timer = 0.0;
		}
	}
}

void	update_enemy_animation(t_enemy *e, double delta_time)
{
	e->anim_timer += delta_time;
	if (e->knockback_time > 0)
		e->knockback_time -= delta_time;
	if (e->hit_flash > 0)
		e->hit_flash--;
	handle_special_states(e);
	if (e->e_state == ENEMY_HIT && e->anim_timer > 0.2)
	{
		e->e_state = ENEMY_MOVE;
		e->anim_timer = 0.0;
	}
	else if (e->e_state == ENEMY_COOLDOWN)
	{
		e->attack_cooldown -= delta_time;
		if (e->attack_cooldown <= 0)
			e->e_state = ENEMY_MOVE;
	}
	else if (e->anim_timer > 0.2 && e->e_state != ENEMY_DEAD)
	{
		e->anim_frame = (e->anim_frame + 1) % 4;
		e->anim_timer = 0.0;
	}
}

void	update_enemy(t_app *a, t_enemy *e, t_camera *c, double dt)
{
	const double	dist = hypot(c->pos.x - e->pos_x, c->pos.y - e->pos_y);

	update_enemy_dir(a, e, dt);
	if (dist < 1.0 && e->e_dir == FRONT && e->e_state == ENEMY_MOVE)
	{
		e->e_state = ENEMY_ATTACK;
		e->anim_frame = 0;
		e->anim_timer = 0.0;
	}
	else if (e->knockback_time > 0.0)
		process_knockback(a, e, dt);
	else if (e->e_state == ENEMY_MOVE)
		move_towards_cam(a, e, c, dt);
	if (e->e_state == ENEMY_ATTACK)
		check_enemy_attack_hit(a, e, c);
	update_enemy_animation(e, dt);
	if (e->e_state != ENEMY_DEAD)
		handle_enemy_collision(a, e, c);
}
