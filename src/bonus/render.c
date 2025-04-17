/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:23:15 by victor            #+#    #+#             */
/*   Updated: 2025/04/17 02:16:12 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

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
static void compute_draw_boundaries(t_draw *draw, t_ray *ray, t_app *app)
{
	draw->lh = (int)(HEIGHT / ray->perpwalldist);
	draw->ds = -draw->lh / 2 + HEIGHT / 2 - app->camera.view_z;
	if (draw->ds < 0)
		draw->ds = 0;
	draw->de = draw->lh / 2 + HEIGHT / 2 - app->camera.view_z;
	if (draw->de >= HEIGHT)
		draw->de = HEIGHT - 1;
}
/*
static void compute_texture_params(t_app *app, t_ray *ray, t_draw *draw)
{
	double wallX;
	mlx_texture_t *tex = NULL;
 
 	if (ray->side == 0)
 		wallX = app->camera.pos.y + (ray->perpwalldist * ray->raydir.y);
	else
 		wallX = app->camera.pos.x + (ray->perpwalldist * ray->raydir.x);
	wallX -= floor(wallX); 
	if (ray->side == 0)
		tex = (ray->raydir.x < 0) ? app->game.tex_we : app->game.tex_ea;
	else
		tex = (ray->raydir.y < 0) ? app->game.tex_no : app->game.tex_so;
	draw->tex = tex;
	draw->tx = (int)(wallX * tex->width);
	if ((ray->side == 0 && ray->raydir.x > 0) || (ray->side == 1 && ray->raydir.y < 0))
		draw->tx = tex->width - draw->tx - 1;
	draw->tx %= tex->width;
	if (draw->tx < 0)
		draw->tx += tex->width;
}*/

static void compute_texture_params(t_app *app, t_ray *ray, t_draw *draw) {
    double wallX;
    mlx_texture_t *tex = NULL;
    t_door *door = NULL;

    if (ray->side == 0)
        wallX = app->camera.pos.y + (ray->perpwalldist * ray->raydir.y);
    else
        wallX = app->camera.pos.x + (ray->perpwalldist * ray->raydir.x);
    wallX -= floor(wallX);

    // Buscar puerta
    if (ray->hit_tile == '2' || ray->hit_tile == '3') {
        for (int i = 0; i < app->game.door_count; i++) {
            if (app->game.doors[i].x == ray->map_x && 
                app->game.doors[i].y == ray->map_y) {
                door = &app->game.doors[i];
                break;
            }
        }
    }

    if (door) {
        if (door->orientation == '2' || door->orientation == '3') { // Puerta NS
            wallX += door->open_offset;
        }

        wallX = fmod(wallX, 1.0);
        if (wallX < 0) wallX += 1.0;

        int is_side = (door->orientation == '2' && ray->side == 0) || 
                     (door->orientation == '3' && ray->side == 1);
        
        tex = is_side ? app->game.tex_door_w : app->game.tex_door;
    } else {
        if (ray->side == 0)
            tex = (ray->raydir.x < 0) ? app->game.tex_we : app->game.tex_ea;
        else
            tex = (ray->raydir.y < 0) ? app->game.tex_no : app->game.tex_so;
    }

    draw->tex = tex;
    if (!tex) return;

    draw->tx = (int)(wallX * tex->width);
    
    if ((ray->side == 0 && ray->raydir.x > 0) || 
        (ray->side == 1 && ray->raydir.y < 0)) {
        draw->tx = tex->width - draw->tx - 1;
    }
    
    draw->tx %= tex->width;
    if (draw->tx < 0) draw->tx += tex->width;
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

	compute_draw_boundaries(&draw, ray, app);
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
