/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:02:45 by victor            #+#    #+#             */
/*   Updated: 2025/03/26 21:15:18 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

static void	validate_map(t_game *game, t_camera *camera)
{
	int spawn_count = 0;
	for (int i = 0; i < game->map_height; i++)
	{
		int len = strlen(game->map[i]);
		if (len == 0)
		{
			fprintf(stderr, "Error\nEmpty map line\n");
			exit(EXIT_FAILURE);
		}
		if (i == 0 || i == game->map_height - 1)
		{
			for (int j = 0; j < len; j++)
			{
				if (game->map[i][j] != '1' && game->map[i][j] != ' ')
				{
					fprintf(stderr, "Error\nMap is not closed at row %d, col %d\n", i, j);
					exit(EXIT_FAILURE);
				}
			}
		}
		else
		{
			int j = 0;
			while (j < len && game->map[i][j] == ' ')
				j++;
			if (j < len && game->map[i][j] != '1')
			{
				fprintf(stderr, "Error\nMap is not closed on the left at row %d\n", i);
				exit(EXIT_FAILURE);
			}
			j = len - 1;
			while (j >= 0 && game->map[i][j] == ' ')
				j--;
			if (j >= 0 && game->map[i][j] != '1')
			{
				fprintf(stderr, "Error\nMap is not closed on the right at row %d\n", i);
				exit(EXIT_FAILURE);
			}
		}
		for (int j = 0; j < len; j++)
		{
			char c = game->map[i][j];
			if (c != ' ' && c != '0' && c != '1' &&
					c != 'N' && c != 'S' && c != 'E' && c != 'W')
			{
				fprintf(stderr, "Error\nInvalid character '%c' in map at row %d, col %d\n", c, i, j);
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
		fprintf(stderr, "Error\nMap must have exactly one spawn point. Found %d\n", spawn_count);
		exit(EXIT_FAILURE);
	}
}
int	read_lines(const char *filename, char ***lines, int *line_count)
{
	FILE *file = fopen(filename, "r");
	if (!file)
		return -1;
	char **temp_lines = NULL;
	char buffer[256];
	int count = 0;
	while (fgets(buffer, sizeof(buffer), file))
	{
		temp_lines = realloc(temp_lines, (count + 1) * sizeof(char *));
		temp_lines[count] = strdup(buffer);
		temp_lines[count][strcspn(temp_lines[count], "\n")] = '\0';
		count++;
	}
	fclose(file);
	*lines = temp_lines;
	*line_count = count;
	return 0;
}
static void parse_color(char *line, int color[3])
{
	int r, g, b;
	if (sscanf(line, "%d,%d,%d", &r, &g, &b) != 3) {
		fprintf(stderr, "Error: Formato de color inválido\n");
		exit(EXIT_FAILURE);
	}
	if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
		fprintf(stderr, "Error: Valores RGB fuera de rango (0-255)\n");
		exit(EXIT_FAILURE);
	}
	color[0] = r;
	color[1] = g;
	color[2] = b;
}
void	process_lines(char **lines, int count, t_game *game, t_camera *camera)
{
	int map_lines_count = 0;
	char **temp_map = NULL;
	for (int i = 0; i < count; i++)
	{
		if (strncmp(lines[i], "NO ", 3) == 0)
			game->texture_no = strdup(lines[i] + 3);
		else if (strncmp(lines[i], "SO ", 3) == 0)
			game->texture_so = strdup(lines[i] + 3);
		else if (strncmp(lines[i], "WE ", 3) == 0)
			game->texture_we = strdup(lines[i] + 3);
		else if (strncmp(lines[i], "EA ", 3) == 0)
			game->texture_ea = strdup(lines[i] + 3);
		else if (strncmp(lines[i], "F ", 2) == 0)
			parse_color(lines[i] + 2, game->floor_color);
		else if (strncmp(lines[i], "C ", 2) == 0)
			parse_color(lines[i] + 2, game->ceiling_color);
		else if (lines[i][0] == '1' || lines[i][0] == ' ')
		{
			temp_map = realloc(temp_map, (map_lines_count + 1) * sizeof(char *));
			temp_map[map_lines_count] = strdup(lines[i]);
			map_lines_count++;
		}
	}
	game->map = temp_map;
	game->map_height = map_lines_count;
	validate_map(game, camera);
}
