/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 20:07:32 by victor            #+#    #+#             */
/*   Updated: 2025/03/26 18:33:21 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

int32_t	ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a)
{
	return (r << 24 | g << 16 | b << 8 | a);
}

void	ft_draw_background(t_app *app)
{
	int32_t		floor_color;
	int32_t		ceiling_color;
	uint32_t	x;
	uint32_t	y;

	floor_color = ft_pixel(app->game.floor_color[0],
			app->game.floor_color[1], app->game.floor_color[2], 255);
	ceiling_color = ft_pixel(app->game.ceiling_color[0],
			app->game.ceiling_color[1], app->game.ceiling_color[2], 255);
	x = 0;
	while (++x < WIDTH)
	{
		y = 0;
		while (++y < HEIGHT / 2)
			mlx_put_pixel(app->image, x, y, ceiling_color);
		y = HEIGHT / 2;
		while (++y < HEIGHT)
			mlx_put_pixel(app->image, x, y, floor_color);
	}
}

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

int	collides(t_game *game, double new_x, double new_y)
{
	t_collision	c;
	int			i;
	int			j;

	c.r = COLLISION_RADIUS;
	c.min_i = (int)floor(new_y - c.r);
	c.max_i = (int)ceil(new_y + c.r);
	c.min_j = (int)floor(new_x - c.r);
	c.max_j = (int)ceil(new_x + c.r);
	i = c.min_i;
	while (i <= c.max_i)
	{
		j = c.min_j;
		while (j <= c.max_j)
		{
			if (safe_get_tile(game, j, i) == '1')
			{
				c.closest_x = new_x;
				if (new_x < j)
					c.closest_x = j;
				else if (new_x > j + 1)
					c.closest_x = j + 1;
				c.closest_y = new_y;
				if (new_y < i)
					c.closest_y = i;
				else if (new_y > i + 1)
					c.closest_y = i + 1;
				c.dx = new_x - c.closest_x;
				c.dy = new_y - c.closest_y;
				if ((c.dx * c.dx + c.dy * c.dy) < c.r * c.r)
					return (1);
			}
			j++;
		}
		i++;
	}
	return (0);
}

void	render_scene(void *param)
{
	t_app *app = (t_app *)param;
	ft_draw_background(app);
	for (int x = 0; x < WIDTH; x++)
	{
		double cameraX = 2 * x / (double)WIDTH - 1;
		t_vec2 rayDir = {app->camera.dir.x + app->camera.plane.x * cameraX,
			app->camera.dir.y + app->camera.plane.y * cameraX};
		int map_x = (int)app->camera.pos.x;
		int map_y = (int)app->camera.pos.y;
		t_vec2 deltaDist = {fabs(1 / rayDir.x), fabs(1 / rayDir.y)};
		t_vec2 sideDist;
		t_vec2 step;
		int hit = 0, side;
		if (rayDir.x < 0)
		{
			step.x = -1;
			sideDist.x = (app->camera.pos.x - map_x) * deltaDist.x;
		}
		else
		{
			step.x = 1;
			sideDist.x = (map_x + 1.0 - app->camera.pos.x) * deltaDist.x;
		}
		if (rayDir.y < 0)
		{
			step.y = -1;
			sideDist.y = (app->camera.pos.y - map_y) * deltaDist.y;
		}
		else
		{
			step.y = 1;
			sideDist.y = (map_y + 1.0 - app->camera.pos.y) * deltaDist.y;
		}
		while (!hit)
		{
			if (sideDist.x < sideDist.y)
			{
				sideDist.x += deltaDist.x;
				map_x += step.x;
				side = 0;
			}
			else
			{
				sideDist.y += deltaDist.y;
				map_y += step.y;
				side = 1;
			}
			if (map_y < 0 || map_y >= app->game.map_height)
			{
				hit = 1;
				break;
			}
			size_t line_length = strlen(app->game.map[map_y]);
			if (map_x < 0 || map_x >= (int)line_length)
			{
				hit = 1;
				break;
			}
			if (safe_get_tile(&app->game, map_x, map_y) == '1')
				hit = 1;
		}
		double perpWallDist = (side == 0) ? (sideDist.x - deltaDist.x)
			: (sideDist.y - deltaDist.y);
		int lineHeight = (int)(HEIGHT / perpWallDist);
		int drawStart = -lineHeight / 2 + HEIGHT / 2;
		if (drawStart < 0)
			drawStart = 0;
		int drawEnd = lineHeight / 2 + HEIGHT / 2;
		if (drawEnd >= HEIGHT)
			drawEnd = HEIGHT - 1;
		double wallX;
		if (side == 0)
			wallX = app->camera.pos.y + perpWallDist * rayDir.y;
		else
			wallX = app->camera.pos.x + perpWallDist * rayDir.x;
		wallX -= floor(wallX);
		mlx_texture_t *tex;
		if (side == 0)
			tex = (rayDir.x < 0) ? app->game.tex_we : app->game.tex_ea;
		else
			tex = (rayDir.y < 0) ? app->game.tex_no : app->game.tex_so;
		if (!tex)
		{
			fprintf(stderr, "Error: Textura no disponible.\n");
			continue;
		}
		int texWidth = tex->width;
		int texHeight = tex->height;
		int texX = (int)(wallX * texWidth);
		if (side == 0 && rayDir.x > 0)
			texX = texWidth - texX - 1;
		if (side == 1 && rayDir.y < 0)
			texX = texWidth - texX - 1;
		for (int y = drawStart; y < drawEnd; y++)
		{
			int d = y * 256 - HEIGHT * 128 + lineHeight * 128;
			int texY = ((d * texHeight) / lineHeight) / 256;
			uint32_t color = ((uint32_t *)tex->pixels)[texY * texWidth + texX];
			mlx_put_pixel(app->image, x, y, color);
		}
	}
}

void	move_camera(void *param)
{
	t_app *app = (t_app *)param;
	mlx_t *mlx = app->mlx;
	if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE))
		mlx_close_window(mlx);

	double new_x = app->camera.pos.x;
	double new_y = app->camera.pos.y;
	if (mlx_is_key_down(mlx, MLX_KEY_W))
	{
		new_x += app->camera.dir.x * app->camera.move_speed;
		new_y += app->camera.dir.y * app->camera.move_speed;
	}
	if (mlx_is_key_down(mlx, MLX_KEY_S))
	{
		new_x -= app->camera.dir.x * app->camera.move_speed;
		new_y -= app->camera.dir.y * app->camera.move_speed;
	}
	if (mlx_is_key_down(mlx, MLX_KEY_A))
	{
		new_x += -app->camera.dir.y * app->camera.move_speed;
		new_y += app->camera.dir.x * app->camera.move_speed;
	}
	if (mlx_is_key_down(mlx, MLX_KEY_D))
	{
		new_x += app->camera.dir.y * app->camera.move_speed;
		new_y += -app->camera.dir.x * app->camera.move_speed;
	}

	if (!collides(&app->game, new_x, new_y))
	{
		app->camera.pos.x = new_x;
		app->camera.pos.y = new_y;
	}
	if (mlx_is_key_down(mlx, MLX_KEY_LEFT))
	{
		double rot = -app->camera.rot_speed;
		double oldDirX = app->camera.dir.x;
		app->camera.dir.x = app->camera.dir.x * cos(rot) - app->camera.dir.y * sin(rot);
		app->camera.dir.y = oldDirX * sin(rot) + app->camera.dir.y * cos(rot);
		double oldPlaneX = app->camera.plane.x;
		app->camera.plane.x = app->camera.plane.x * cos(rot) - app->camera.plane.y * sin(rot);
		app->camera.plane.y = oldPlaneX * sin(rot) + app->camera.plane.y * cos(rot);
	}
	if (mlx_is_key_down(mlx, MLX_KEY_RIGHT))
	{
		double rot = app->camera.rot_speed;
		double oldDirX = app->camera.dir.x;
		app->camera.dir.x = app->camera.dir.x * cos(rot) - app->camera.dir.y * sin(rot);
		app->camera.dir.y = oldDirX * sin(rot) + app->camera.dir.y * cos(rot);
		double oldPlaneX = app->camera.plane.x;
		app->camera.plane.x = app->camera.plane.x * cos(rot) - app->camera.plane.y * sin(rot);
		app->camera.plane.y = oldPlaneX * sin(rot) + app->camera.plane.y * cos(rot);
	}
}

void	validate_map(t_game *game, t_camera *camera)
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
				/* Configuramos la cámara directamente */
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

