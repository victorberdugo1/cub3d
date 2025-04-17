/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:23:15 by victor            #+#    #+#             */
/*   Updated: 2025/04/17 23:59:45 by victor           ###   ########.fr       */
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

    // 1) Cálculo base de wallX
    if (ray->side == 0)
        wallX = app->camera.pos.y + ray->perpwalldist * ray->raydir.y;
    else
        wallX = app->camera.pos.x + ray->perpwalldist * ray->raydir.x;
    wallX -= floor(wallX);

    // 2) Buscamos puerta (si golpeamos '2' o '3')
    if (ray->hit_tile == '2' || ray->hit_tile == '3') {
        for (int i = 0; i < app->game.door_count; i++) {
            t_door *d = &app->game.doors[i];
            if (d->x == ray->map_x && d->y == ray->map_y) {
                door = d;
                break;
            }
        }
    }

    if (door) {
        // ¿Panel o marco?
        bool panel_face = (door->orientation == '2' && ray->side == 1)
                       || (door->orientation == '3' && ray->side == 0);
        if (panel_face) {
            // Aplicar open_offset sólo al panel
            double dir = (door->orientation == '2') ? -1.0 : +1.0;
            wallX = fmod(wallX + dir * door->open_offset + 1.0, 1.0);
            tex   = app->game.tex_door;
        } else {
            // Siempre marco en los lados
            tex = app->game.tex_door_w;
        }
    }
    // 3) Muro normal o pared contigua a una puerta
    if (!tex) {
        if (ray->hit_tile == '1') {
            // Detectamos si es muro junto a puerta para usar marco
            bool use_frame = false;
            if (ray->side == 0) {
                int adjX = ray->map_x - ray->step.x;
                int adjY = ray->map_y;
                char adj = safe_get_tile(&app->game, adjX, adjY);
                if (adj == '2') use_frame = true;
            } else {
                int adjX = ray->map_x;
                int adjY = ray->map_y - ray->step.y;
                char adj = safe_get_tile(&app->game, adjX, adjY);
                if (adj == '3') use_frame = true;
            }
            if (use_frame) {
                tex = app->game.tex_door_w;
            } else {
                // Textura estándar de muros
                if (ray->side == 0)
                    tex = (ray->raydir.x < 0) ? app->game.tex_we : app->game.tex_ea;
                else
                    tex = (ray->raydir.y < 0) ? app->game.tex_no : app->game.tex_so;
            }
        }
    }

    // 4) Asignar y calcular tx
    draw->tex = tex;
    if (!tex) return;          // nada que pintar
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
    if (!draw.tex)
		return;
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
	t_enemy **sorted_enemies = malloc(app->game.enemy_count * sizeof(t_enemy*));
    for (int i = 0; i < app->game.enemy_count; i++)
        sorted_enemies[i] = &app->game.enemies[i];
    
    // Bubble sort simple por distancia
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

    // Renderizar en orden
    for (int i = 0; i < app->game.enemy_count; i++) {
        if (sorted_enemies[i]->is_active)
            render_enemy(app, sorted_enemies[i]);
    }
    
    free(sorted_enemies);

}
