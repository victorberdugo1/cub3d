/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   background_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 02:02:47 by victor            #+#    #+#             */
/*   Updated: 2025/04/23 19:49:32 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

static inline bool	handle_edge_case(t_app *app, int x, int y, t_vec2 frac)
{
	int			*c;
	const int	edge_mask = (frac.x < 0.005f) | (frac.x > 0.995f)
		| (frac.y < 0.005f) | (frac.y > 0.995f);

	if (edge_mask)
	{
		c = app->game.ceiling_color;
		mlx_put_pixel(app->image, x, y, ft_pixel(c[0] * 0.8,
				c[1] * 0.8, c[2] * 0.8, 255));
		return (true);
	}
	return (false);
}

static inline bool	handle_light_panel(t_app *app,
		const bool *pattern, t_ceiling *d)
{
	int		*c;
	double	max_dist;
	double	i;

	max_dist = fmax(fabs(d->col.dx - 0.5), fabs(d->col.dy - 0.5));
	if (!pattern[d->col.i * 15 + d->col.j])
		return (false);
	if (fabs(d->col.dx - 0.5) < 0.3338 && fabs(d->col.dy - 0.5) < 0.3338)
		return (mlx_put_pixel(app->image, d->x, d->y,
				ft_pixel(240, 240, 245, 255)), true);
	if (max_dist < 0.4338)
	{
		i = 1.0 - (max_dist - 0.3338) * 10.0;
		if (i > 0.0)
		{
			c = app->game.ceiling_color;
			return (mlx_put_pixel(app->image, d->x, d->y, ft_pixel(c[0]
						+ (int)((235 - c[0]) * i), c[1] + (int)((235 - c[1]
					) * i), c[2] + (int)((240 - c[2]) * i), 255)), true);
		}
	}
	return (false);
}

static void	draw_ceiling(t_app *app, int y, const double *cam_x_table,
		const bool light_panel[225])
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
		mlx_put_pixel(app->image, x, d.y, ft_pixel(app->game.ceiling_color[0],
				app->game.ceiling_color[1], app->game.ceiling_color[2], 255));
	}
}

static void	draw_floor(t_app *app, int y)
{
	const int		h = HEIGHT / 2 - app->cam.view_z;
	const double	vertical_factor = (double)(y - h) / (HEIGHT - h - 1);
	const double	gradient = 1.0 - vertical_factor * 0.4;
	uint32_t		color;
	int				x;

	color = ft_pixel(
			app->game.floor_color[0] * gradient,
			app->game.floor_color[1] * gradient,
			app->game.floor_color[2] * gradient,
			255
			);
	x = -1;
	while (++x < WIDTH)
		mlx_put_pixel(app->image, x, y, color);
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