void	parse_color(char *line, int color[3])
{
	sscanf(line, "%d,%d,%d", &color[0], &color[1], &color[2]);
}

void	process_lines(char **lines, int line_count, t_game *game, t_camera *camera)
{
	int map_lines_count = 0;
	char **temp_map = NULL;
	for (int i = 0; i < line_count; i++)
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

void	close_window(void *param)
{
	t_app *app = (t_app *)param;
	mlx_close_window(app->mlx);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Error: Usage: %s <map_file.cub>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char **lines = NULL;
	int line_count = 0;

	if (read_lines(argv[1], &lines, &line_count) == -1)
	{
		fprintf(stderr, "Error: Failed to load map %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	t_app app;
	memset(&app, 0, sizeof(app));

	process_lines(lines, line_count, &app.game, &app.camera);
	for (int i = 0; i < line_count; i++)
		free(lines[i]);
	free(lines);

	app.camera.move_speed = 0.05;
	app.camera.rot_speed = 0.05;

	app.mlx = mlx_init(WIDTH, HEIGHT, "cub3D", true);
	if (!app.mlx)
	{
		fprintf(stderr, "Error: %s\n", mlx_strerror(mlx_errno));
		return EXIT_FAILURE;
	}

	app.image = mlx_new_image(app.mlx, WIDTH, HEIGHT);
	if (!app.image)
	{
		mlx_terminate(app.mlx);
		fprintf(stderr, "Error: %s\n", mlx_strerror(mlx_errno));
		return EXIT_FAILURE;
	}

	if (mlx_image_to_window(app.mlx, app.image, 0, 0) == -1)
	{
		mlx_terminate(app.mlx);
		fprintf(stderr, "Error: %s\n", mlx_strerror(mlx_errno));
		return EXIT_FAILURE;
	}

	if (!app.game.texture_no || !app.game.texture_so ||
			!app.game.texture_we || !app.game.texture_ea)
	{
		fprintf(stderr, "Error: Missing texture paths in map file.\n");
		mlx_terminate(app.mlx);
		return EXIT_FAILURE;
	}

	app.game.tex_no = mlx_load_png(app.game.texture_no);
	app.game.tex_so = mlx_load_png(app.game.texture_so);
	app.game.tex_we = mlx_load_png(app.game.texture_we);
	app.game.tex_ea = mlx_load_png(app.game.texture_ea);

	if (!app.game.tex_no || !app.game.tex_so ||
			!app.game.tex_we || !app.game.tex_ea)
	{
		fprintf(stderr, "Error: Failed to load one or more textures.\n");
		mlx_terminate(app.mlx);
		return EXIT_FAILURE;
	}

	mlx_loop_hook(app.mlx, render_scene, &app);
	mlx_loop_hook(app.mlx, move_camera, &app);
	mlx_close_hook(app.mlx, close_window, &app);
	mlx_loop(app.mlx);

	mlx_delete_texture(app.game.tex_no);
	mlx_delete_texture(app.game.tex_so);
	mlx_delete_texture(app.game.tex_we);
	mlx_delete_texture(app.game.tex_ea);
	free(app.game.texture_no);
	free(app.game.texture_so);
	free(app.game.texture_we);
	free(app.game.texture_ea);
	for (int i = 0; i < app.game.map_height; i++)
		free(app.game.map[i]);
	free(app.game.map);

	mlx_terminate(app.mlx);

	return EXIT_SUCCESS;
}
