/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 14:53:46 by victor            #+#    #+#             */
/*   Updated: 2025/04/20 23:22:00 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

void	update_hit_feedback(t_app *app, double dt)
{
	if (!app->player_hit_feedback.active)
		return ;
	app->player_hit_feedback.timer += dt;
	if (app->player_hit_feedback.timer > app->player_hit_feedback.duration)
		app->player_hit_feedback.active = false;
}

// -----------------------------------------------------------------------------
// Mezcla dos colores 0xAARRGGBB con un alpha externo (0–255)
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Dibuja un tintado rojo semitransparente sobre la imagen entera
// -----------------------------------------------------------------------------
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

void	update_enemies(t_app *app, double delta_time)
{
	t_camera	*cam;
	int			i;

	apply_hit_to_enemy(app);
	cam = &app->camera;
	i = -1;
	while (++i < app->game.enemy_count)
	{
		if (app->game.enemies[i].is_active)
			update_enemy(app, &app->game.enemies[i], cam, delta_time);
	}
}

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
