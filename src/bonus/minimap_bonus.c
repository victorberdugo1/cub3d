/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 11:36:39 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 12:45:15 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Draws the player indicator on the minimap.                               */
/*                                                                            */
/*   - Puts a green pixel at the center (MINI_X, MINI_Y).                     */
/*   - Draws a vertical white line below.                                     */
/*   - Draws two diagonal white lines to the sides forming a "∧" shape.       */
/*                                                                            */
/* ************************************************************************** */
static void	draw_player_indicator(t_app *app)
{
	int	i;

	mlx_put_pixel(app->image, MINI_X, MINI_Y, 0x00FF00FF);
	i = -1;
	while (++i < 10)
		mlx_put_pixel(app->image, MINI_X, MINI_Y + i, 0xFFFFFFFF);
	i = -1;
	while (++i < 5)
	{
		mlx_put_pixel(app->image, MINI_X - i, MINI_Y - 3 + i, 0xFFFFFFFF);
		mlx_put_pixel(app->image, MINI_X + i, MINI_Y - 3 + i, 0xFFFFFFFF);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Draws an enemy on the minimap based on its relative position.            */
/*                                                                            */
/*   - Converts relative enemy position into minimap pixel coordinates.       */
/*   - Only draws pixels inside the minimap's circular radius.                */
/*   - Enemies are shown as small red squares.                                */
/*                                                                            */
/* ************************************************************************** */
static void	draw_enemy(t_vec2 pos, t_app *app)
{
	int		dx;
	int		dy;
	t_vec2	pixel;
	t_vec2	center;

	center = (t_vec2){MINI_X, MINI_Y};
	dx = -3;
	while (++dx < 4)
	{
		dy = -3;
		while (++dy < 4)
		{
			pixel.x = MINI_X + (pos.x * MINI_SCALE) + dx;
			pixel.y = MINI_Y + (pos.y * MINI_SCALE) + dy;
			if (hypot(pixel.x - center.x, pixel.y - center.y) <= MINI_RADIUS)
				mlx_put_pixel(app->image, pixel.x, pixel.y, 0xFF0000FF);
		}
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Calculates an enemy's position relative to the player, rotated to        */
/*   match the player's view angle.                                           */
/*                                                                            */
/*   - Subtracts player position to get relative vector.                      */
/*   - Applies a 2D rotation by given angle.                                  */
/*   - Returns rotated coordinates in player view space.                      */
/*                                                                            */
/* ************************************************************************** */
static t_vec2	calc_enemy_pos(t_enemy *e, t_app *app, double angle)
{
	t_vec2	rel_pos;
	t_vec2	rotated;

	rel_pos.x = e->pos_x - app->cam.pos.x;
	rel_pos.y = e->pos_y - app->cam.pos.y;
	rotated.x = rel_pos.x * cos(angle) - rel_pos.y * sin(angle);
	rotated.y = rel_pos.x * sin(angle) + rel_pos.y * cos(angle);
	return (rotated);
}

/* ************************************************************************** */
/*                                                                            */
/*   Draws all active enemies on the minimap.                                 */
/*                                                                            */
/*   - Computes rotation angle based on player direction.                     */
/*   - Calculates each enemy's rotated position and draws it.                 */
/*                                                                            */
/* ************************************************************************** */
void	draw_enemies(t_app *app)
{
	int		i;
	double	angle;
	t_vec2	pos;

	angle = -atan2(app->cam.dir.y, app->cam.dir.x) - M_PI / 2.0;
	i = -1;
	while (++i < app->game.enemy_count)
	{
		if (!app->game.enemies[i].is_active)
			continue ;
		pos = calc_enemy_pos(&app->game.enemies[i], app, angle);
		draw_enemy(pos, app);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Renders the entire minimap, including background, features, enemies,     */
/*   and the player indicator.                                                */
/*                                                                            */
/* ************************************************************************** */
void	render_minimap(t_app *app)
{
	draw_minimap_background(app);
	draw_map_features(app);
	draw_enemies(app);
	draw_player_indicator(app);
}
