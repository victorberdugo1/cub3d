/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 20:07:32 by victor            #+#    #+#             */
/*   Updated: 2025/04/15 17:55:49 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bonus/cub3D_bonus.h"

int32_t ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a)
{
    return (r << 24 | g << 16 | b << 8 | a);
}

uint32_t	convert_pixel(uint32_t px)
{
	uint8_t	red;
	uint8_t	green;
	uint8_t	blue;
	uint8_t	alpha;

	red = (px >> 16) & 0xFF;
	green = (px >> 8) & 0xFF;
	blue = px & 0xFF;
	alpha = (px >> 24) & 0xFF;
	return (ft_pixel(blue, green, red, alpha));
}

void ft_draw_background(t_app *app)
{
    int32_t floor_color = ft_pixel(app->game.floor_color[0], app->game.floor_color[1], app->game.floor_color[2], 255);
    int32_t ceiling_color = ft_pixel(app->game.ceiling_color[0], app->game.ceiling_color[1], app->game.ceiling_color[2], 255);

    int32_t horizon = HEIGHT / 2 - app->camera.view_z;

    // Aseguramos que el horizonte no se salga de pantalla
    if (horizon < 0) horizon = 0;
    if (horizon > HEIGHT) horizon = HEIGHT;

    for (uint32_t x = 0; x < WIDTH; x++) {
        // cielo
        for (int32_t y = 0; y < horizon; y++)
            mlx_put_pixel(app->image, x, y, ceiling_color);
        // suelo
        for (uint32_t y = horizon; y < HEIGHT; y++)
            mlx_put_pixel(app->image, x, y, floor_color);
    }
}

char safe_get_tile(t_game *game, int x, int y)
{
    if (y < 0 || y >= game->map_height) return '1';
    int len = strlen(game->map[y]);
    return (x < 0 || x >= len) ? '1' : game->map[y][x];
}

int collides(t_game *game, double new_x, double new_y)
{
    t_collision c = {
        .r = COLLISION_RADIUS,
        .min_i = (int)floor(new_y - COLLISION_RADIUS),
        .max_i = (int)ceil(new_y + COLLISION_RADIUS),
        .min_j = (int)floor(new_x - COLLISION_RADIUS),
        .max_j = (int)ceil(new_x + COLLISION_RADIUS)
    };

    for (int i = c.min_i; i <= c.max_i; i++) {
        for (int j = c.min_j; j <= c.max_j; j++) {
            if (safe_get_tile(game, j, i) == '1') {
                double closest_x = fmax(j, fmin(new_x, j + 1));
                double closest_y = fmax(i, fmin(new_y, i + 1));
                double dx = new_x - closest_x;
                double dy = new_y - closest_y;
                if ((dx*dx + dy*dy) < c.r*c.r) return 1;
            }
        }
    }
    return 0;
}

void render_scene(void *param)
{
    t_app *app = param;
    ft_draw_background(app);

    for (int x = 0; x < WIDTH; x++) {
        double cameraX = 2 * x / (double)WIDTH - 1;
        t_vec2 rayDir = {
            app->camera.dir.x + app->camera.plane.x * cameraX,
            app->camera.dir.y + app->camera.plane.y * cameraX
        };

        int mapX = (int)app->camera.pos.x;
        int mapY = (int)app->camera.pos.y;
        t_vec2 deltaDist = {fabs(1 / rayDir.x), fabs(1 / rayDir.y)};
        t_vec2 sideDist, step;
        int hit = 0, side = 0;

        step.x = (rayDir.x < 0) ? -1 : 1;
        sideDist.x = (rayDir.x < 0) ? (app->camera.pos.x - mapX) * deltaDist.x : (mapX + 1.0 - app->camera.pos.x) * deltaDist.x;
        step.y = (rayDir.y < 0) ? -1 : 1;
        sideDist.y = (rayDir.y < 0) ? (app->camera.pos.y - mapY) * deltaDist.y : (mapY + 1.0 - app->camera.pos.y) * deltaDist.y;

        while (!hit) {
            if (sideDist.x < sideDist.y) {
                sideDist.x += deltaDist.x;
                mapX += step.x;
                side = 0;
            } else {
                sideDist.y += deltaDist.y;
                mapY += step.y;
                side = 1;
            }
            if (mapY < 0 || mapY >= app->game.map_height || mapX < 0 || mapX >= (int)strlen(app->game.map[mapY]) || 
                safe_get_tile(&app->game, mapX, mapY) == '1') hit = 1;
        }

        double perpWallDist = (side == 0) ? (sideDist.x - deltaDist.x) : (sideDist.y - deltaDist.y);
        int lineHeight = (int)(HEIGHT / perpWallDist);
        int drawStart = -lineHeight / 2 + HEIGHT / 2 - app->camera.view_z;
        drawStart = (drawStart < 0) ? 0 : drawStart;
        int drawEnd = lineHeight / 2 + HEIGHT / 2 - app->camera.view_z;
        drawEnd = (drawEnd >= HEIGHT) ? HEIGHT - 1 : drawEnd;

        double wallX = (side == 0) ? app->camera.pos.y + perpWallDist * rayDir.y : app->camera.pos.x + perpWallDist * rayDir.x;
        wallX -= floor(wallX);

        mlx_texture_t *tex = NULL;
        if (side == 0) tex = (rayDir.x < 0) ? app->game.tex_we : app->game.tex_ea;
        else tex = (rayDir.y < 0) ? app->game.tex_no : app->game.tex_so;

		if (tex) {
			int texX = (int)(wallX * tex->width);
			if ((side == 0 && rayDir.x > 0) || (side == 1 && rayDir.y < 0)) texX = tex->width - texX - 1;
			texX = texX % tex->width;
			if (texX < 0) texX += tex->width;

			for (int y = drawStart; y < drawEnd; y++) {
				int d = (y + app->camera.view_z) * 256 - HEIGHT * 128 + lineHeight * 128;
				int texY = ((d * tex->height) / lineHeight) / 256;
				texY = texY % tex->height;
				if (texY < 0) texY += tex->height;

				uint32_t color = ((uint32_t *)tex->pixels)[texY * tex->width + texX];
				color = convert_pixel(color);
				mlx_put_pixel(app->image, x, y, color);
			}
		}

    }
}

void move_camera(void *param)
{
    t_app *app = param;
    mlx_t *mlx = app->mlx;

    if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE)) mlx_close_window(mlx);
    
    double new_x = app->camera.pos.x;
    double new_y = app->camera.pos.y;
    double move_speed = app->camera.move_speed;
    
    if (mlx_is_key_down(mlx, MLX_KEY_W)) { new_x += app->camera.dir.x * move_speed; new_y += app->camera.dir.y * move_speed; }
    if (mlx_is_key_down(mlx, MLX_KEY_S)) { new_x -= app->camera.dir.x * move_speed; new_y -= app->camera.dir.y * move_speed; }
    if (mlx_is_key_down(mlx, MLX_KEY_A)) { new_x += -app->camera.dir.y * move_speed; new_y += app->camera.dir.x * move_speed; }
    if (mlx_is_key_down(mlx, MLX_KEY_D)) { new_x += app->camera.dir.y * move_speed; new_y += -app->camera.dir.x * move_speed; }
    
    double view_speed = 5.0;
    if (mlx_is_key_down(mlx, MLX_KEY_UP)) app->camera.view_z = fmin(app->camera.view_z + view_speed, HEIGHT/2);
    if (mlx_is_key_down(mlx, MLX_KEY_DOWN)) app->camera.view_z = fmax(app->camera.view_z - view_speed, -HEIGHT/2);
    
    if (!collides(&app->game, new_x, new_y)) {
        app->camera.pos.x = new_x;
        app->camera.pos.y = new_y;
    }
    
    double rot = app->camera.rot_speed * (mlx_is_key_down(mlx, MLX_KEY_LEFT) ? -1 : mlx_is_key_down(mlx, MLX_KEY_RIGHT) ? 1 : 0);
    if (rot != 0) {
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
