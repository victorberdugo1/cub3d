/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 19:46:33 by victor            #+#    #+#             */
/*   Updated: 2025/03/22 14:06:18 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define WIDTH 512
#define HEIGHT 512

static mlx_image_t *image;

typedef struct s_camera {
    t_vec2 pos;
    t_vec2 dir;
    t_vec2 plane;
    double move_speed;
    double rot_speed;
} t_camera;

typedef struct s_game {
    char *texture_no;
    char *texture_so;
    char *texture_we;
    char *texture_ea;
    int floor_color[3];
    int ceiling_color[3];
    char **map;
    int map_height;
    mlx_texture_t *tex_no;
    mlx_texture_t *tex_so;
    mlx_texture_t *tex_we;
    mlx_texture_t *tex_ea;
} t_game;

t_camera camera = {
    .pos = {2.0, 2.0},
    .dir = {1.0, 0.0},
    .plane = {0.0, 0.66},
    .move_speed = 0.05,
    .rot_speed = 0.05
};

t_game game;

int32_t ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a) {
    return (r << 24 | g << 16 | b << 8 | a);
}

void ft_draw_background() {
    int32_t floor_color = ft_pixel(game.floor_color[0], game.floor_color[1], game.floor_color[2], 255);
    int32_t ceiling_color = ft_pixel(game.ceiling_color[0], game.ceiling_color[1], game.ceiling_color[2], 255);
    for (uint32_t x = 0; x < WIDTH; ++x) {
        for (uint32_t y = 0; y < HEIGHT / 2; ++y)
            mlx_put_pixel(image, x, y, ceiling_color);
        for (uint32_t y = HEIGHT / 2; y < HEIGHT; ++y)
            mlx_put_pixel(image, x, y, floor_color);
    }
}

void render_scene(void *param) {
    (void)param;
    ft_draw_background();
    for (int x = 0; x < WIDTH; x++) {
        double cameraX = 2 * x / (double)WIDTH - 1;
        t_vec2 rayDir = {camera.dir.x + camera.plane.x * cameraX, camera.dir.y + camera.plane.y * cameraX};
        int map_x = (int)camera.pos.x;
        int map_y = (int)camera.pos.y;
        t_vec2 deltaDist = {fabs(1 / rayDir.x), fabs(1 / rayDir.y)};
        t_vec2 sideDist;
        t_vec2 step;
        int hit = 0, side;
        if (rayDir.x < 0) {
            step.x = -1;
            sideDist.x = (camera.pos.x - map_x) * deltaDist.x;
        } else {
            step.x = 1;
            sideDist.x = (map_x + 1.0 - camera.pos.x) * deltaDist.x;
        }
        if (rayDir.y < 0) {
            step.y = -1;
            sideDist.y = (camera.pos.y - map_y) * deltaDist.y;
        } else {
            step.y = 1;
            sideDist.y = (map_y + 1.0 - camera.pos.y) * deltaDist.y;
        }
        while (!hit) {
            if (sideDist.x < sideDist.y) {
                sideDist.x += deltaDist.x;
                map_x += step.x;
                side = 0;
            } else {
                sideDist.y += deltaDist.y;
                map_y += step.y;
                side = 1;
            }
            if (map_y < 0 || map_y >= game.map_height) { hit = 1; break; }
            size_t line_length = strlen(game.map[map_y]);
            if (map_x < 0 || map_x >= (int)line_length) { hit = 1; break; }
            if (game.map[map_y][map_x] == '1')
                hit = 1;
        }
        double perpWallDist = (side == 0) ? (sideDist.x - deltaDist.x) : (sideDist.y - deltaDist.y);
        int lineHeight = (int)(HEIGHT / perpWallDist);
        int drawStart = -lineHeight / 2 + HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + HEIGHT / 2;
        if (drawEnd >= HEIGHT) drawEnd = HEIGHT - 1;
        double wallX;
        if (side == 0)
            wallX = camera.pos.y + perpWallDist * rayDir.y;
        else
            wallX = camera.pos.x + perpWallDist * rayDir.x;
        wallX -= floor(wallX);
        mlx_texture_t *tex;
        if (side == 0)
            tex = (rayDir.x < 0) ? game.tex_ea : game.tex_we;
        else
            tex = (rayDir.y < 0) ? game.tex_no : game.tex_so;

        // Verificar que la textura esté disponible
        if (!tex) {
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
        for (int y = drawStart; y < drawEnd; y++) {
            int d = y * 256 - HEIGHT * 128 + lineHeight * 128;
            int texY = ((d * texHeight) / lineHeight) / 256;
            uint32_t color = ((uint32_t *)tex->pixels)[texY * texWidth + texX];
            mlx_put_pixel(image, x, y, color);
        }
    }
}

void move_camera(void *param) {
    mlx_t *mlx = param;
    if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE))
        mlx_close_window(mlx);
    /* Movimiento: W (adelante), S (atrás), A (izquierda/strafe), D (derecha/strafe) */
    if (mlx_is_key_down(mlx, MLX_KEY_W)) {
        int new_x = (int)(camera.pos.x + camera.dir.x * camera.move_speed);
        int new_y = (int)(camera.pos.y);
        if (game.map[new_y][new_x] == '0' || game.map[new_y][new_x] == ' ')
            camera.pos.x += camera.dir.x * camera.move_speed;
        new_x = (int)(camera.pos.x);
        new_y = (int)(camera.pos.y + camera.dir.y * camera.move_speed);
        if (game.map[new_y][new_x] == '0' || game.map[new_y][new_x] == ' ')
            camera.pos.y += camera.dir.y * camera.move_speed;
    }
    if (mlx_is_key_down(mlx, MLX_KEY_S)) {
        int new_x = (int)(camera.pos.x - camera.dir.x * camera.move_speed);
        int new_y = (int)(camera.pos.y);
        if (game.map[new_y][new_x] == '0' || game.map[new_y][new_x] == ' ')
            camera.pos.x -= camera.dir.x * camera.move_speed;
        new_x = (int)(camera.pos.x);
        new_y = (int)(camera.pos.y - camera.dir.y * camera.move_speed);
        if (game.map[new_y][new_x] == '0' || game.map[new_y][new_x] == ' ')
            camera.pos.y -= camera.dir.y * camera.move_speed;
    }
    /* Strafing: A (izquierda), D (derecha) */
    if (mlx_is_key_down(mlx, MLX_KEY_A)) {
        double strafeX = -camera.dir.y;
        double strafeY = camera.dir.x;
        int new_x = (int)(camera.pos.x + strafeX * camera.move_speed);
        int new_y = (int)(camera.pos.y);
        if (game.map[new_y][new_x] == '0' || game.map[new_y][new_x] == ' ')
            camera.pos.x += strafeX * camera.move_speed;
        new_x = (int)(camera.pos.x);
        new_y = (int)(camera.pos.y + strafeY * camera.move_speed);
        if (game.map[new_y][new_x] == '0' || game.map[new_y][new_x] == ' ')
            camera.pos.y += strafeY * camera.move_speed;
    }
    if (mlx_is_key_down(mlx, MLX_KEY_D)) {
        double strafeX = camera.dir.y;
        double strafeY = -camera.dir.x;
        int new_x = (int)(camera.pos.x + strafeX * camera.move_speed);
        int new_y = (int)(camera.pos.y);
        if (game.map[new_y][new_x] == '0' || game.map[new_y][new_x] == ' ')
            camera.pos.x += strafeX * camera.move_speed;
        new_x = (int)(camera.pos.x);
        new_y = (int)(camera.pos.y + strafeY * camera.move_speed);
        if (game.map[new_y][new_x] == '0' || game.map[new_y][new_x] == ' ')
            camera.pos.y += strafeY * camera.move_speed;
    }
    /* Rotación con las flechas izquierda y derecha */
    if (mlx_is_key_down(mlx, MLX_KEY_LEFT)) {
        double rot = camera.rot_speed;
        double oldDirX = camera.dir.x;
        camera.dir.x = camera.dir.x * cos(rot) - camera.dir.y * sin(rot);
        camera.dir.y = oldDirX * sin(rot) + camera.dir.y * cos(rot);
        double oldPlaneX = camera.plane.x;
        camera.plane.x = camera.plane.x * cos(rot) - camera.plane.y * sin(rot);
        camera.plane.y = oldPlaneX * sin(rot) + camera.plane.y * cos(rot);
    }
    if (mlx_is_key_down(mlx, MLX_KEY_RIGHT)) {
        double rot = -camera.rot_speed;
        double oldDirX = camera.dir.x;
        camera.dir.x = camera.dir.x * cos(rot) - camera.dir.y * sin(rot);
        camera.dir.y = oldDirX * sin(rot) + camera.dir.y * cos(rot);
        double oldPlaneX = camera.plane.x;
        camera.plane.x = camera.plane.x * cos(rot) - camera.plane.y * sin(rot);
        camera.plane.y = oldPlaneX * sin(rot) + camera.plane.y * cos(rot);
    }
}

