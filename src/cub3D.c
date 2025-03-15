/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3D.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 19:46:33 by victor            #+#    #+#             */
/*   Updated: 2025/03/15 13:46:54 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"
#include <math.h>

#define WIDTH 512
#define HEIGHT 512

static mlx_image_t* image;

typedef struct s_camera
{
    t_vec2 pos;       // Posición en el mapa
    t_vec2 dir;       // Dirección de la cámara
    t_vec2 plane;     // Plano de la cámara (define FOV)
    double move_speed;
    double rot_speed;
} t_camera;

t_camera camera = {
    .pos = {2.0, 2.0},
    .dir = {1.0, 0.0},
    .plane = {0.0, 0.66},
    .move_speed = 0.05,
    .rot_speed = 0.05
};

// Mapa simple (1 = pared, 0 = espacio libre)
int world_map[10][10] = {
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,0,1,0,1},
    {1,0,1,0,0,0,0,1,0,1},
    {1,0,1,0,0,0,0,1,0,1},
    {1,0,0,0,0,1,1,1,0,1},
    {1,0,1,1,0,1,0,0,0,1},
    {1,0,1,0,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1}
};

// Función para calcular el color de un píxel
int32_t ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a)
{
    return (r << 24 | g << 16 | b << 8 | a);
}

void ft_draw_background()
{
    for (uint32_t i = 0; i < image->width; ++i)
    {
        for (uint32_t y = 0; y < image->height; ++y)
        {
            mlx_put_pixel(image, i, y, ft_pixel(255, 255, 0, 255)); // Fondo rojo
        }
    }
}

// Algoritmo de Raycasting para dibujar el mundo
void render_scene(void *param)
{
    (void)param;
    ft_draw_background();
    for (int x = 0; x < WIDTH; x++)
    {
        double cameraX = 2 * x / (double)WIDTH - 1;
        t_vec2 rayDir = {camera.dir.x + camera.plane.x * cameraX, camera.dir.y + camera.plane.y * cameraX};

        t_vec2 map = { (int)camera.pos.x, (int)camera.pos.y };
        t_vec2 deltaDist = {
            fabs(1 / rayDir.x),
            fabs(1 / rayDir.y)
        };

        t_vec2 sideDist;
        t_vec2 step;
        int hit = 0;
        int side;

        if (rayDir.x < 0)
        {
            step.x = -1;
            sideDist.x = (camera.pos.x - map.x) * deltaDist.x;
        }
        else
        {
            step.x = 1;
            sideDist.x = (map.x + 1.0 - camera.pos.x) * deltaDist.x;
        }
        if (rayDir.y < 0)
        {
            step.y = -1;
            sideDist.y = (camera.pos.y - map.y) * deltaDist.y;
        }
        else
        {
            step.y = 1;
            sideDist.y = (map.y + 1.0 - camera.pos.y) * deltaDist.y;
        }

        while (!hit)
        {
            if (sideDist.x < sideDist.y)
            {
                sideDist.x += deltaDist.x;
                map.x += step.x;
                side = 0;
            }
            else
            {
                sideDist.y += deltaDist.y;
                map.y += step.y;
                side = 1;
            }
            if (world_map[(int)map.x][(int)map.y] > 0)
                hit = 1;
        }

        double perpWallDist = (side == 0) ? (sideDist.x - deltaDist.x) : (sideDist.y - deltaDist.y);
        int lineHeight = (int)(HEIGHT / perpWallDist);

        int drawStart = -lineHeight / 2 + HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + HEIGHT / 2;
        if (drawEnd >= HEIGHT) drawEnd = HEIGHT - 1;

        int32_t color = (side == 1) ? ft_pixel(255, 0, 0, 255) : ft_pixel(200, 0, 0, 255);

        for (int y = drawStart; y < drawEnd; y++)
            mlx_put_pixel(image, x, y, color);
    }
}

// Movimiento de la cámara
void move_camera(void *param)
{
    mlx_t *mlx = param;

    if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE))
        mlx_close_window(mlx);

    if (mlx_is_key_down(mlx, MLX_KEY_W))
    {
        if (!world_map[(int)(camera.pos.x + camera.dir.x * camera.move_speed)][(int)camera.pos.y])
            camera.pos.x += camera.dir.x * camera.move_speed;
        if (!world_map[(int)camera.pos.x][(int)(camera.pos.y + camera.dir.y * camera.move_speed)])
            camera.pos.y += camera.dir.y * camera.move_speed;
    }
    if (mlx_is_key_down(mlx, MLX_KEY_S))
    {
        if (!world_map[(int)(camera.pos.x - camera.dir.x * camera.move_speed)][(int)camera.pos.y])
            camera.pos.x -= camera.dir.x * camera.move_speed;
        if (!world_map[(int)camera.pos.x][(int)(camera.pos.y - camera.dir.y * camera.move_speed)])
            camera.pos.y -= camera.dir.y * camera.move_speed;
    }
    if (mlx_is_key_down(mlx, MLX_KEY_A) || mlx_is_key_down(mlx, MLX_KEY_D))
    {
        double rotDir = (mlx_is_key_down(mlx, MLX_KEY_A)) ? 1 : -1;
        double oldDirX = camera.dir.x;
        camera.dir.x = camera.dir.x * cos(rotDir * camera.rot_speed) - camera.dir.y * sin(rotDir * camera.rot_speed);
        camera.dir.y = oldDirX * sin(rotDir * camera.rot_speed) + camera.dir.y * cos(rotDir * camera.rot_speed);

        double oldPlaneX = camera.plane.x;
        camera.plane.x = camera.plane.x * cos(rotDir * camera.rot_speed) - camera.plane.y * sin(rotDir * camera.rot_speed);
        camera.plane.y = oldPlaneX * sin(rotDir * camera.rot_speed) + camera.plane.y * cos(rotDir * camera.rot_speed);
    }
}


// Main
int32_t main(void)
{
    mlx_t* mlx;

    if (!(mlx = mlx_init(WIDTH, HEIGHT, "cub3D", true)))
    {
        puts(mlx_strerror(mlx_errno));
        return EXIT_FAILURE;
    }

    if (!(image = mlx_new_image(mlx, WIDTH, HEIGHT)))
    {
        mlx_close_window(mlx);
        puts(mlx_strerror(mlx_errno));
        return EXIT_FAILURE;
    }

    if (mlx_image_to_window(mlx, image, 0, 0) == -1)
    {
        mlx_close_window(mlx);
        puts(mlx_strerror(mlx_errno));
        return EXIT_FAILURE;
    }

    mlx_loop_hook(mlx, render_scene, mlx);
    mlx_loop_hook(mlx, move_camera, mlx);

    mlx_loop(mlx);
    mlx_terminate(mlx);
    return EXIT_SUCCESS;
}

