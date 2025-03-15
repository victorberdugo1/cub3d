# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: victor <marvin@42.fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/04 19:37:02 by victor            #+#    #+#              #
#    Updated: 2025/03/15 12:59:36 by vberdugo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = cub3D
NAME_BONUS = cub3D_bonus

LIBFT = libft/libft.a
MINI = MLX42/build/libmlx42.a
MINI_BACKUP = $(MINI).backup

OBJ_DIR = ./obj
SRC_DIR = ./src

# Lista de archivos fuente
SRC = $(SRC_DIR)/cub3D.c 

OBJ = $(SRC:src/%.c=$(OBJ_DIR)/%.o)
# Lista de archivos objeto
# OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

INCLUDE = iinc/cube3D.hnc/cub3D.h #libft/libft.h

BONUS_SRC = $(SRC_DIR)/bonus/cub3D_bonus.c
BONUS_OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(BONUS_SRC))

CC = gcc
CFLAGS = -Wall -Wextra -Werror -Ilibft -Iinc  #-g -fsanitize=address

LDFLAGS = $(LIBFT) $(MINI) -lglfw -lm

HBONUS = .bonus

all: prepare_libft prepare_mlx libs mlx $(NAME)

xGREEN_DARK = \033[0;32m	# Verde oscuro
WHITE = \033[0m				# Blanco
RED = \033[0;91m			# Rojo
RESET = \033[0m				# Reiniciar color

# Regla para clonar y preparar libft
prepare_libft:
	@if [ ! -d "libft/.git" ]; then \
		echo "$(WHITE)Cloning libft...$(RESET)"; \
		git clone git@github.com:victorberdugo1/libft.git libft; \
	fi

# Regla para clonar y preparar MLX42
prepare_mlx:
	@if [ ! -d "MLX42/.git" ]; then \
		echo "$(WHITE)Cloning MLX42...$(RESET)"; \
		git clone https://github.com/codam-coding-college/MLX42.git MLX42; \
	fi

libs: prepare_libft
	@make -C libft

mlx: prepare_mlx
	@cmake MLX42 -B MLX42/build && make -C MLX42/build -j4

$(NAME): $(LIBFT) $(MINI) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)
	@echo "$(GREEN_DARK)Executable $(NAME) created successfully!$(RESET)"

$(NAME_BONUS): $(LIBFT) $(MINI) $(BONUS_OBJ)
	$(CC) $(CFLAGS) $(BONUS_OBJ) -o $(NAME_BONUS) $(LDFLAGS)

$(LIBFT):
	@make -C libft

$(MINI):
	@cmake MLX42 -B MLX42/build && make -C MLX42/build -j4

# Regla para compilar archivos objeto
$(OBJ_DIR)/%.o: src/%.c $(INCLUDE) Makefile
	@mkdir -p $(dir $@)  # Crea el directorio de destino si no existe
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(GREEN_DARK)Compiled: $<$(RESET)"

bonus: libs mlx $(NAME_BONUS)
	@if [ ! -f $(HBONUS) ] || \
		find $(BONUS_OBJ) -newer $(HBONUS) | grep -q .; then \
		$(MAKE) $(NAME_BONUS); \
		touch $(HBONUS); \
	else \
		echo "make bonus: Nothing to be done for 'bonus'."; \
	fi

clean:
	@make -C libft clean
	@make -C MLX42/build clean
	rm -rf $(OBJ_DIR) $(BONUS_OBJ)
	@echo "$(RED)Cleaned up object files.$(RESET)"

fclean: clean
	@make -C libft fclean
	rm -f $(MINI)
	rm -f $(NAME) $(NAME_BONUS)
	@echo "$(RED)Executable $(NAME) removed.$(RESET)"

re: fclean all

.PHONY: all bonus clean fclean re		
