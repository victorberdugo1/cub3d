/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_utils_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 13:20:35 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 13:10:12 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Encodes four 8-bit color components (r, g, b, a) into a single 32-bit    */
/*   integer in RGBA order.                                                   */
/*                                                                            */
/*   - Each component is shifted into its position:                           */
/*       - r << 24, g << 16, b << 8, a.                                       */
/*                                                                            */
/* ************************************************************************** */
int32_t	ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a)
{
	return (r << 24 | g << 16 | b << 8 | a);
}

/* ************************************************************************** */
/*                                                                            */
/*   Converts a pixel from standard ABGR format (as given by MLX42) to        */
/*   RGBA format as expected by the renderer.                                 */
/*                                                                            */
/*   Steps:                                                                   */
/*    1. Extract red, green, blue, alpha channels from original pixel.        */
/*    2. Rearrange channels:                                                  */
/*         - Blue -> Red, Green -> Green, Red -> Blue.                        */
/*         - Alpha stays in place.                                            */
/*                                                                            */
/* ************************************************************************** */
uint32_t	convert_pixel(uint32_t px)
{
	uint8_t	red;
	uint8_t	green;
	uint8_t	blue;
	uint8_t	alpha;

	red = (px >> 16) & 0xFF;
	green = (px >> 8) & 0xFF;
	blue = px & 0xFF;
	alpha = (px >> 24) & 0xFF;
	return (ft_pixel(blue, green, red, alpha));
}

/* ************************************************************************** */
/*                                                                            */
/*   Initializes background distortion tables and randomizes a light panel.   */
/*                                                                            */
/*   Steps:                                                                   */
/*    1. Fills `sx` with horizontal sine distortions.                         */
/*       (small horizontal wavy movement based on sine function)              */
/*    2. Fills `cam_x_table` to store the camera X coordinate for each column */
/*    3. Fills `cy` with vertical cosine distortions (wave effect in Y).      */
/*    4. Randomizes `light_panel` to simulate random flickering lights.       */
/*                                                                            */
/*   Math:                                                                    */
/*     sx[x] = sin(x * 0.4) * 0.08                                            */
/*     cam_x_table[x] = 2.0 * (x + 0.5) / WIDTH - 1.0                         */
/*     cy[y] = cos(y * 0.6) * 0.08                                            */
/*                                                                            */
/* ************************************************************************** */
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

/* ************************************************************************** */
/*                                                                            */
/*   Calculates grid-relative coordinates and indices for collision.          */
/*                                                                            */
/*   - Computes the fractional part of the world position (`dx`, `dy`).       */
/*   - Computes which light panel tile (`i`, `j`) the player is currently on. */
/*   - Ensures positive indices even when modulo results are negative.        */
/*                                                                            */
/*   Useful for grid-based effects (like lighting panels, shadows, etc.).     */
/*                                                                            */
/* ************************************************************************** */
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
/*   Draws a vertical column of wall pixels with fog effect.                  */
/*                                                                            */
/*   Steps:                                                                   */
/*    1. For each screen pixel from `draw->ds` to `draw->de`:                 */
/*         - Calculate distance-based texture Y coordinate `ty`.              */
/*         - Fetch the corresponding pixel color from texture.                */
/*         - Apply a fog effect based on distance stored in `z_buffer[x]`.    */
/*         - Draw the pixel to the screen with `mlx_put_pixel()`.             */
/*                                                                            */
/*   Math for ty:                                                             */
/*     d = (y + view_z) * 256 - HEIGHT * 128 + lh * 128                       */
/*     ty = (d * texture_height / lh) / 256                                   */
/*                                                                            */
/*   Fog factor:                                                              */
/*     fog_factor = max(1.0 / (1.0 + 0.25 * distance), 0.4)                   */
/*     (minimum fog = 0.4, closer = clearer, farther = foggier)               */
/*                                                                            */
/* ************************************************************************** */
void	draw_pixels(t_app *app, int x, t_draw *draw)
{
	int			y;
	int			d;
	int			ty;
	uint32_t	px;
	double		fog_factor;

	y = draw->ds;
	while (y < draw->de)
	{
		d = (y + app->cam.view_z) * 256 - HEIGHT * 128 + draw->lh * 128;
		ty = (((d * draw->tex->height) / draw->lh) / 256) % draw->tex->height;
		if (ty < 0)
			ty += draw->tex->height;
		px = ((uint32_t *)draw->tex->pixels)[ty * draw->tex->width + draw->tx];
		fog_factor = fmax(1.0 / (1.0 + 0.25 * app->z_buffer[x]), 0.4);
		mlx_put_pixel(app->image, x, y++, ft_pixel((px & 0xFF) * fog_factor,
				((px >> 8) & 0xFF) * fog_factor, ((px >> 16) & 0xFF)
				* fog_factor, convert_pixel(px) & 0xFF));
	}
}
