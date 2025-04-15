/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:23:15 by victor            #+#    #+#             */
/*   Updated: 2025/04/03 12:49:14 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

/* ************************************************************************** */
/*                                                                            */
/*   Retrieves the X coordinate of the wall intersection (texture position).  */
/*                                                                            */
/*   - Uses the perpendicular wall distance (`perpwalldist`) and the ray's   */
/*     direction to compute the intersection's position on the wall.          */
/*   - If the ray is hitting a vertical wall (side == 0), it calculates the   */
/*     intersection along the y-axis, otherwise along the x-axis.            */
/*   - The function returns a value between 0 and 1 that represents the      */
/*     position along the wall's texture for rendering.                       */
/*                                                                            */
/* ************************************************************************** */
static double	get_wall_x(t_app *app, t_ray *ray)
{
	double	wx;
	double	mod;

	mod = ray->raydir_mod;
	if (ray->side == 0)
		wx = app->camera.pos.y + (ray->perpwalldist * ray->raydir.y)/mod;
	else
		wx = app->camera.pos.x + (ray->perpwalldist * ray->raydir.x)/mod;
	return (wx - floor(wx));
}

/* ************************************************************************** */
/*                                                                            */
/*   Selects the correct texture based on the ray's side and direction.       */
/*                                                                            */
/*   - Checks whether the ray is hitting a vertical or horizontal wall.      */
/*   - Depending on the direction of the ray, it assigns the appropriate     */
/*     texture (North, South, East, West).                                   */
/*   - Calculates the texture's horizontal coordinate (`tx`) based on the   */
/*     `wx` value (wall intersection coordinate).                            */
/*   - If the ray is hitting a wall from the opposite direction, the texture */
/*     coordinate is mirrored (reversed).                                    */
/*                                                                            */
/* ************************************************************************** */
static void	select_texture(t_app *app, t_ray *ray, t_draw *draw, double wx)
{
	if (ray->side == 0)
	{
		if (ray->raydir.x < 0)
			draw->tex = app->game.tex_we;
		else
			draw->tex = app->game.tex_ea;
	}
	else
	{
		if (ray->raydir.y < 0)
			draw->tex = app->game.tex_no;
		else
			draw->tex = app->game.tex_so;
	}
	if (!draw->tex)
		return ;
	draw->tx = (int)(wx * draw->tex->width);
	if (ray->side == 0 && ray->raydir.x > 0)
		draw->tx = draw->tex->width - draw->tx - 1;
	else if (ray->side == 1 && ray->raydir.y < 0)
		draw->tx = draw->tex->width - draw->tx - 1;
}

/* ************************************************************************** */
/*                                                                            */
/*   Computes the necessary parameters for drawing the texture.              */
/*                                                                            */
/*   - First, it calculates the wall's intersection X-coordinate using the  */
/*     `get_wall_x()` function.                                              */
/*   - Then, it selects the correct texture based on the ray's side and      */
/*     direction using `select_texture()`.                                    */
/*                                                                            */
/* ************************************************************************** */
static void	compute_texture_params(t_app *app, t_ray *ray, t_draw *draw)
{
	double	wx;

	wx = get_wall_x(app, ray);
	select_texture(app, ray, draw, wx);
}

/* ************************************************************************** */
/*                                                                            */
/*   Renders a column of pixels for the given ray at the specified X.        */
/*                                                                            */
/*   - First, it computes the boundaries of the vertical line (column) to    */
/*     be drawn using `compute_draw_boundaries()`.                            */
/*   - It then calculates the texture parameters and applies them to the     */
/*     drawing process using `compute_texture_params()`.                     */
/*   - Finally, it draws the pixels for the column by calling `draw_pixels()`.*/
/*                                                                            */
/* ************************************************************************** */
static void	render_column(t_app *app, int x, t_ray *ray)
{
	t_draw	draw;

	compute_draw_boundaries(&draw, ray);
	compute_texture_params(app, ray, &draw);
	draw_pixels(app, x, &draw);
}

/* ************************************************************************** */
/*                                                                            */
/*   Renders the entire scene.                                               */
/*                                                                            */
/*   - Loops over every column (X-coordinate) in the screen.                */
/*   - For each column, it initializes the ray, performs the DDA (raycasting),*/
/*     and renders the corresponding column.                                  */
/*   - The function calls `render_column()` to draw each vertical line.      */
/*                                                                            */
/* ************************************************************************** */
void	render_scene(void *param)
{
	t_app	*app;
	int		x;
	t_ray	ray;

	app = (t_app *)param;
	ft_draw_background(app);
	x = 0;
	while (x < WIDTH)
	{
		init_ray(app, x, &ray);
		do_dda(app, &ray);
		render_column(app, x, &ray);
		x++;
	}
}
