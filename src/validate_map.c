/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_map.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 21:58:25 by victor            #+#    #+#             */
/*   Updated: 2025/04/02 00:05:21 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

static void	check_empty_line(t_game *game)
{
	char	*trimmed;

	while (game->map_height > 0)
	{
		trimmed = ft_strtrim(game->map[game->map_height - 1], " ");
		if (ft_strlen(trimmed) == 0)
		{
			free(game->map[game->map_height - 1]);
			game->map[game->map_height - 1] = NULL;
			game->map_height--;
		}
		else
		{
			free(trimmed);
			break ;
		}
		free(trimmed);
	}
}

static void	validate_borders(char *line, int is_border)
{
	int	j;
	int	len;

	len = ft_strlen(line);
	if (is_border)
	{
		j = -1;
		while (++j < len)
			if (line[j] != '1' && line[j] != ' ')
				exit(write(2, "Error\nMap is not closed\n", 25));
	}
	else
	{
		j = -1;
		while (++j < len && line[j] == ' ')
			;
		if (j < len && line[j] != '1')
			exit(write(2, "Error\nMap is left open\n", 24));
		j = len;
		while (--j >= 0 && line[j] == ' ')
			;
		if (j >= 0 && line[j] != '1')
			exit(write(2, "Error\nMap is right open\n", 25));
	}
}

static void	set_camera(t_camera *camera, char dir, int x, int y)
{
	camera->pos.x = x + 0.5;
	camera->pos.y = y + 0.5;
	if (dir == 'N')
	{
		camera->dir = (t_vec2){0, -1};
		camera->plane = (t_vec2){0.66, 0};
	}
	else if (dir == 'S')
	{
		camera->dir = (t_vec2){0, 1};
		camera->plane = (t_vec2){-0.66, 0};
	}
	else if (dir == 'E')
	{
		camera->dir = (t_vec2){1, 0};
		camera->plane = (t_vec2){0, 0.66};
	}
	else if (dir == 'W')
	{
		camera->dir = (t_vec2){-1, 0};
		camera->plane = (t_vec2){0, -0.66};
	}
}

static void	process_row_chars(t_game *g, t_camera *cam, int i, int *count)
{
	int		j;
	char	c;

	j = -1;
	while (g->map[i][++j])
	{
		c = g->map[i][j];
		if (!ft_strchr(" 01NSEW", c))
			exit(write(2, "Error\nInvalid character in map\n", 31));
		if (ft_strchr("NSEW", c))
		{
			set_camera(cam, c, j, i);
			(*count)++;
			g->map[i][j] = '0';
		}
	}
}

void	validate_map(t_game *game, t_camera *camera)
{
	int		i;
	int		spawn_count;

	check_empty_line(game);
	spawn_count = 0;
	i = -1;
	while (++i < game->map_height)
	{
		if (ft_strlen(game->map[i]) == 0)
			exit(write(2, "Error\nEmpty map line\n", 21));
		validate_borders(game->map[i], (i == 0 || i == game->map_height - 1));
		process_row_chars(game, camera, i, &spawn_count);
	}
	if (spawn_count != 1)
		exit(write(2, "Error\nMap must have only one spawn point\n", 42));
}
