/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:02:45 by victor            #+#    #+#             */
/*   Updated: 2025/03/28 13:35:40 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

static void	validate_map(t_game *game, t_camera *camera)
{
	int		spawn_count;
	int		i;
	int		j;
	int		len;
	char	c;

	spawn_count = 0;
	i = -1;
	while (++i < game->map_height)
	{
		len = ft_strlen(game->map[i]);
		if (len == 0)
		{
			printf("Error\nEmpty map line\n");
			exit(EXIT_FAILURE);
		}
		if (i == 0 || i == game->map_height - 1)
		{
			j = -1;
			while (++j < len)
			{
				if (game->map[i][j] != '1' && game->map[i][j] != ' ')
				{
					printf("Error\nMap not closed at row %d, col %d\n", i, j);
					exit(EXIT_FAILURE);
				}
			}
		}
		else
		{
			j = 0;
			while (j < len && game->map[i][j] == ' ')
				j++;
			if (j < len && game->map[i][j] != '1')
			{
				printf("Error\nMap is not closed on the left at row %d\n", i);
				exit(EXIT_FAILURE);
			}
			j = len - 1;
			while (j >= 0 && game->map[i][j] == ' ')
				j--;
			if (j >= 0 && game->map[i][j] != '1')
			{
				printf("Error\nMap is not closed on the right at row %d\n", i);
				exit(EXIT_FAILURE);
			}
		}
		j = -1;
		while (++j < len)
		{
			c = game->map[i][j];
			if (c != ' ' && c != '0' && c != '1'
				&& c != 'N' && c != 'S' && c != 'E' && c != 'W')
			{
				printf("Error\nInvalid char '%c' at row %d, col %d\n", c, i, j);
				exit(EXIT_FAILURE);
			}
			if (c == 'N' || c == 'S' || c == 'E' || c == 'W')
			{
				spawn_count++;
				camera->pos.x = j + 0.5;
				camera->pos.y = i + 0.5;
				if (c == 'N')
				{
					camera->dir.x = 0;
					camera->dir.y = -1;
					camera->plane.x = 0.66;
					camera->plane.y = 0;
				}
				else if (c == 'S')
				{
					camera->dir.x = 0;
					camera->dir.y = 1;
					camera->plane.x = -0.66;
					camera->plane.y = 0;
				}
				else if (c == 'E')
				{
					camera->dir.x = 1;
					camera->dir.y = 0;
					camera->plane.x = 0;
					camera->plane.y = 0.66;
				}
				else if (c == 'W')
				{
					camera->dir.x = -1;
					camera->dir.y = 0;
					camera->plane.x = 0;
					camera->plane.y = -0.66;
				}
				game->map[i][j] = '0';
			}
		}
	}
	if (spawn_count != 1)
	{
		printf("Error\nMap must have only one spawn point. Found %d\n",
			spawn_count);
		exit(EXIT_FAILURE);
	}
}

int	read_lines(const char *filename, char ***lines, int *line_count)
{
	int		fd;
	int		count;
	char	**temp;
	char	*line;
	char	buf[2];
	char	*new_line;
	char	**new_temp;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (-1);
	count = 0;
	temp = NULL;
	line = ft_strdup("");
	if (!line)
		return (-1);
	while (read(fd, buf, 1) > 0)
	{
		buf[1] = '\0';
		new_line = ft_strjoin(line, buf);
		if (!new_line)
			return (-1);
		free(line);
		line = new_line;
		if (buf[0] == '\n')
		{
			line[ft_strlen(line) - 1] = '\0';
			new_temp = ft_realloc(temp, (count + 1) * sizeof(char *));
			if (!new_temp)
				return (-1);
			temp = new_temp;
			temp[count++] = line;
			line = ft_strdup("");
			if (!line)
				return (-1);
		}
	}
	if (*line)
	{
		new_temp = ft_realloc(temp, (count + 1) * sizeof(char *));
		if (!new_temp)
			return (-1);
		temp = new_temp;
		temp[count++] = line;
	}
	else
		free(line);
	close(fd);
	*lines = temp;
	*line_count = count;
	return (0);
}

static void	parse_color(char *line, int color[3])
{
	int		i;
	char	*s;

	i = -1;
	s = line;
	while (++i < 3)
	{
		while (*s == ' ' || *s == '\t')
			s++;
		if (!*s)
			exit(write(1, "Error: Formato de color inválido\n", 33));
		color[i] = ft_atoi(s);
		while (*s >= '0' && *s <= '9')
			s++;
		if (color[i] < 0 || color[i] > 255)
			exit(write(1, "Error: Valores RGB fuera de rango (0-255)\n", 42));
		if (i < 2)
		{
			while (*s == ' ' || *s == '\t')
				s++;
			if (*s != ',')
				exit(write(1, "Error: Formato de color inválido\n", 33));
			s++;
		}
	}
}

void	process_lines(char **lines, int count, t_game *game, t_camera *camera)
{
	int		lines_count;
	char	**temp_map;
	int		i;

	lines_count = 0;
	temp_map = NULL;
	i = -1;
	while (++i < count)
	{
		if (ft_strncmp(lines[i], "NO ", 3) == 0)
			game->texture_no = ft_strdup(lines[i] + 3);
		else if (ft_strncmp(lines[i], "SO ", 3) == 0)
			game->texture_so = ft_strdup(lines[i] + 3);
		else if (ft_strncmp(lines[i], "WE ", 3) == 0)
			game->texture_we = ft_strdup(lines[i] + 3);
		else if (ft_strncmp(lines[i], "EA ", 3) == 0)
			game->texture_ea = ft_strdup(lines[i] + 3);
		else if (ft_strncmp(lines[i], "F ", 2) == 0)
			parse_color(lines[i] + 2, game->floor_color);
		else if (ft_strncmp(lines[i], "C ", 2) == 0)
			parse_color(lines[i] + 2, game->ceiling_color);
		else if (lines[i][0] == '1' || lines[i][0] == ' ')
		{
			temp_map = ft_realloc(temp_map, (lines_count + 1) * sizeof(char *));
			temp_map[lines_count] = ft_strdup(lines[i]);
			lines_count++;
		}
	}
	game->map = temp_map;
	game->map_height = lines_count;
	validate_map(game, camera);
}
