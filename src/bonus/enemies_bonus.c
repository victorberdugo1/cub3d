/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 14:53:46 by victor            #+#    #+#             */
/*   Updated: 2025/04/18 17:40:57 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

void	update_enemy_dir(t_app *app, t_enemy *enemy, double delta_time)
{
	const double	target = atan2(app->camera.pos.y - enemy->pos_y,
			app->camera.pos.x - enemy->pos_x);
	double			angle_diff;

	if (!enemy->initialized)
	{
		enemy->facing_angle = target;
		enemy->initialized = true;
	}
	angle_diff = fmod((target - enemy->facing_angle) + M_PI, 2 * M_PI) - M_PI;
	if (angle_diff > 1.0 * delta_time)
		angle_diff = 1.0 * delta_time;
	else if (angle_diff < -1.0 * delta_time)
		angle_diff = -1.0 * delta_time;
	enemy->facing_angle += angle_diff;
	angle_diff = fmod(target - enemy->facing_angle + M_PI, 2 * M_PI) - M_PI;
	if (fabs(angle_diff) < M_PI_4)
		enemy->e_dir = FRONT;
	else if (angle_diff > M_PI_4 && angle_diff < 3 * M_PI_4)
		enemy->e_dir = LEFT;
	else if (angle_diff < -M_PI_4 && angle_diff > -3 * M_PI_4)
		enemy->e_dir = RIGHT;
	else
		enemy->e_dir = BACK;
}

void	update_enemies(t_app *app, double delta_time)
{
	t_camera	*cam;
	int			i;

	cam = &app->camera;
	i = -1;
	while (++i < app->game.enemy_count)
	{
		if (app->game.enemies[i].is_active)
			update_enemy(app, &app->game.enemies[i], cam, delta_time);
	}
}

static void	draw_enemy_stripe(t_app *app, t_draw_data *d, int x)
{
	int			y;
	int			tx;
	int			ty;
	uint32_t	color;

	if (x < 0 || x >= WIDTH || d->transform_y > app->z_buffer[x])
		return ;
	y = d->start_y - 1;
	while (++y < d->end_y)
	{
		if (y >= 0 && y < HEIGHT)
		{
			tx = (int)((float)(x - d->start_x)
					/ (d->end_x - d->start_x) * d->width);
			ty = (int)((float)(y - d->start_y)
					/ (d->end_y - d->start_y) * d->height);
			if (tx >= 0 && tx < d->width && ty >= 0 && ty < d->height)
			{
				color = ((uint32_t *)app->game.tex_enemy->pixels)
				[(d->offset_y + ty) * d->tex_width + (d->offset_x + tx)];
				if ((color & 0xFF000000) != 0)
					mlx_put_pixel(app->image, x, y, convert_pixel(color));
			}
		}
	}
}

static void	draw_enemy_sprite(t_app *app, t_enemy *e,
		int screen_x, double transform_y)
{
	t_draw_data	d;
	int			x;

	d.sprite_height = abs((int)(HEIGHT / transform_y));
	d.sprite_width = d.sprite_height;
	d.start_y = -d.sprite_height / 2 + HEIGHT / 2 - app->camera.view_z;
	d.end_y = d.sprite_height / 2 + HEIGHT / 2 - app->camera.view_z;
	d.start_x = -d.sprite_width / 2 + screen_x;
	d.end_x = d.sprite_width / 2 + screen_x;
	d.tex_width = app->game.tex_enemy->width;
	d.tex_height = app->game.tex_enemy->height;
	d.width = d.tex_width / 4;
	d.height = d.tex_height / 4;
	d.offset_x = e->anim_frame * d.width;
	d.offset_y = e->e_dir * d.height;
	d.transform_y = transform_y;
	x = d.start_x;
	while (x < d.end_x)
	{
		draw_enemy_stripe(app, &d, x);
		x++;
	}
}

void	render_enemy(t_app *app, t_enemy *e)
{
	t_vec2	rel;
	t_vec2	transform;
	double	inv_det;
	int		screen_x;

	rel.x = e->pos_x - app->camera.pos.x;
	rel.y = e->pos_y - app->camera.pos.y;
	inv_det = 1.0 / (app->camera.plane.x * app->camera.dir.y
			- app->camera.dir.x * app->camera.plane.y);
	transform.x = inv_det * (app->camera.dir.y * rel.x
			- app->camera.dir.x * rel.y);
	transform.y = inv_det * (-app->camera.plane.y * rel.x
			+ app->camera.plane.x * rel.y);
	if (transform.y <= 0)
		return ;
	screen_x = (int)((WIDTH / 2) * (1 + transform.x / transform.y));
	draw_enemy_sprite(app, e, screen_x, transform.y);
}
