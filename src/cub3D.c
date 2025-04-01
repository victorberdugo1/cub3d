/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 19:46:33 by victor            #+#    #+#             */
/*   Updated: 2025/04/01 23:06:30 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

static int	check_usage(int argc, char **argv)
{
	int	len;

	if (argc != 2)
	{
		printf("Error: Usage: %s <map_file.cub>\n", argv[0]);
		return (EXIT_FAILURE);
	}
	len = ft_strlen(argv[1]);
	if (len < 4 || ft_strcmp(argv[1] + len - 4, ".cub") != 0)
	{
		printf("Error: Invalid file extension. Expected .cub\n");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static int	init_mlx_and_image(t_app *app)
{
	mlx_t			*mlx_instance;
	mlx_image_t		*img;

	mlx_instance = mlx_init(WIDTH, HEIGHT, "cub3D", true);
	if (!mlx_instance)
	{
		printf("Error: %s\n", mlx_strerror(mlx_errno));
		return (-1);
	}
	app->mlx = mlx_instance;
	img = mlx_new_image(mlx_instance, WIDTH, HEIGHT);
	if (!img)
	{
		mlx_terminate(mlx_instance);
		printf("Error: %s\n", mlx_strerror(mlx_errno));
		return (-1);
	}
	app->image = img;
	if (mlx_image_to_window(mlx_instance, img, 0, 0) == -1)
	{
		mlx_terminate(mlx_instance);
		printf("Error: %s\n", mlx_strerror(mlx_errno));
		return (-1);
	}
	return (0);
}

static int	load_game_textures(t_app *app)
{
	t_game	*game;

	game = &app->game;
	if (!game->texture_no || !game->texture_so
		|| !game->texture_we || !game->texture_ea)
	{
		printf("Error: Missing texture paths in map file.\n");
		mlx_terminate(app->mlx);
		return (-1);
	}
	game->tex_no = mlx_load_png(game->texture_no);
	game->tex_so = mlx_load_png(game->texture_so);
	game->tex_we = mlx_load_png(game->texture_we);
	game->tex_ea = mlx_load_png(game->texture_ea);
	if (!game->tex_no || !game->tex_so
		|| !game->tex_we || !game->tex_ea)
	{
		printf("Error: Failed to load one or more textures.\n");
		cleanup(app);
		return (-1);
	}
	return (0);
}

static void	run_loop(t_app *app)
{
	mlx_loop_hook(app->mlx, render_scene, app);
	mlx_loop_hook(app->mlx, move_camera, app);
	mlx_close_hook(app->mlx, close_window, app);
	mlx_loop(app->mlx);
}

int	main(int argc, char **argv)
{
	char	**lines;
	int		line_count;
	t_app	app;

	if (check_usage(argc, argv) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	lines = NULL;
	line_count = 0;
	if (load_map(argv[1], &lines, &line_count) == -1)
		return (EXIT_FAILURE);
	if (init_app_struct(&app, lines, line_count) != 0)
		return (EXIT_FAILURE);
	free_map_lines(lines, line_count);
	app.camera.move_speed = 0.05;
	app.camera.rot_speed = 0.05;
	if (init_mlx_and_image(&app) == -1)
		return (EXIT_FAILURE);
	if (load_game_textures(&app) == -1)
		return (EXIT_FAILURE);
	run_loop(&app);
	return (cleanup(&app), EXIT_SUCCESS);
}