void validate_map(t_game *game) {
    int spawn_count = 0;
    for (int i = 0; i < game->map_height; i++) {
        int len = strlen(game->map[i]);
        if (len == 0) {
            fprintf(stderr, "Error\nEmpty map line\n");
            exit(EXIT_FAILURE);
        }
        if (i == 0 || i == game->map_height - 1) {
            for (int j = 0; j < len; j++) {
                if (game->map[i][j] != '1' && game->map[i][j] != ' ') {
                    fprintf(stderr, "Error\nMap is not closed at row %d, col %d\n", i, j);
                    exit(EXIT_FAILURE);
                }
            }
        } else {
            int j = 0;
            while (j < len && game->map[i][j] == ' ')
                j++;
            if (j < len && game->map[i][j] != '1') {
                fprintf(stderr, "Error\nMap is not closed on the left at row %d\n", i);
                exit(EXIT_FAILURE);
            }
            j = len - 1;
            while (j >= 0 && game->map[i][j] == ' ')
                j--;
            if (j >= 0 && game->map[i][j] != '1') {
                fprintf(stderr, "Error\nMap is not closed on the right at row %d\n", i);
                exit(EXIT_FAILURE);
            }
        }
        for (int j = 0; j < len; j++) {
            char c = game->map[i][j];
            if (c != ' ' && c != '0' && c != '1' && c != 'N' && c != 'S' && c != 'E' && c != 'W') {
                fprintf(stderr, "Error\nInvalid character '%c' in map at row %d, col %d\n", c, i, j);
                exit(EXIT_FAILURE);
            }
            if (c == 'N' || c == 'S' || c == 'E' || c == 'W') {
                spawn_count++;
                camera.pos.x = j + 0.5;
                camera.pos.y = i + 0.5;
                if (c == 'N') {
                    camera.dir.x = 0;
                    camera.dir.y = -1;
                    camera.plane.x = 0.66;
                    camera.plane.y = 0;
                } else if (c == 'S') {
                    camera.dir.x = 0;
                    camera.dir.y = 1;
                    camera.plane.x = -0.66;
                    camera.plane.y = 0;
                } else if (c == 'E') {
                    camera.dir.x = 1;
                    camera.dir.y = 0;
                    camera.plane.x = 0;
                    camera.plane.y = 0.66;
                } else if (c == 'W') {
                    camera.dir.x = -1;
                    camera.dir.y = 0;
                    camera.plane.x = 0;
                    camera.plane.y = -0.66;
                }
                game->map[i][j] = '0';
            }
        }
    }
    if (spawn_count != 1) {
        fprintf(stderr, "Error\nMap must have exactly one spawn point. Found %d\n", spawn_count);
        exit(EXIT_FAILURE);
    }
}

