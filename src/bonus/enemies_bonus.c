/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 14:53:46 by victor            #+#    #+#             */
/*   Updated: 2025/04/18 10:51:29 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D_bonus.h"

void update_enemy_direction(t_app *app, t_enemy *e, double delta_time)
{
    double target_angle = atan2(app->camera.pos.y - e->pos_y,
                                app->camera.pos.x - e->pos_x);

	if (!e->initialized) {
        e->facing_angle = target_angle;
        e->initialized = true;
    }

    double angle_diff = target_angle - e->facing_angle;

    angle_diff = fmod(angle_diff + M_PI, 2 * M_PI) - M_PI;

    double max_rotation_speed = 1.0 * delta_time;
	if (angle_diff > max_rotation_speed)
        angle_diff = max_rotation_speed;
    else if (angle_diff < -max_rotation_speed)
        angle_diff = -max_rotation_speed;

    e->facing_angle += angle_diff;

    double player_angle = atan2(app->camera.pos.y - e->pos_y,
                                app->camera.pos.x - e->pos_x);
    double relative_angle = player_angle - e->facing_angle;

    relative_angle = fmod(relative_angle + M_PI, 2 * M_PI) - M_PI;

    if (fabs(relative_angle) < M_PI_4)
        e->dir = FRONT;   
    else if (relative_angle > M_PI_4 && relative_angle < 3*M_PI_4)
        e->dir = LEFT;
    else if (relative_angle < -M_PI_4 && relative_angle > -3*M_PI_4)
        e->dir = RIGHT;
    else
        e->dir = BACK;     
}

void update_enemy_animation(t_enemy *e, double delta_time)
{
    e->time_since_last_move += delta_time;

    if (e->time_since_last_move > 0.2) {
        e->anim_frame = (e->anim_frame + 1) % 4;
        e->time_since_last_move = 0.0;
    }
}

void update_enemies(t_app *app, double delta_time)
{
    t_camera *cam = &app->camera;
    double cam_x = cam->pos.x;
    double cam_y = cam->pos.y;

    for (int i = 0; i < app->game.enemy_count; i++) {
        t_enemy *e = &app->game.enemies[i];
        if (!e->is_active)
            continue;

        update_enemy_direction(app, e, delta_time);
        update_enemy_animation(e, delta_time);

        double dx = cam_x - e->pos_x;
        double dy = cam_y - e->pos_y;
        double distance = sqrt(dx*dx + dy*dy);

        if (distance > 1.0 && distance < 10.0) {
            double move_x = (dx / distance) * e->speed * delta_time;
            double move_y = (dy / distance) * e->speed * delta_time;

            if (!collides(&app->game, e->pos_x + move_x, e->pos_y + move_y)) {
                e->pos_x += move_x;
                e->pos_y += move_y;
            }
        }

        double ax = e->pos_x - cam_x;
        double ay = e->pos_y - cam_y;
        double dist_sq = ax*ax + ay*ay;
        double min_dist = e->radius + COLLISION_RADIUS;

        if (dist_sq < (min_dist * min_dist) && dist_sq > 1e-8) {
            double dist = sqrt(dist_sq);
            double overlap = (min_dist - dist) + 0.001;
            double orig_ex = e->pos_x;
            double orig_ey = e->pos_y;

            double push_x = (ax / dist) * overlap;
            double push_y = (ay / dist) * overlap;
            e->pos_x += push_x;
            e->pos_y += push_y;

            if (collides(&app->game, e->pos_x, e->pos_y)) {
                e->pos_x = orig_ex;
                e->pos_y = orig_ey;

                cam->pos.x -= push_x;
                cam->pos.y -= push_y;
            }
        }
    }
}

void render_enemy(t_app *app, t_enemy *e)
{
    double rel_x = e->pos_x - app->camera.pos.x;
    double rel_y = e->pos_y - app->camera.pos.y;
    
    double inv_det = 1.0 / (app->camera.plane.x * app->camera.dir.y
			- app->camera.dir.x * app->camera.plane.y);
    double transform_x = inv_det * (app->camera.dir.y * rel_x - app->camera.dir.x * rel_y);
    double transform_y = inv_det * (-app->camera.plane.y * rel_x + app->camera.plane.x * rel_y);

    if (transform_y <= 0) return;

    int screen_x = (int)((WIDTH / 2) * (1 + transform_x / transform_y));
    int sprite_height = abs((int)(HEIGHT / transform_y));
    int draw_start_y = -sprite_height / 2 + HEIGHT / 2 - app->camera.view_z;
    int draw_end_y = sprite_height / 2 + HEIGHT / 2 - app->camera.view_z;
    int sprite_width = sprite_height;
    int draw_start_x = -sprite_width / 2 + screen_x;
    int draw_end_x = sprite_width / 2 + screen_x;

    const int tex_width = app->game.tex_enemy->width;
    const int tex_height = app->game.tex_enemy->height;
    const int frame_width = tex_width / 4;
    const int frame_height = tex_height / 4;

    const int tex_offset_x = e->anim_frame * frame_width;
    const int tex_offset_y = e->dir * frame_height;

    for (int x = draw_start_x; x < draw_end_x; x++)
    {
        if (x < 0 || x >= WIDTH || transform_y > app->z_buffer[x]) continue;

        for (int y = draw_start_y; y < draw_end_y; y++)
        {
            if (y < 0 || y >= HEIGHT) continue;

            int tx = (int)((float)(x - draw_start_x) / (draw_end_x - draw_start_x) * frame_width);
            int ty = (int)((float)(y - draw_start_y) / (draw_end_y - draw_start_y) * frame_height);

            if (tx >= 0 && tx < frame_width && ty >= 0 && ty < frame_height)
            {
                uint32_t color = ((uint32_t*)app->game.tex_enemy->pixels)
                    [(tex_offset_y + ty) * tex_width + (tex_offset_x + tx)];
                
                if ((color & 0xFF000000) != 0)
                    mlx_put_pixel(app->image, x, y, convert_pixel(color));
            }
        }
    }
}
