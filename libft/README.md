# Libft

## Descripción

El proyecto "Libft" consiste en programar una librería en C con un conjunto de funciones de propósito general que serán de gran utilidad en tus futuros proyectos.

## Funcionalidades

- Reimplementación de funciones comunes de C.
- Funciones adicionales para manipulación de cadenas y memoria.
- Bonus: funciones para la manipulación de listas.

## Instrucciones Generales

- El proyecto escrito en C y seguir la Norma.
- Las funciones no terminan inesperadamente (segfault, bus error, etc.).
- Toda la memoria asignada en el heap se libera adecuadamente.
- Se crea Makefile que compila los archivos fuente con las flags -Wall, -Werror y -Wextra.
- El Makefile contiene las normas `$(NAME)`, `all`, `clean`, `fclean` y `re`.
- Los bonus estan en archivos distintos `_bonus.{c/h}` y se compilan con una regla `bonus` en el Makefile.

## Parte Obligatoria

### Consideraciones Técnicas

- No se declaran variables globales.
- Utiliza `static` para funciones complejas divididas en varias partes.
- Rodos los archivos estan en la raíz del repositorio.
- Todos los archivos `.c` se compilan con las flags -Wall -Werror -Wextra.
- Se usa `ar` para generar la librería, sin`libtool`.
- `libft.a` se crea en la raíz del repositorio.

### Parte 1 - Funciones de libc

Se erimplementan las siguientes funciones de la libc con los mismos prototipos y comportamientos:

- `ft_isalpha`
- `ft_isdigit`
- `ft_isalnum`
- `ft_isascii`
- `ft_isprint`
- `ft_strlen`
- `ft_memset`
- `ft_bzero`
- `ft_memcpy`
- `ft_memmove`
- `ft_strlcpy`
- `ft_strlcat`
- `ft_toupper`
- `ft_tolower`
- `ft_strchr`
- `ft_strrchr`
- `ft_strncmp`
- `ft_memchr`
- `ft_memcmp`
- `ft_strnstr`
- `ft_atoi`

Para las siguientes funciones, utiliza `malloc`:

- `ft_calloc`
- `ft_strdup`

### Parte 2 - Funciones Adicionales

Se desarrollan las siguientes funciones adicionales:

- `ft_substr`
- `ft_strjoin`
- `ft_strtrim`
- `ft_split`
- `ft_itoa`
- `ft_strmapi`
- `ft_striteri`
- `ft_putchar_fd`
- `ft_putstr_fd`
- `ft_putendl_fd`
- `ft_putnbr_fd`

## Parte Bonus

Se implementan funciones para manipular listas utilizando la siguiente estructura:

```c
typedef struct s_list
{
    void *content;
    struct s_list *next;
} t_list;
```
Las funciones a implementar son:

- `ft_lstnew`
- `ft_lstadd_front`
- `ft_lstsize`
- `ft_lstlast`
- `ft_lstadd_back`
- `ft_lstdelone`
- `ft_lstclear`
- `ft_lstiter`
- `ft_lstmap`

Se añade una regla make bonus en el Makefile para compilar estas funciones adicionales. 