int read_lines(const char *filename, char ***lines, int *line_count) {
    FILE *file = fopen(filename, "r");
    if (!file)
        return -1;
    char **temp_lines = NULL;
    char buffer[256];
    int count = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
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

void parse_color(char *line, int color[3]) {
    sscanf(line, "%d,%d,%d", &color[0], &color[1], &color[2]);
}

void process_lines(char **lines, int line_count, t_game *game) {
    int map_lines_count = 0;
    char **temp_map = NULL;
    for (int i = 0; i < line_count; i++) {
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
        else if (lines[i][0] == '1' || lines[i][0] == ' ') {
            temp_map = realloc(temp_map, (map_lines_count + 1) * sizeof(char *));
            temp_map[map_lines_count] = strdup(lines[i]);
            map_lines_count++;
        }
    }
    game->map = temp_map;
    game->map_height = map_lines_count;
    validate_map(game);
}

void close_window(void *param) {
    mlx_t *mlx = param;
    mlx_close_window(mlx);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Error: Usage: %s <map_file.cub>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char **lines = NULL;
    int line_count = 0;

    if (read_lines(argv[1], &lines, &line_count) == -1) {
        fprintf(stderr, "Error: Failed to load map %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    memset(&game, 0, sizeof(game));
    process_lines(lines, line_count, &game);

    for (int i = 0; i < line_count; i++)
        free(lines[i]);
    free(lines);

    mlx_t *mlx = mlx_init(WIDTH, HEIGHT, "cub3D", true);
    if (!mlx) {
        fprintf(stderr, "Error: %s\n", mlx_strerror(mlx_errno));
        return EXIT_FAILURE;
    }

    image = mlx_new_image(mlx, WIDTH, HEIGHT);
    if (!image) {
        mlx_terminate(mlx);
        fprintf(stderr, "Error: %s\n", mlx_strerror(mlx_errno));
        return EXIT_FAILURE;
    }

    if (mlx_image_to_window(mlx, image, 0, 0) == -1) {
        mlx_terminate(mlx);
        fprintf(stderr, "Error: %s\n", mlx_strerror(mlx_errno));
        return EXIT_FAILURE;
    }

    if (!game.texture_no || !game.texture_so || !game.texture_we || !game.texture_ea) {
        fprintf(stderr, "Error: Missing texture paths in map file.\n");
        mlx_terminate(mlx);
        return EXIT_FAILURE;
    }

    game.tex_no = mlx_load_png(game.texture_no);
    game.tex_so = mlx_load_png(game.texture_so);
    game.tex_we = mlx_load_png(game.texture_we);
    game.tex_ea = mlx_load_png(game.texture_ea);

    if (!game.tex_no || !game.tex_so || !game.tex_we || !game.tex_ea) {
        fprintf(stderr, "Error: Failed to load one or more textures.\n");
        mlx_terminate(mlx);
        return EXIT_FAILURE;
    }

    mlx_loop_hook(mlx, render_scene, mlx);
    mlx_loop_hook(mlx, move_camera, mlx);
    mlx_close_hook(mlx, close_window, mlx);
    mlx_loop(mlx);

    // Liberar texturas después de que termine el bucle principal
    mlx_delete_texture(game.tex_no);
    mlx_delete_texture(game.tex_so);
    mlx_delete_texture(game.tex_we);
    mlx_delete_texture(game.tex_ea);

    mlx_terminate(mlx);

    // Liberar memoria del mapa
    for (int i = 0; i < game.map_height; i++)
        free(game.map[i]);
    free(game.map);

    return EXIT_SUCCESS;
}
