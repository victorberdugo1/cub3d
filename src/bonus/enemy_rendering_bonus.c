/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemy_rendering_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 23:07:08 by victor            #+#    #+#             */
/*   Updated: 2025/04/22 01:30:17 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

static void	set_enemy_texture(t_draw_data *d, t_enemy *e,
								bool *flip, int *col)
{
	const int				state_row = 4;
	static const int		cols[8] = {0, 1, 2, 3, 4, 3, 2, 1};

	*flip = false;
	if (e->e_state == ENEMY_ATTACK
		|| e->e_state == ENEMY_HIT
		|| e->e_state == ENEMY_DEAD)
	{
		d->offset_y = state_row * d->hgt;
		d->offset_x = e->anim_frame * d->width;
		return ;
	}
	*col = cols[e->e_dir];
	if (e->e_dir == FRONT_RIGHT
		|| e->e_dir == RIGHT
		|| e->e_dir == BACK_RIGHT)
		*flip = true;
	d->offset_x = *col * d->width;
	d->offset_y = e->anim_frame * d->hgt;
}

static void	init_draw_data(t_app *app, t_draw_data *d,
		double transform_y, int screen_x)
{
	d->sprite_height = abs((int)(HEIGHT / transform_y));
	d->sprite_width = d->sprite_height;
	d->st_y = -d->sprite_height / 2 + HEIGHT / 2 - app->cam.view_z;
	d->end_y = d->sprite_height / 2 + HEIGHT / 2 - app->cam.view_z;
	d->st_x = -d->sprite_width / 2 + screen_x;
	d->end_x = d->sprite_width / 2 + screen_x;
	d->tex_width = app->game.tex_enemy->width;
	d->tex_height = app->game.tex_enemy->height;
	d->width = d->tex_width / 5;
	d->hgt = d->tex_height / 5;
	d->transform_y = transform_y;
}

static void	draw_enemy_stripe(t_app *app, t_enemy *e,
								t_draw_data *d, int x)
{
	int			t[3];
	uint32_t	color;
	bool		flip;

	flip = (e->e_dir == RIGHT || e->e_dir == BACK_RIGHT
			|| e->e_dir == FRONT_RIGHT);
	if (x < 0 || x >= WIDTH || d->transform_y > app->z_buffer[x])
		return ;
	t[2] = d->st_y - 1;
	while (++t[2] < d->end_y)
	{
		if (t[2] < 0 || t[2] >= HEIGHT)
			continue ;
		t[0] = (int)((float)(x - d->st_x) / (d->end_x - d->st_x) * d->width);
		t[1] = (int)((float)(t[2] - d->st_y) / (d->end_y - d->st_y) * d->hgt);
		if (t[0] < 0 || t[0] >= d->width || t[1] < 0 || t[1] >= d->hgt)
			continue ;
		if (flip)
			t[0] = d->width - 1 - t[0];
		color = ((uint32_t *)app->game.tex_enemy->pixels)
		[(d->offset_y + t[1]) * d->tex_width + (d->offset_x + t[0])];
		apply_hit_flash(&color, e);
		if ((color & 0xFF000000) != 0)
			mlx_put_pixel(app->image, x, t[2], convert_pixel(color));
	}
}

static void	draw_enemy_sprite(t_app *app, t_enemy *e,
							int screen_x, double transform_y)
{
	t_draw_data	d;
	bool		flip;
	int			col;
	int			x;

	init_draw_data(app, &d, transform_y, screen_x);
	set_enemy_texture(&d, e, &flip, &col);
	x = d.st_x - 1;
	while (++x < d.end_x)
		draw_enemy_stripe(app, e, &d, x);
}

void	render_enemy(t_app *app, t_enemy *e)
{
	t_vec2	rel;
	t_vec2	transform;
	double	inv_det;
	int		screen_x;

	rel.x = e->pos_x - app->cam.pos.x;
	rel.y = e->pos_y - app->cam.pos.y;
	inv_det = 1.0 / (app->cam.plane.x * app->cam.dir.y
			- app->cam.dir.x * app->cam.plane.y);
	transform.x = inv_det * (app->cam.dir.y * rel.x
			- app->cam.dir.x * rel.y);
	transform.y = inv_det * (-app->cam.plane.y * rel.x
			+ app->cam.plane.x * rel.y);
	if (transform.y <= 0)
		return ;
	screen_x = (int)((WIDTH / 2) * (1 + transform.x / transform.y));
	draw_enemy_sprite(app, e, screen_x, transform.y);
}
