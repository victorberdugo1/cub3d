/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_bonus.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 19:05:57 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 12:37:05 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Determine attack animation frame range and delay based on weapon mode.   */
/*   - If alt_animation: frames 11–15; else frames 3–7.                       */
/*   - Delay between frames set to 0.05s.                                     */
/*                                                                            */
/* ************************************************************************** */
static void	weapon_attack_params(const t_weapon *w,
		int *start,
		int *end,
		double *delay)
{
	if (w->alt_animation)
	{
		*start = 11;
		*end = 15;
	}
	else
	{
		*start = 3;
		*end = 7;
	}
	*delay = 0.05;
}

/* ************************************************************************** */
/*                                                                            */
/*   Determine idle animation frame range and delay based on weapon mode.     */
/*   - If alt_animation: frames 8–10; else frames 0–2.                        */
/*   - Delay between frames set to 0.1s.                                      */
/*                                                                            */
/* ************************************************************************** */
static void	weapon_idle_params(const t_weapon *w,
		int *start,
		int *end,
		double *delay)
{
	if (w->alt_animation)
	{
		*start = 8;
		*end = 10;
	}
	else
	{
		*start = 0;
		*end = 2;
	}
	*delay = 0.1;
}

/* ************************************************************************** */
/*                                                                            */
/*   Compute number of animation steps to advance this frame.                 */
/*   - steps = floor(frame_timer / delay).                                    */
/*   - frame_timer wrapped modulo delay for leftover time.                    */
/*                                                                            */
/* ************************************************************************** */
static int	weapon_compute_steps(t_weapon *w, double delay)
{
	int	steps;

	steps = (int)(w->frame_timer / delay);
	w->frame_timer = fmod(w->frame_timer, delay);
	return (steps);
}

/* ************************************************************************** */
/*                                                                            */
/*   Advance current_frame by steps within [start,end] loop.                  */
/*   - If current_frame exceeds end:                                          */
/*       • If attacking, stop attack, set to end, and return ended=true.      */
/*       • Else wrap to start.                                                */
/*   - Returns true if an attack animation just ended.                        */
/*                                                                            */
/* ************************************************************************** */
static bool	weapon_advance_loop(t_weapon *w, int start, int end, int steps)
{
	bool	ended;
	int		i;

	ended = false;
	i = -1;
	while (++i < steps)
	{
		w->current_frame++;
		if (w->current_frame > end)
		{
			if (w->is_attacking)
			{
				w->is_attacking = false;
				w->current_frame = end;
				ended = true;
			}
			else
			{
				w->current_frame = start;
			}
		}
	}
	return (ended);
}

/* ************************************************************************** */
/*                                                                            */
/*   Update weapon animation each frame:                                      */
/*   - Initialize weapon state if needed.                                     */
/*   - Increment frame_timer by delta_time.                                   */
/*   - Select params (attack or idle) based on is_attacking.                  */
/*   - Clamp current_frame to selected range.                                 */
/*   - Advance frames using computed steps; reset timer if attack ended.      */
/*                                                                            */
/* ************************************************************************** */
void	update_weapon_animation(t_app *app, double delta_time)
{
	t_weapon	*w;
	static bool	initialized = false;
	int			range[2];
	double		delay;

	w = &app->weapon;
	weapon_initialize(w, &initialized);
	w->frame_timer = w->frame_timer + delta_time;
	if (w->is_attacking)
		weapon_attack_params(w, &range[0], &range[1], &delay);
	else
		weapon_idle_params(w, &range[0], &range[1], &delay);
	if (w->current_frame < range[0] || w->current_frame > range[1])
	{
		w->current_frame = range[0];
		w->frame_timer = 0.0;
	}
	if (weapon_advance_loop(w, range[0], range[1],
			weapon_compute_steps(w, delay)))
		w->frame_timer = 0.0;
}
