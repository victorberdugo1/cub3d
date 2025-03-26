/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collition.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:17:00 by victor            #+#    #+#             */
/*   Updated: 2025/03/26 20:49:23 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

char	safe_get_tile(t_game *game, int x, int y)
{
	int	len;

	if (y < 0 || y >= game->map_height)
		return ('1');
	len = strlen(game->map[y]);
	if (x < 0 || x >= len)
		return ('1');
	return (game->map[y][x]);
}

static void	init_collision(t_collision *c, double new_x, double new_y)
{
	double	r;

	r = COLLISION_RADIUS;
	c->r = r;
	c->min_i = (int)floor(new_y - r);
	c->max_i = (int)ceil(new_y + r);
	c->min_j = (int)floor(new_x - r);
	c->max_j = (int)ceil(new_x + r);
}

static int	check_cell_collision(double new_x, double new_y,
		t_collision *c)
{
	double	closest_x;
	double	closest_y;
	double	dx;
	double	dy;

	closest_x = new_x;
	if (new_x < c->j)
		closest_x = c->j;
	else if (new_x > c->j + 1)
		closest_x = c->j + 1;
	closest_y = new_y;
	if (new_y < c->i)
		closest_y = c->i;
	else if (new_y > c->i + 1)
		closest_y = c->i + 1;
	dx = new_x - closest_x;
	dy = new_y - closest_y;
	if ((dx * dx + dy * dy) < (c->r * c->r))
		return (1);
	return (0);
}

int	collides(t_game *game, double new_x, double new_y)
{
	t_collision	c;

	init_collision(&c, new_x, new_y);
	c.i = c.min_i - 1;
	while (++c.i <= c.max_i)
	{
		c.j = c.min_j - 1;
		while (++c.j <= c.max_j)
		{
			if (safe_get_tile(game, c.j, c.i) == '1')
			{
				if (check_cell_collision(new_x, new_y, &c) == 1)
					return (1);
			}
		}
	}
	return (0);
}
