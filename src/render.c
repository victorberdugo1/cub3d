/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:23:15 by victor            #+#    #+#             */
/*   Updated: 2025/03/27 13:26:09 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

static int32_t	ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a)
{
	return (r << 24 | g << 16 | b << 8 | a);
}

static void	ft_draw_background(t_app *app)
{
	int32_t		floor_color;
	int32_t		ceiling_color;
	uint32_t	x;
	uint32_t	y;

	floor_color = ft_pixel(app->game.floor_color[0],
			app->game.floor_color[1], app->game.floor_color[2], 255);
	ceiling_color = ft_pixel(app->game.ceiling_color[0],
			app->game.ceiling_color[1], app->game.ceiling_color[2], 255);
	x = -1;
	while (++x < WIDTH)
	{
		y = -1;
		while (++y < HEIGHT / 2)
			mlx_put_pixel(app->image, x, y, ceiling_color);
		y = HEIGHT / 2;
		while (++y < HEIGHT)
			mlx_put_pixel(app->image, x, y, floor_color);
	}
}

void	render_scene(void *param)
{
	t_app	*app;

	app = (t_app *)param;
	ft_draw_background(app);
	for (int x = 0; x < WIDTH; x++)
	{
		double cameraX = 2 * x / (double)WIDTH - 1;
		t_vec2 rayDir = {app->camera.dir.x + app->camera.plane.x * cameraX,
			app->camera.dir.y + app->camera.plane.y * cameraX};
		int map_x = (int)app->camera.pos.x;
		int map_y = (int)app->camera.pos.y;
		t_vec2 deltaDist = {fabs(1 / rayDir.x), fabs(1 / rayDir.y)};
		t_vec2 sideDist;
		t_vec2 step;
		int hit = 0, side;
		if (rayDir.x < 0)
		{
			step.x = -1;
			sideDist.x = (app->camera.pos.x - map_x) * deltaDist.x;
		}
		else
		{
			step.x = 1;
			sideDist.x = (map_x + 1.0 - app->camera.pos.x) * deltaDist.x;
		}
		if (rayDir.y < 0)
		{
			step.y = -1;
			sideDist.y = (app->camera.pos.y - map_y) * deltaDist.y;
		}
		else
		{
			step.y = 1;
			sideDist.y = (map_y + 1.0 - app->camera.pos.y) * deltaDist.y;
		}
		while (!hit)
		{
			if (sideDist.x < sideDist.y)
			{
				sideDist.x += deltaDist.x;
				map_x += step.x;
				side = 0;
			}
			else
			{
				sideDist.y += deltaDist.y;
				map_y += step.y;
				side = 1;
			}
			if (map_y < 0 || map_y >= app->game.map_height)
			{
				hit = 1;
				break;
			}
			size_t line_length = strlen(app->game.map[map_y]);
			if (map_x < 0 || map_x >= (int)line_length)
			{
				hit = 1;
				break;
			}
			if (safe_get_tile(&app->game, map_x, map_y) == '1')
				hit = 1;
		}
		double perpWallDist = (side == 0) ? (sideDist.x - deltaDist.x)
			: (sideDist.y - deltaDist.y);
		int lineHeight = (int)(HEIGHT / perpWallDist);
		int drawStart = -lineHeight / 2 + HEIGHT / 2;
		if (drawStart < 0)
			drawStart = 0;
		int drawEnd = lineHeight / 2 + HEIGHT / 2;
		if (drawEnd >= HEIGHT)
			drawEnd = HEIGHT - 1;
		double wallX;
		if (side == 0)
			wallX = app->camera.pos.y + perpWallDist * rayDir.y;
		else
			wallX = app->camera.pos.x + perpWallDist * rayDir.x;
		wallX -= floor(wallX);
		mlx_texture_t *tex;
		if (side == 0)
			tex = (rayDir.x < 0) ? app->game.tex_we : app->game.tex_ea;
		else
			tex = (rayDir.y < 0) ? app->game.tex_no : app->game.tex_so;
		if (!tex)
		{
			fprintf(stderr, "Error: Textura no disponible.\n");
			continue;
		}
		int texWidth = tex->width;
		int texHeight = tex->height;
		int texX = (int)(wallX * texWidth);
		if (side == 0 && rayDir.x > 0)
			texX = texWidth - texX - 1;
		if (side == 1 && rayDir.y < 0)
			texX = texWidth - texX - 1;
		for (int y = drawStart; y < drawEnd; y++)
		{
			int d = y * 256 - HEIGHT * 128 + lineHeight * 128;
			int texY = ((d * texHeight) / lineHeight) / 256;
			uint32_t color = ((uint32_t *)tex->pixels)[texY * texWidth + texX];
			mlx_put_pixel(app->image, x, y, color);
		}
	}
}
