/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:23:15 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 13:00:58 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Computes the vertical drawing boundaries for a wall slice.               */
/*                                                                            */
/*   - The line height (`lh`) is inversely proportional to the perpendicular  */
/*     wall distance:                                                         */
/*        lh = HEIGHT / perpwalldist                                          */
/*     So, closer walls appear taller, and farther walls shorter.             */
/*                                                                            */
/*   - The starting (`ds`) and ending (`de`) pixel coordinates are then       */
/*     computed to vertically center the wall slice on the screen, adjusted   */
/*     by the camera's vertical offset (`view_z`) to allow looking up/down.   */
/*                                                                            */
/*   - We clamp `ds` and `de` to stay inside the screen bounds.               */
/*                                                                            */
/* ************************************************************************** */
static void	compute_draw_boundaries(t_draw *draw, t_ray *ray, t_app *app)
{
	draw->lh = (int)(HEIGHT / ray->perpwalldist);
	draw->ds = -draw->lh / 2 + HEIGHT / 2 - app->cam.view_z;
	if (draw->ds < 0)
		draw->ds = 0;
	draw->de = draw->lh / 2 + HEIGHT / 2 - app->cam.view_z;
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
/*   Sorts enemies in descending order of distance from the player.           */
/*                                                                            */
/*   - Uses a simple bubble sort (acceptable since enemy count is small).     */
/*                                                                            */
/*   - Distance is calculated with the Euclidean distance formula:            */
/*        distance = sqrt((x2 - x1)^2 + (y2 - y1)^2)                          */
/*      using `hypot()`.                                                      */
/*                                                                            */
/*   - This ensures enemies farther away are drawn first, and closer ones     */
/*     later, preventing visual glitches (painters algorithm).                */
/*                                                                            */
/* ************************************************************************** */
static void	sort_enemies_by_distance(t_enemy **enemies, int count, t_app *a)
{
	int		i;
	int		j;
	t_enemy	*temp;

	i = -1;
	while (++i < count - 1)
	{
		j = -1;
		while (++j < count - i - 1)
		{
			if (hypot(enemies[j]->pos_x - a->cam.pos.x,
					enemies[j]->pos_y - a->cam.pos.y)
				< hypot(enemies[j + 1]->pos_x - a->cam.pos.x,
					enemies[j + 1]->pos_y - a->cam.pos.y))
			{
				temp = enemies[j];
				enemies[j] = enemies[j + 1];
				enemies[j + 1] = temp;
			}
		}
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Sorts enemies by distance and renders them in correct order.             */
/*                                                                            */
/*   - Allocates a temporary array to sort pointers to enemies without        */
/*     modifying the original enemy array.                                    */
/*                                                                            */
/*   - Only active enemies (`is_active == 1`) are rendered.                   */
/*                                                                            */
/*   - After rendering, the temporary array is freed.                         */
/*                                                                            */
/* ************************************************************************** */
static void	sort_and_render(t_app *a)
{
	t_enemy	**sorted;
	int		i;

	sorted = malloc(a->game.enemy_count * sizeof(t_enemy *));
	if (!sorted)
		return ;
	i = -1;
	while (++i < a->game.enemy_count)
		sorted[i] = &a->game.enemies[i];
	sort_enemies_by_distance(sorted, a->game.enemy_count, a);
	i = -1;
	while (++i < a->game.enemy_count)
		if (sorted[i]->is_active)
			render_enemy(a, sorted[i]);
	free(sorted);
}

/* ************************************************************************** */
/*                                                                            */
/*   Renders the full scene for the current frame.                            */
/*                                                                            */
/*   - Steps:                                                                 */
/*      1. Draws the background (sky and floor).                              */
/*      2. For each column (screen X-coordinate):                             */
/*          - Initializes a ray for that column (`init_ray`).                 */
/*          - Performs DDA to detect wall hit (`do_dda`).                     */
/*          - Renders the vertical slice of wall (`render_column`).           */
/*      3. Draws the minimap on top.                                          */
/*      4. Renders enemies (sorted by distance).                              */
/*      5. Renders player's weapon and damage feedback effects.               */
/*                                                                            */
/* ************************************************************************** */
void	render_scene(void *param)
{
	t_app	*a;
	int		x;
	t_ray	r;

	a = param;
	ft_draw_background(a);
	x = -1;
	while (++x < WIDTH)
	{
		init_ray(a, x, &r);
		do_dda(a, &r);
		render_column(a, x, &r);
	}
	render_minimap(a);
	sort_and_render(a);
	render_weapon(a);
	render_hit_feedback(a);
}
