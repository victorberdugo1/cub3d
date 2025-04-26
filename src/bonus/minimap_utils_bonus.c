/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap_utils_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 18:06:16 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 13:29:43 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Draws the circular background of the minimap.                            */
/*                                                                            */
/*   - Fills a circle centered at (MINI_X, MINI_Y) with a dark gray color.    */
/*   - Only pixels inside the radius are colored.                             */
/*                                                                            */
/* ************************************************************************** */
void	draw_minimap_background(t_app *app)
{
	int	i;
	int	j;

	i = -MINI_RADIUS - 1;
	while (++i <= MINI_RADIUS)
	{
		j = -MINI_RADIUS - 1;
		while (++j <= MINI_RADIUS)
			if (i * i + j * j <= MINI_RADIUS * MINI_RADIUS)
				mlx_put_pixel(app->image, MINI_X + j, MINI_Y + i, 0x202020FF);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Determines the color to use for a given tile at (x, y) on the map.       */
/*                                                                            */
/*   - Walls ('1') are gray.                                                  */
/*   - Doors ('2' or '3') are orange if closed and green if open.             */
/*   - Other tiles are transparent (color 0).                                 */
/*                                                                            */
/* ************************************************************************** */
static uint32_t	get_tile_color(t_app *app, int x, int y)
{
	char	tile;
	int		d;

	tile = safe_get_tile(&app->game, x, y);
	if (tile == '1')
		return (0x808080FF);
	if (tile == '2' || tile == '3')
	{
		d = -1;
		while (++d < app->game.door_count)
		{
			if (app->game.doors[d].x == x && app->game.doors[d].y == y)
			{
				if (app->game.doors[d].is_open)
					return (0x00FF00FF);
				else
					return (0xFF8000FF);
			}
		}
		return (0xFF8000FF);
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Draws a tile on the minimap with rotation based on the player's view.    */
/*                                                                            */
/*   - Draws a small square centered at (cx, cy), rotated by the view angle.  */
/*   - Only draws pixels that are inside the minimap's circle.                */
/*                                                                            */
/* ************************************************************************** */
static void	draw_rotated_pixels(t_app *app, int cx, int cy, uint32_t color)
{
	int		dx;
	int		dy;
	double	rot_x;
	double	rot_y;
	double	angle;

	angle = -atan2(app->cam.dir.y, app->cam.dir.x) - M_PI / 2.0;
	dx = -3;
	while (++dx < 6)
	{
		dy = -3;
		while (++dy < 6)
		{
			rot_x = dx * cos(angle) - dy * sin(angle);
			rot_y = dx * sin(angle) + dy * cos(angle);
			if (hypot(cx + rot_x - MINI_X, cy + rot_y - MINI_Y) <= MINI_RADIUS)
				mlx_put_pixel(app->image, cx + rot_x, cy + rot_y, color);
		}
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Draws all visible map features (walls, doors) on the minimap.            */
/*                                                                            */
/*   - Iterates through the map tiles within view distance.                   */
/*   - Calculates their rotated and scaled position on the minimap.           */
/*   - Draws them with the appropriate color.                                 */
/*                                                                            */
/* ************************************************************************** */
void	draw_map_features(t_app *app)
{
	int			x;
	int			y;
	uint32_t	color;
	double		angle;

	angle = -atan2(app->cam.dir.y, app->cam.dir.x) - M_PI / 2.0;
	x = -MINI_VIEW_DIST - 1;
	while (++x <= MINI_VIEW_DIST)
	{
		y = -MINI_VIEW_DIST - 1;
		while (++y <= MINI_VIEW_DIST)
		{
			color = get_tile_color(app, (int)app->cam.pos.x + x,
					(int)app->cam.pos.y + y);
			if (color)
				draw_rotated_pixels(app,
					MINI_X + (x * cos(angle) - y * sin(angle)) * MINI_SCALE,
					MINI_Y + (x * sin(angle) + y * cos(angle)) * MINI_SCALE,
					color);
		}
	}
}
