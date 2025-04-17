/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 19:46:33 by victor            #+#    #+#             */
/*   Updated: 2025/04/17 02:12:09 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

/* ************************************************************************** */
/*                                                                            */
/*   Validates the command-line arguments. Checks that exactly one argument   */
/*   is provided and that the file extension is ".cub". Returns EXIT_SUCCESS  */
/*   if valid, otherwise returns EXIT_FAILURE.                                */
/*                                                                            */
/* ************************************************************************** */
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

/* ************************************************************************** */
/*                                                                            */
/*   Initializes the MLX instance and creates a new image with the specified  */
/*   WIDTH and HEIGHT. If any initialization fails, prints an error message   */
/*   and returns -1. Otherwise, assigns the MLX instance and image to the app */
/*                                                                            */
/* ************************************************************************** */
static int	init_mlx_and_image(t_app *app)
{
	app->mlx = mlx_init(WIDTH, HEIGHT, "cub3D", true);
	if (!app->mlx)
	{
		printf("Error: %s\n", mlx_strerror(mlx_errno));
		return (-1);
	}
	app->image = mlx_new_image(app->mlx, WIDTH, HEIGHT);
	if (!app->image)
	{
		mlx_terminate(app->mlx);
		printf("Error: %s\n", mlx_strerror(mlx_errno));
		return (-1);
	}
	if (mlx_image_to_window(app->mlx, app->image, 0, 0) == -1)
	{
		mlx_terminate(app->mlx);
		printf("Error: %s\n", mlx_strerror(mlx_errno));
		return (-1);
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Loads texture PNG files using the texture paths stored in the game       */
/*   structure. If any texture path is missing or any texture fails to load,  */
/*   prints an error message, terminates MLX, and returns -1. Otherwise,      */
/*	 loads  all textures and returns 0.                                       */
/*                                                                            */
/* ************************************************************************** */
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
	if (app->game.texture_door) {
        app->game.tex_door = mlx_load_png(app->game.texture_door);
    }
    if (app->game.texture_door_w) {
        app->game.tex_door_w = mlx_load_png(app->game.texture_door_w);
    }

    // Verificar carga correcta
    if ((app->game.tex_door && !app->game.tex_door_w) ||
        (!app->game.tex_door && app->game.tex_door_w)) {
        printf("Error\nBoth door textures (D/W) must be provided\n");
        cleanup(app);
        return (-1);
    }
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Sets up the MLX loop hooks for rendering, camera movement, and window    */
/*   closing. Then enters the MLX main loop.                                  */
/*                                                                            */
/* ************************************************************************** */
static void	run_loop(t_app *app)
{
	mlx_set_cursor_mode(app->mlx, MLX_MOUSE_HIDDEN);
	mlx_loop_hook(app->mlx, render_scene, app);
	mlx_loop_hook(app->mlx, move_camera, app);
	mlx_close_hook(app->mlx, close_window, app);
	mlx_loop(app->mlx);
}

/* ************************************************************************** */
/*                                                                            */
/*   Entry point of the program. It validates usage, loads the map,           */
/*   initializes the application and MLX, loads textures, sets camera         */
/*   parameters, and finally enters the main loop. Cleans up resources before */
/*   exiting. Camera rot speed and move speed are mesured in units/second.    */
/*                                                                            */
/* ************************************************************************** */
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
	app.camera.move_speed = 3.;
	app.camera.rot_speed = 3.;
	if (init_mlx_and_image(&app) == -1)
		return (EXIT_FAILURE);
	if (load_game_textures(&app) == -1)
		return (EXIT_FAILURE);
	mlx_set_mouse_pos(app.mlx, WIDTH/2, HEIGHT/2);
	run_loop(&app);
	return (cleanup(&app), EXIT_SUCCESS);
}
