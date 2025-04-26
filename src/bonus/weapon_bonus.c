/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   weapon_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 21:47:45 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 13:17:08 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Initializes weapon-related variables if not already initialized.         */
/*                                                                            */
/*   - Sets the total number of idle and attack frames for the weapon.        */
/*   - Initializes alternate animation flag to false.                         */
/*   - Marks the weapon as initialized to prevent reinitialization.           */
/*                                                                            */
/* ************************************************************************** */
void	weapon_initialize(t_weapon *w, bool *inited)
{
	if (!*inited)
	{
		w->total_idle_frames = 3;
		w->total_attack_frames = 5;
		w->alt_animation = false;
		*inited = true;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Initializes drawing data for the weapon texture rendering.               */
/*                                                                            */
/*   - Computes the rendered width and height based on the weapon's texture.  */
/*   - Calculates the offsets for centering the weapon on the screen.         */
/*   - Calculates texture sub-region dimensions and starting coordinates for  */
/*     animation frames.                                                      */
/*                                                                            */
/* ************************************************************************** */
static void	init_draw_data(t_app *app, t_draw_data *d)
{
	t_weapon	*w;
	int			rendered_h;
	int			rendered_w;

	w = &app->weapon;
	rendered_h = HEIGHT * SCALE;
	rendered_w = (w->texture->width * rendered_h)
		/ w->texture->height;
	d->sprite_height = rendered_h;
	d->sprite_width = rendered_w;
	d->offset_x = WIDTH / 2 - rendered_w / 2;
	d->offset_y = HEIGHT - rendered_h + Y_OFFSET;
	d->tex_width = w->texture->width;
	d->tex_height = w->texture->height;
	d->width = w->texture->width / COLS;
	d->hgt = w->texture->height / ROWS;
	d->st_x = (w->current_frame % COLS) * d->width;
	d->st_y = (w->current_frame / COLS) * d->hgt;
}

/* ************************************************************************** */
/*                                                                            */
/*   Draws a single pixel of the weapon texture onto the screen.              */
/*                                                                            */
/*   - Uses the pixel data from the weapon texture to select the color.       */
/*   - Converts the texture's pixel color to the screen's color format.       */
/*   - Only draws non-transparent pixels (alpha value is non-zero).           */
/*   - Places the pixel at the correct position on the screen based on the    */
/*     calculated offsets.                                                    */
/*                                                                            */
/* ************************************************************************** */
static void	draw_weapon_pixel(t_app *app, t_draw_data *d,
		int x, int y)
{
	t_weapon	*w;
	uint32_t	*pixels;
	uint32_t	color;
	int			screen_xy[2];
	int			tex_xy[2];

	w = &app->weapon;
	pixels = (uint32_t *)w->texture->pixels;
	screen_xy[0] = d->offset_x + x;
	screen_xy[1] = d->offset_y + y;
	tex_xy[0] = (x * d->width) / d->sprite_width + d->st_x;
	tex_xy[1] = (y * d->hgt) / d->sprite_height + d->st_y;
	color = pixels[tex_xy[1] * d->tex_width + tex_xy[0]];
	if ((color & 0xFF000000) != 0)
		mlx_put_pixel(app->image,
			screen_xy[0], screen_xy[1],
			convert_pixel(color));
}

/* ************************************************************************** */
/*                                                                            */
/*   Loops over all pixels in the weapon sprite and draws them on the screen. */
/*                                                                            */
/*   - Loops through each pixel's coordinates in both X and Y directions.     */
/*   - Calls `draw_pixel()` to draw each valid pixel in the screen area.      */
/*   - Ensures the weapon is drawn within the screen boundaries (both X and   */
/*     Y offsets are respected).                                              */
/*                                                                            */
/* ************************************************************************** */
static void	render_weapon_loop(t_app *app, t_draw_data *d)
{
	int	x;
	int	y;

	y = 0;
	while (y < d->sprite_height)
	{
		if (d->offset_y + y >= 0 && d->offset_y + y < HEIGHT)
		{
			x = 0;
			while (x < d->sprite_width)
			{
				if (d->offset_x + x >= 0 && d->offset_x + x < WIDTH)
					draw_weapon_pixel(app, d, x, y);
				x++;
			}
		}
		y++;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Renders the weapon preparing drawing data and invoking the render loop.  */
/*                                                                            */
/*   - Initializes the drawing data for the weapon.                           */
/*   - Calls `render_loop()` to perform the pixel-by-pixel drawing.           */
/*   - Ensures the weapon texture is valid before rendering.                  */
/*                                                                            */
/* ************************************************************************** */
void	render_weapon(t_app *app)
{
	t_weapon	*w;
	t_draw_data	d;

	w = &app->weapon;
	if (!w->texture)
		return ;
	init_draw_data(app, &d);
	render_weapon_loop(app, &d);
}
