/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 20:51:23 by victor            #+#    #+#             */
/*   Updated: 2025/04/17 12:23:22 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

void update_door_animation(t_app *app, double dt) {
    for (int i = 0; i < app->game.door_count; i++) {
        t_door *door = &app->game.doors[i];
        if (door->move_progress < DOOR_ANIM_DURATION) {
            door->move_progress += dt;
            if (door->move_progress > DOOR_ANIM_DURATION)
                door->move_progress = DOOR_ANIM_DURATION;

            double t = door->move_progress / DOOR_ANIM_DURATION;  // lineal 0→1

            if (door->is_open)
                door->open_offset = t;
            else
                door->open_offset = 1.0 - t;
        }
    }
}

// Inicia o invierte la animación al pulsar Espacio
void toggle_doors(t_app *app) {
    static int last_space = 0;
    int current_space = mlx_is_key_down(app->mlx, MLX_KEY_SPACE);

    if (current_space && !last_space) {
        t_ray ray;
        init_ray(app, WIDTH/2, &ray);
        do_dda(app, &ray);

        if (ray.perpwalldist < 1.5 && (ray.hit_tile == '2' || ray.hit_tile == '3')) {
            for (int i = 0; i < app->game.door_count; i++) {
                t_door *door = &app->game.doors[i];
                if (door->x == ray.map_x && door->y == ray.map_y) {
                    door->is_open = 1;
                    door->move_progress = 0.0;  // reset
                    break;
                }
            }
        }
    }
    last_space = current_space;
}

