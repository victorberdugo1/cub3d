/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies_utils_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 17:29:36 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 12:23:40 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Advance enemy attack and death animation states.                         */
/*   - If in ATTACK state and timer > 0.15s:                                  */
/*       • Cycle through attack frames 0→1→2.                                 */
/*       • On frame 2, switch to COOLDOWN and set attack_cooldown to 0.6s.    */
/*   - If in DEAD state:                                                      */
/*       • After frame 2 and 0.175s, advance to frame 3.                      */
/*       • After frame 3 and 0.175s, advance to frame 4 (final death pose).   */
/*                                                                            */
/* ************************************************************************** */
static void	handle_special_states(t_enemy *e)
{
	if (e->e_state == ENEMY_ATTACK && e->anim_timer > 0.15)
	{
		e->anim_frame = (e->anim_frame + 1) % 3;
		e->anim_timer = 0.0;
		if (e->anim_frame == 2)
		{
			e->e_state = ENEMY_COOLDOWN;
			e->attack_cooldown = 0.6;
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

/* ************************************************************************** */
/*                                                                            */
/*   Update animation timers and state transitions for one enemy.             */
/*   - Advance anim_timer by delta_time.                                      */
/*   - Decrease knockback_time and hit_flash counters if positive.            */
/*   - Handle ATTACK/DEAD special states via helper.                          */
/*   - If in HIT state and timer > 0.2s: switch back to MOVE and reset timer. */
/*   - If in COOLDOWN state: decrement attack_cooldown and resume MOVE once   */
/*     it reaches zero.                                                       */
/*   - If not DEAD and timer > 0.2s: advance walk animation frame and set     */
/*     anim_timer to 0.075s for next frame.                                   */
/*                                                                            */
/* ************************************************************************** */
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
		e->anim_timer = 0.075;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Update enemy behavior each frame:                                        */
/*   - Compute distance to camera.                                            */
/*   - Update facing direction.                                               */
/*   - If within 1.0 unit and facing front in MOVE state: start ATTACK.       */
/*   - Else if knockback_time > 0: apply knockback movement.                  */
/*   - Else if in MOVE state: advance towards camera.                         */
/*   - If in ATTACK state: check for hit on player.                           */
/*   - Update animation and resolve collisions.                               */
/*                                                                            */
/* ************************************************************************** */
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
	handle_enemy_collision(a, e, c);
}
