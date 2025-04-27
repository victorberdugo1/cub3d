/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 14:53:46 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 12:16:53 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Manages red-screen flash timing after the player is hit.                 */
/*   - If feedback is inactive, does nothing.                                 */
/*   - Increments timer by dt while active.                                   */
/*   - Deactivates feedback when timer exceeds duration.                      */
/*                                                                            */
/* ************************************************************************** */
void	update_hit_feedback(t_app *app, double dt)
{
	if (!app->player_hit_feedback.active)
		return ;
	app->player_hit_feedback.timer += dt;
	if (app->player_hit_feedback.timer > app->player_hit_feedback.duration)
		app->player_hit_feedback.active = false;
}

/* ************************************************************************** */
/*                                                                            */
/*   Blends two ARGB colors with a given alpha.                               */
/*   - base and overlay are 0xAARRGGBB values.                                */
/*   - alpha in [0,255] is strength of overlay.                               */
/*   - Computes r,g,b = base*(1−a)+overlay*a; returns 0xFFRRGGBB.             */
/*                                                                            */
/* ************************************************************************** */
static uint32_t	blend_colors(uint32_t base, uint32_t overlay, int alpha)
{
	float	a;
	uint8_t	r;
	uint8_t	g;
	uint8_t	b;

	a = alpha / 255.0f;
	r = (uint8_t)(((base >> 16) & 0xFF) * (1.0f - a)
			+ ((overlay >> 16) & 0xFF) * a);
	g = (uint8_t)(((base >> 8) & 0xFF) * (1.0f - a)
			+ ((overlay >> 8) & 0xFF) * a);
	b = (uint8_t)(((base) & 0xFF) * (1.0f - a)
			+ ((overlay) & 0xFF) * a);
	return ((0xFF << 24) | (r << 16) | (g << 8) | b);
}

/* ************************************************************************** */
/*                                                                            */
/*   Applies a red tint overlay to the entire image when hit.                 */
/*   - Skips if no feedback active.                                           */
/*   - prog = timer / duration gives fade progress.                           */
/*   - alpha = (1 - prog) * 120 for overlay strength.                         */
/*   - Blends each pixel with red_overlay using blend_colors.                 */
/*                                                                            */
/* ************************************************************************** */
void	render_hit_feedback(t_app *app)
{
	int			alpha;
	double		prog;
	uint32_t	red_overlay;
	uint32_t	*p;
	uint32_t	*end;

	if (!app->player_hit_feedback.active)
		return ;
	prog = app->player_hit_feedback.timer
		/ app->player_hit_feedback.duration;
	alpha = (int)((1.0 - prog) * 120);
	red_overlay = 0x002222CC;
	p = (uint32_t *)app->image->pixels;
	end = p + app->image->width * app->image->height;
	while (p < end)
	{
		*p = blend_colors(*p, red_overlay, alpha);
		p++;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Updates all enemies each frame.                                          */
/*   - Calls apply_hit_to_enemy to register hits on enemies.                  */
/*   - For each active enemy, invokes update_enemy with camera and delta_time */
/*                                                                            */
/* ************************************************************************** */
void	update_enemies(t_app *app, double delta_time)
{
	t_camera	*cam;
	int			i;

	apply_hit_to_enemy(app);
	cam = &app->cam;
	i = -1;
	while (++i < app->game.enemy_count)
	{
		if (app->game.enemies[i].is_active)
			update_enemy(app, &app->game.enemies[i], cam, delta_time);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Applies a hit flash effect to a single enemy color.                      */
/*   - Increases red channel by 100 (clamped to 255).                         */
/*   - Halves green and blue channels.                                        */
/*   - Recombines channels into a 0xAARRGGBB color, preserving alpha.         */
/*                                                                            */
/* ************************************************************************** */
void	apply_hit_flash(uint32_t *color, t_enemy *e)
{
	uint8_t	r;
	uint8_t	g;
	uint8_t	b;

	r = *color & 0xFF;
	g = (*color >> 8) & 0xFF;
	b = (*color >> 16) & 0xFF;
	if (e->hit_flash > 0)
	{
		if (r + 100 > 255)
			r = 255;
		else
			r += 100;
		g = g * 0.5;
		b = b * 0.5;
		*color = (b << 16) | (g << 8) | r | (*color & 0xFF000000);
	}
}
