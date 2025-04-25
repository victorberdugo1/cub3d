/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_bonus.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 19:05:57 by victor            #+#    #+#             */
/*   Updated: 2025/04/25 15:52:39 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

uint32_t	fog(const int *color, double r, double fog_dens, double min_factor)
{
	double	fog_factor;
	int		dr;
	int		dg;
	int		db;

	fog_factor = 1.0 / (1.0 + fog_dens * r);
	if (fog_factor < min_factor)
		fog_factor = min_factor;
	dr = (int)(color[0] * fog_factor);
	dg = (int)(color[1] * fog_factor);
	db = (int)(color[2] * fog_factor);
	return (ft_pixel(dr, dg, db, 255));
}

void	update_weapon_animation(t_app *app, double delta_time)
{
	t_weapon *w = &app->weapon;
	static bool initialized = false;

	if (!initialized)
	{
		w->total_idle_frames   = 3;
		w->total_attack_frames = 5;
		w->alt_animation       = false;
		initialized = true;
	}
	int idle_start, idle_end;
	int attack_start, attack_end;
	double frame_delay;
	if (w->alt_animation)
	{
		idle_start   = 8;
		idle_end     = 10;
		attack_start = 11;
		attack_end   = 15;
	}
	else
	{
		idle_start   = 0;
		idle_end     = 2;
		attack_start = 3;
		attack_end   = 7;
	}
	const double attack_frame_time = 0.05;
	const double idle_frame_time = 0.1;
	w->frame_timer += delta_time;
	int current_start, current_end;
	if (w->is_attacking)
	{
		current_start = attack_start;
		current_end = attack_end;
		frame_delay = attack_frame_time;
	}
	else
	{
		current_start = idle_start;
		current_end = idle_end;
		frame_delay = idle_frame_time;
	}
	if (w->current_frame < current_start || w->current_frame > current_end)
	{
		w->current_frame = current_start;
		w->frame_timer = 0.0;
	}
	int frames_to_advance = (int)(w->frame_timer / frame_delay);
	if (frames_to_advance > 0)
	{
		w->frame_timer = fmod(w->frame_timer, frame_delay);
		for (int i = 0; i < frames_to_advance; i++)
		{
			w->current_frame++;
			if (w->current_frame > current_end)
			{
				if (w->is_attacking)
				{
					w->is_attacking = false;
					w->current_frame = idle_start;
					current_end = idle_end;
				}
				else
				{
					w->current_frame = current_start;
				}
			}
		}
	}
}

void render_weapon(t_app *app)
{
	t_weapon *w = &app->weapon;
	static const int COLS = 4;
	static const int ROWS = 4;
	static const float SCALE = 1.8f;
	static const int Y_OFFSET = 10;

	if (!w->texture) return;

	const int rendered_h = HEIGHT * SCALE;
	const int rendered_w = (w->texture->width * rendered_h) / w->texture->height;
	const int pos_x = WIDTH / 2 - rendered_w / 2;
	const int pos_y = HEIGHT - rendered_h + Y_OFFSET;

	const int frame_w = w->texture->width / COLS;
	const int frame_h = w->texture->height / ROWS;
	const int frame_col = w->current_frame % COLS;
	const int frame_row = w->current_frame / COLS;
	const int frame_x_offset = frame_col * frame_w;
	const int frame_y_offset = frame_row * frame_h;

	const float scale_x = (float)frame_w / rendered_w;
	const float scale_y = (float)frame_h / rendered_h;

	uint32_t *pixels = (uint32_t *)w->texture->pixels;

	for (int y = 0; y < rendered_h; y++)
	{
		int screen_y = pos_y + y;
		if (screen_y < 0 || screen_y >= HEIGHT)
			continue;

		uint32_t tex_y = (int)(y * scale_y) + frame_y_offset;
		if (tex_y >= w->texture->height)
			continue;

		for (int x = 0; x < rendered_w; x++)
		{
			int screen_x = pos_x + x;
			if (screen_x < 0 || screen_x >= WIDTH)
				continue;

			uint32_t tex_x = (int)(x * scale_x) + frame_x_offset;
			if (tex_x >= w->texture->width)
				continue;

			uint32_t color = pixels[tex_y * w->texture->width + tex_x];

			if ((color & 0xFF000000) != 0)
				mlx_put_pixel(app->image, screen_x, screen_y, convert_pixel(color));
		}
	}
}
