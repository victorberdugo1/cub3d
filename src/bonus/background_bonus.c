/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   background_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 02:02:47 by victor            #+#    #+#             */
/*   Updated: 2025/04/22 02:05:05 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

static inline bool	handle_edge_case(t_app *app, int x, int y, t_vec2 frac)
{
	int	*c;

	if (frac.x < 0.005 || frac.x > 0.995 || frac.y < 0.005 || frac.y > 0.995)
	{
		c = app->game.ceiling_color;
		put_pixel_safe(app->image, x, y, ft_pixel(c[0] * 0.8,
				c[1] * 0.8, c[2] * 0.8, 255));
		return (true);
	}
	return (false);
}

static inline bool	handle_light_panel(t_app *app,
	const bool pattern[15][15], t_ceiling *d)
{
	int		*c;
	double	dx;
	double	dy;
	double	i;

	if (!pattern[d->col.i][d->col.j])
		return (false);
	dx = fabs(d->col.dx - 0.5);
	dy = fabs(d->col.dy - 0.5);
	if (dx < 0.3338 && dy < 0.3338)
		return (put_pixel_safe(app->image, d->x, d->y,
				ft_pixel(255, 255, 255, 255)), true);
	if (dx < 0.4338 && dy < 0.4338)
	{
		i = 1.0 - (fmax(dx, dy) - 0.3338) * 10.0;
		if (i > 0.0)
		{
			c = app->game.ceiling_color;
			put_pixel_safe(app->image, d->x, d->y,
				ft_pixel(c[0] + (int)((255 - c[0]) * i), c[1] + (int)((255
							- c[1]) * i), c[2] + (int)((255 - c[2]) * i), 255));
			return (true);
		}
	}
	return (false);
}

static void	draw_ceiling(t_app *app, int y, const double *cam_x_table,
		const bool light_panel[15][15])
{
	double		r;
	int			x;
	double		c;
	t_ceiling	d;

	r = HEIGHT / (2.0 * (HEIGHT / 2 - app->cam.view_z - y));
	d.y = y;
	x = -1;
	while (++x < WIDTH)
	{
		c = cam_x_table[x];
		d.x = x;
		d.wrd.x = app->cam.pos.x + r * (app->cam.dir.x + app->cam.plane.x * c);
		d.wrd.y = app->cam.pos.y + r * (app->cam.dir.y + app->cam.plane.y * c);
		d.frac.x = d.wrd.x - (int)d.wrd.x;
		d.frac.y = d.wrd.y - (int)d.wrd.y;
		calculate_grid_coordinates(d.wrd, &d.col);
		if (handle_edge_case(app, x, d.y, d.frac))
			continue ;
		if (handle_light_panel(app, light_panel, &d))
			continue ;
		put_pixel_safe(app->image, x, d.y, ft_pixel(app->game.ceiling_color[0],
				app->game.ceiling_color[1], app->game.ceiling_color[2], 255));
	}
}

static void	draw_floor(t_app *app, int y, const double *sx, const double *cy)
{
	const int		h = HEIGHT / 2 - app->cam.view_z;
	const int		offset[2] = {
		((int)(app->cam.pos.x * 0.3) % WIDTH + WIDTH) % WIDTH,
		((int)(app->cam.pos.y * 0.3) % HEIGHT + HEIGHT) % HEIGHT
	};
	const t_vec2	cy_hf = {
		cy[(y + offset[1]) % HEIGHT],
		(double)(y - h) / (HEIGHT - h - 1)
	};
	t_vec2			wave_adj;
	int				x;

	x = -1;
	while (++x < WIDTH)
	{
		wave_adj.x = 0.85 + (sx[(x + offset[0]) % WIDTH] + cy_hf.x) * 0.15;
		wave_adj.y = wave_adj.x * (1.0 - cy_hf.y * 0.35);
		mlx_put_pixel(app->image, x, y, ft_pixel(
				app->game.floor_color[0] * wave_adj.y * (1.0 - cy_hf.y * 0.2),
				app->game.floor_color[1] * wave_adj.y * (1.0 - cy_hf.y * 0.2),
				app->game.floor_color[2] * wave_adj.y * (1.0 - cy_hf.y * 0.2),
				255));
	}
}

void	ft_draw_background(t_app *app)
{
	static double	sx[WIDTH];
	static double	cy[HEIGHT];
	static double	cam[WIDTH];
	static bool		light_panel[15][15];
	int				view_offs_y[4];

	init_background_tables(sx, cy, cam, light_panel);
	view_offs_y[0] = HEIGHT / 2 - app->cam.view_z;
	view_offs_y[1] = ((int)(app->cam.pos.x * 0.3)
			% WIDTH + WIDTH) % WIDTH;
	view_offs_y[2] = ((int)(app->cam.pos.y * 0.3)
			% HEIGHT + HEIGHT) % HEIGHT;
	view_offs_y[3] = 0;
	while (view_offs_y[3] < HEIGHT)
	{
		if (view_offs_y[3] < view_offs_y[0])
			draw_ceiling(app, view_offs_y[3], cam, light_panel);
		else
			draw_floor(app, view_offs_y[3], sx, cy);
		view_offs_y[3]++;
	}
}
