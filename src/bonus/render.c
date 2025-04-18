/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:23:15 by victor            #+#    #+#             */
/*   Updated: 2025/04/18 19:36:49 by victor           ###   ########.fr       */
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
static void	compute_draw_boundaries(t_draw *draw, t_ray *ray, t_app *app)
{
	draw->lh = (int)(HEIGHT / ray->perpwalldist);
	draw->ds = -draw->lh / 2 + HEIGHT / 2 - app->camera.view_z;
	if (draw->ds < 0)
		draw->ds = 0;
	draw->de = draw->lh / 2 + HEIGHT / 2 - app->camera.view_z;
	if (draw->de >= HEIGHT)
		draw->de = HEIGHT - 1;
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
	if (!draw.tex)
		return ;
	app->z_buffer[x] = ray->perpwalldist;
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
	render_minimap(app);
	t_enemy **sorted_enemies = malloc(app->game.enemy_count * sizeof(t_enemy*));
	for (int i = 0; i < app->game.enemy_count; i++)
		sorted_enemies[i] = &app->game.enemies[i];

	for (int i = 0; i < app->game.enemy_count - 1; i++) {
		for (int j = 0; j < app->game.enemy_count - i - 1; j++) {
			double dist1 = hypot(sorted_enemies[j]->pos_x - app->camera.pos.x,
					sorted_enemies[j]->pos_y - app->camera.pos.y);
			double dist2 = hypot(sorted_enemies[j+1]->pos_x - app->camera.pos.x,
					sorted_enemies[j+1]->pos_y - app->camera.pos.y);
			if (dist1 < dist2) {
				t_enemy *temp = sorted_enemies[j];
				sorted_enemies[j] = sorted_enemies[j+1];
				sorted_enemies[j+1] = temp;
			}
		}
	}
	for (int i = 0; i < app->game.enemy_count; i++) {
		if (sorted_enemies[i]->is_active)
			render_enemy(app, sorted_enemies[i]);
	}
	free(sorted_enemies);

}
