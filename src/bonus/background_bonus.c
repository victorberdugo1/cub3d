/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   background_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 02:02:47 by victor            #+#    #+#             */
/*   Updated: 2025/04/25 11:23:43 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

static inline bool	handle_edge_case(t_app *app, int x, int y, t_vec2 frac)
{
	int		*c;
	int		edge_mask;
	double	r;
	int		dark_c[3];

	edge_mask = (frac.x < 0.005f) | (frac.x > 0.995f) | (frac.y < 0.005f)
		| (frac.y > 0.995f);
	r = HEIGHT / (2.0 * (HEIGHT / 2 - app->cam.view_z - y));
	if (!edge_mask)
		return (false);
	c = app->game.c_color;
	dark_c[0] = (int)(c[0] * 0.8);
	dark_c[1] = (int)(c[1] * 0.8);
	dark_c[2] = (int)(c[2] * 0.8);
	mlx_put_pixel(app->image, x, y, fog(dark_c, r, 0.4, 0.2));
	return (true);
}

static inline bool	handle_light_panel(t_app *a, const bool *pattern,
		t_ceiling *d)
{
	double		r;
	double		max_dist;
	double		i;
	static int	wht[3] = {240, 240, 245};

	if (!pattern[d->col.i * 15 + d->col.j])
		return (false);
	r = HEIGHT / (2.0 * (HEIGHT / 2 - a->cam.view_z - d->y));
	if (fabs(d->col.dx - 0.5) < 0.3338 && fabs(d->col.dy - 0.5) < 0.3338)
		return (mlx_put_pixel(a->image, d->x, d->y, fog(wht, r, 0.08, 0.2)), 1);
	max_dist = fmax(fabs(d->col.dx - 0.5), fabs(d->col.dy - 0.5));
	if (max_dist < 0.4338)
	{
		i = 1.0 - (max_dist - 0.3338) * 10.0;
		if (i > 0.0)
		{
			wht[0] = a->game.c_color[0] + (int)((235 - a->game.c_color[0]) * i);
			wht[1] = a->game.c_color[1] + (int)((235 - a->game.c_color[1]) * i);
			wht[2] = a->game.c_color[2] + (int)((240 - a->game.c_color[2]) * i);
			mlx_put_pixel(a->image, d->x, d->y, fog(wht, r, 0.08, 0.2));
			return (true);
		}
	}
	return (false);
}

static void	draw_ceiling(t_app *app, int y, const double *cam_x_table,
		const bool light_panel[225])
{
	int			x;
	double		r;
	double		c;
	t_ceiling	d;

	r = HEIGHT / (2.0 * (HEIGHT / 2 - app->cam.view_z - y));
	x = -1;
	while (++x < WIDTH)
	{
		d.y = y;
		d.x = x;
		c = cam_x_table[x];
		d.wrd.x = app->cam.pos.x + r * (app->cam.dir.x + app->cam.plane.x * c);
		d.wrd.y = app->cam.pos.y + r * (app->cam.dir.y + app->cam.plane.y * c);
		d.frac.x = d.wrd.x - (int)d.wrd.x;
		d.frac.y = d.wrd.y - (int)d.wrd.y;
		calculate_grid_coordinates(d.wrd, &d.col);
		if (handle_edge_case(app, x, y, d.frac))
			continue ;
		if (handle_light_panel(app, light_panel, &d))
			continue ;
		mlx_put_pixel(app->image, x, y, fog(app->game.c_color,
				r, 0.4, 0.2));
	}
}

static void	draw_floor(t_app *app, int y)
{
	int		x;
	int		h;
	double	r;

	h = HEIGHT / 2 - app->cam.view_z;
	r = HEIGHT / (2.0 * (y - h));
	x = -1;
	while (++x < WIDTH)
	{
		mlx_put_pixel(app->image, x, y, fog(app->game.floor_color,
				r, 0.3, 0.2));
	}
}

void	ft_draw_background(t_app *app)
{
	static double	sx[WIDTH];
	static double	cy[HEIGHT];
	static double	cam[WIDTH];
	static bool		light_panel[225];
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
			draw_floor(app, view_offs_y[3]);
		view_offs_y[3]++;
	}
}
