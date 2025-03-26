/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 19:49:25 by victor            #+#    #+#             */
/*   Updated: 2025/03/26 21:19:10 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

void	close_window(void *param)
{
	t_app *app = (t_app *)param;
	mlx_close_window(app->mlx);
}

int	load_map(char *filename, char ***lines, int *line_count)
{
	int	ret;

	ret = read_lines(filename, lines, line_count);
	if (ret == -1)
	{
		fprintf(stderr, "Error: Failed to load map %s\n", filename);
		return (-1);
	}
	return (0);
}

void	free_map_lines(char **lines, int line_count)
{
	int	i;

	i = 0;
	while (i < line_count)
	{
		free(lines[i]);
		i = i + 1;
	}
	free(lines);
}

int	init_app_struct(t_app *app, char **lines, int line_count)
{
	memset(app, 0, sizeof(*app));
	process_lines(lines, line_count, &app->game, &app->camera);
	return (0);
}

void	cleanup(t_app *app)
{
	int	i;

	mlx_delete_texture(app->game.tex_no);
	mlx_delete_texture(app->game.tex_so);
	mlx_delete_texture(app->game.tex_we);
	mlx_delete_texture(app->game.tex_ea);
	free(app->game.texture_no);
	free(app->game.texture_so);
	free(app->game.texture_we);
	free(app->game.texture_ea);
	i = 0;
	while (i < app->game.map_height)
	{
		free(app->game.map[i]);
		i = i + 1;
	}
	free(app->game.map);
	mlx_terminate(app->mlx);
}
