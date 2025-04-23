/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_utils_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 13:20:35 by victor            #+#    #+#             */
/*   Updated: 2025/04/22 22:46:13 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

int32_t	ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a)
{
	return (r << 24 | g << 16 | b << 8 | a);
}

void	init_background_tables(double *sx, double *cy, double *cam_x_table,
		bool light_panel[225])
{
	static bool	init = false;
	int			x;
	int			y;

	if (init)
		return ;
	srand((unsigned int)(mlx_get_time() * 100.0));
	x = -1;
	while (++x < WIDTH)
	{
		sx[x] = sin(x * 0.4) * 0.08;
		cam_x_table[x] = 2.0 * (x + 0.5) / WIDTH - 1.0;
	}
	y = -1;
	while (++y < HEIGHT)
		cy[y] = cos(y * 0.6) * 0.08;
	x = -1;
	while (++x < 15)
	{
		y = -1;
		while (++y < 15)
			light_panel[x * 15 + y] = (rand() % 15) == 0;
	}
	init = true;
}

void	calculate_grid_coordinates(t_vec2 world, t_collision *col)
{
	col->dx = world.x - (int)world.x;
	col->dy = world.y - (int)world.y;
	col->i = (int)world.x % 15;
	if (col->i < 0)
		col->i += 15;
	col->j = (int)world.y % 15;
	if (col->j < 0)
		col->j += 15;
}

/* ************************************************************************** */
/*                                                                            */
/*   Draws a vertical strip of pixels for a wall texture on the screen.       */
/*                                                                            */
/*   - Iterates from `draw->ds` (draw start) to `draw->de` (draw end).        */
/*   - Calculates the texture y-coordinate (`ty`) for each screen pixel.      */
/*   - Retrieves the corresponding texture pixel and plots it with            */
/*     `mlx_put_pixel()`.                                                     */
/*                                                                            */
/* ************************************************************************** */
void	draw_pixels(t_app *app, int x, t_draw *draw)
{
	int			y;
	int			d;
	int			ty;
	uint32_t	px;

	y = draw->ds;
	while (y < draw->de)
	{
		d = (y + app->cam.view_z) * 256 - HEIGHT * 128 + draw->lh * 128;
		ty = ((d * draw->tex->height) / draw->lh) / 256;
		ty = ty % draw->tex->height;
		if (ty < 0)
			ty += draw->tex->height;
		px = ((uint32_t *)draw->tex->pixels)[ty * draw->tex->width + draw->tx];
		px = convert_pixel(px);
		mlx_put_pixel(app->image, x, y, px);
		y++;
	}
}
