/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_txt_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 19:15:16 by victor            #+#    #+#             */
/*   Updated: 2025/04/26 13:05:59 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Finds the door at a given map position (x, y), if it exists.             */
/*                                                                            */
/*   - Iterates over all doors stored in the game.                            */
/*   - Returns a pointer to the matching door if found, NULL otherwise.       */
/*                                                                            */
/* ************************************************************************** */
static t_door	*get_door(t_game *g, int x, int y)
{
	int	i;

	i = 0;
	while (i < g->door_count)
	{
		if (g->doors[i].x == x && g->doors[i].y == y)
			return (&g->doors[i]);
		i++;
	}
	return (NULL);
}

/* ************************************************************************** */
/*                                                                            */
/*   Selects the appropriate wall texture based on ray direction and side.    */
/*                                                                            */
/*   - Checks if the adjacent tile is a door ('2' or '3') depending on side.  */
/*     If so, returns the door wall texture (`tex_door_w`).                   */
/*                                                                            */
/*   - Otherwise, selects the texture based on the ray direction:             */
/*       - North wall (`tex_no`), South wall (`tex_so`),                      */
/*         West wall (`tex_we`), East wall (`tex_ea`).                        */
/*                                                                            */
/* ************************************************************************** */
static mlx_texture_t	*get_wall_texture(t_app *app, t_ray *ray)
{
	char	adj;

	if (ray->side == 0)
		adj = safe_get_tile(&app->game, ray->map_x - ray->step.x, ray->map_y);
	else
		adj = safe_get_tile(&app->game, ray->map_x, ray->map_y - ray->step.y);
	if ((ray->side == 0 && adj == '2') || (ray->side == 1 && adj == '3'))
		return (app->game.tex_door_w);
	if (ray->side == 0)
	{
		if (ray->raydir.x < 0)
			return (app->game.tex_we);
		else
			return (app->game.tex_ea);
	}
	else
	{
		if (ray->raydir.y < 0)
			return (app->game.tex_no);
		else
			return (app->game.tex_so);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Selects the correct door texture and adjusts texture coordinate `w`.     */
/*                                                                            */
/*   - If the ray hits the door panel (front face), the texture coordinate `w`*/
/*     is shifted according to the door's opening (`open_offset`).            */
/*                                                                            */
/*   - For side faces (non-panel), a static door wall texture is used.        */
/*                                                                            */
/*   Math:                                                                    */
/*     - When a door opens, the visible part of the door panel shifts.        */
/*     - `fmod` is used to keep the value inside [0.0, 1.0) after shifting.   */
/*                                                                            */
/* ************************************************************************** */
static mlx_texture_t	*get_door_texture(t_game *game, t_door *d,
		t_ray *ray, double *w)
{
	bool	panel_face;

	panel_face = (d->orientation == '2' && ray->side == 1)
		|| (d->orientation == '3' && ray->side == 0);
	if (panel_face)
	{
		if (d->orientation == '2')
			*w = fmod(*w - d->open_offset + 1.0, 1.0);
		else
			*w = fmod(*w + d->open_offset + 1.0, 1.0);
		return (game->tex_door);
	}
	return (game->tex_door_w);
}

/* ************************************************************************** */
/*                                                                            */
/*   Computes the horizontal texture X coordinate (`tx`) for a ray hit.       */
/*                                                                            */
/*   - Maps the relative wall hit position `w` to a pixel coordinate on the   */
/*     texture:                                                               */
/*         tx = (int)(w * texture_width)                                      */
/*                                                                            */
/*   - Adjusts for ray direction to correctly mirror textures when needed:    */
/*     - When hitting from certain sides, the texture must be flipped.        */
/*                                                                            */
/*   Math:                                                                    */
/*     - If ray hits the "back" side of the wall, mirror by:                  */
/*         tx = texture_width - tx - 1                                        */
/*                                                                            */
/* ************************************************************************** */
static int	compute_tx(mlx_texture_t *t, double w, t_ray *ray)
{
	int	tx;

	tx = (int)(w * t->width);
	if ((ray->side == 0 && ray->raydir.x > 0)
		|| (ray->side == 1 && ray->raydir.y < 0))
		tx = t->width - tx - 1;
	return ((tx + t->width) % t->width);
}

/* ************************************************************************** */
/*                                                                            */
/*   Computes texture parameters needed to draw a wall slice.                 */
/*                                                                            */
/*   Steps:                                                                   */
/*    1. Calculate the exact location of wall hit:                            */
/*         - If vertical wall (side == 0), use Y-coordinate.                  */
/*         - If horizontal wall (side == 1), use X-coordinate.                */
/*                                                                            */
/*    2. Extract the fractional part to determine horizontal position `w`.    */
/*                                                                            */
/*    3. If the tile is a door, use special door texture logic (dynamic).     */
/*       Otherwise, select static wall texture.                               */
/*                                                                            */
/*    4. Compute the horizontal texture pixel `tx` for rendering.             */
/*                                                                            */
/* ************************************************************************** */
void	compute_texture_params(t_app *app, t_ray *ray, t_draw *draw)
{
	double			w;
	mlx_texture_t	*t;
	t_door			*d;

	if (ray->side == 0)
		w = app->cam.pos.y + ray->perpwalldist * ray->raydir.y;
	else
		w = app->cam.pos.x + ray->perpwalldist * ray->raydir.x;
	w -= floor(w);
	if (ray->hit_tile == '2' || ray->hit_tile == '3')
		d = get_door(&app->game, ray->map_x, ray->map_y);
	else
		d = NULL;
	if (d)
		t = get_door_texture(&app->game, d, ray, &w);
	else if (ray->hit_tile == '1')
		t = get_wall_texture(app, ray);
	else
		t = NULL;
	draw->tex = t;
	if (!t)
		return ;
	draw->tx = compute_tx(t, w, ray);
}
