/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:02:45 by victor            #+#    #+#             */
/*   Updated: 2025/04/18 13:41:07 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Parses an RGB color string into an integer array.                        */
/*                                                                            */
/*   - Skips leading spaces and tabs.                                         */
/*   - Extracts three integer values using `ft_atoi()`.                       */
/*   - Ensures values are in the range [0, 255].                              */
/*   - Verifies correct formatting (e.g., commas between values).             */
/*   - Exits with an error message if the format is invalid.                  */
/*                                                                            */
/* ************************************************************************** */
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
			exit(write(1, "Error\nInvalid color format\n", 28));
		color[i] = ft_atoi(s);
		while (*s >= '0' && *s <= '9')
			s++;
		if (color[i] < 0 || color[i] > 255)
			exit(write(1, "Error\nRGB values out of range (0-255)\n", 39));
		if (i < 2)
		{
			while (*s == ' ' || *s == '\t')
				s++;
			if (*s != ',')
				exit(write(1, "Error\nInvalid color format\n", 28));
			s++;
		}
	}
}

/* ************************************************************************** */
/*                                                                            */
/*   Checks if a texture type has already been assigned to prevent duplicates.*/
/*                                                                            */
/*   - Compares the given `texture_type` string with known texture labels.    */
/*   - Returns `1` (true) if the texture has already been set, otherwise `0`. */
/*                                                                            */
/* ************************************************************************** */
static int	check_texture_duplicate(t_game *game, const char *texture_type)
{
	if (!ft_strncmp(texture_type, "NO", 2) && game->texture_no)
		return (1);
	if (!ft_strncmp(texture_type, "SO", 2) && game->texture_so)
		return (1);
	if (!ft_strncmp(texture_type, "WE", 2) && game->texture_we)
		return (1);
	if (!ft_strncmp(texture_type, "EA", 2) && game->texture_ea)
		return (1);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Processes a texture or color directive from the map file.                */
/*                                                                            */
/*   - Checks for duplicate texture definitions.                              */
/*   - Assigns texture paths to their respective variables.                   */
/*   - Calls `parse_color()` for floor (F) and ceiling (C) colors.            */
/*                                                                            */
/* ************************************************************************** */
static void	process_texture(char *trimmed, t_game *game)
{
	if (check_texture_duplicate(game, trimmed))
	{
		exit(write(2, "Error\nDuplicate texture\n", 24));
	}
	if (!ft_strncmp(trimmed, "NO", 2))
		game->texture_no = ft_strtrim(skip_spaces(trimmed + 2), " ");
	else if (!ft_strncmp(trimmed, "SO", 2))
		game->texture_so = ft_strtrim(skip_spaces(trimmed + 2), " ");
	else if (!ft_strncmp(trimmed, "WE", 2))
		game->texture_we = ft_strtrim(skip_spaces(trimmed + 2), " ");
	else if (!ft_strncmp(trimmed, "EA", 2))
		game->texture_ea = ft_strtrim(skip_spaces(trimmed + 2), " ");
	else if (!ft_strncmp(trimmed, "F", 1))
		parse_color(skip_spaces(trimmed + 1), game->floor_color);
	else if (!ft_strncmp(trimmed, "C", 1))
		parse_color(skip_spaces(trimmed + 1), game->ceiling_color);
	else if (!ft_strncmp(trimmed, "D", 1))
        game->texture_door = ft_strtrim(skip_spaces(trimmed + 1), " ");
    else if (!ft_strncmp(trimmed, "M", 1))
        game->texture_door_w = ft_strtrim(skip_spaces(trimmed + 1), " ");
    else if (!ft_strncmp(trimmed, "B", 1))
        game->texture_enemy = ft_strtrim(skip_spaces(trimmed + 1), " ");
}

/* ************************************************************************** */
/*                                                                            */
/*   Processes a line from the map file, classifying it as:                   */
/*                                                                            */
/*   - A map row (if it starts with '1' or if map parsing has started).       */
/*   - A texture or color definition.                                         */
/*   - An empty line (ignored).                                               */
/*                                                                            */
/*   - Stores map lines dynamically using `ft_realloc()` and `ft_strdup()`.   */
/*   - Calls `process_texture()` for texture/color definitions.               */
/*                                                                            */
/* ************************************************************************** */
static void	process_txt_col_map(char *line, t_game *game, char ***temp_map,
		int *lines)
{
	static int	map_started = 0;
	char		*trimmed;

	trimmed = skip_spaces(line);
	if (map_started || trimmed[0] == '1')
	{
		map_started = 1;
		*temp_map = ft_realloc(*temp_map, ((*lines) + 1) * sizeof(char *));
		(*temp_map)[*lines] = ft_strdup(line);
		(*lines)++;
	}
	else if (*trimmed == '\0')
		return ;
	else if (!ft_strncmp(trimmed, "NO", 2) || !ft_strncmp(trimmed, "SO", 2)
		|| !ft_strncmp(trimmed, "WE", 2) || !ft_strncmp(trimmed, "EA", 2)
		|| !ft_strncmp(trimmed, "F", 1) || !ft_strncmp(trimmed, "C", 1)
		|| !ft_strncmp(trimmed, "D", 1) || !ft_strncmp(trimmed, "M", 1)
		|| !ft_strncmp(trimmed, "B", 1))
		process_texture(trimmed, game);
}

/* ************************************************************************** */
/*                                                                            */
/*   Processes all lines from the map file and validates the final structure. */
/*                                                                            */
/*   - Reads and classifies each line.                                        */
/*   - Stores the map as a dynamic array (`temp_map`).                        */
/*   - Calls `validate_map()` to ensure the map's correctness.                */
/*   - Checks that all required textures are present.                         */
/*   - Ensures floor and ceiling colors are properly defined.                 */
/*                                                                            */
/* ************************************************************************** */
void	process_lines(char **lines, int c, t_game *game, t_camera *camera)
{
	int		lines_count;
	char	**temp_map;
	int		i;

	lines_count = 0;
	temp_map = NULL;
	i = -1;
	while (++i < c)
		process_txt_col_map(lines[i], game, &temp_map, &lines_count);
	game->map = temp_map;
	game->map_height = lines_count;
	validate_map(game, camera);
	if (!game->texture_no || !game->texture_so
		|| !game->texture_we || !game->texture_ea)
	{
		printf("Error\nMissing texture paths in map file\n");
		exit(EXIT_FAILURE);
	}
	if ((game->floor_color[0] == -1 && game->floor_color[1] == -1
			&& game->floor_color[2] == -1)
		|| (game->ceiling_color[0] == -1 && game->ceiling_color[1] == -1
			&& game->ceiling_color[2] == -1))
		exit(write(1, "Error\nMissing floor or ceiling color\n", 38));
}
