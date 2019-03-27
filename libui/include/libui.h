/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libui.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbednar <sbednar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/10 19:09:04 by sbednar           #+#    #+#             */
/*   Updated: 2019/03/27 18:45:47 by sbednar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBUI_H
# define LIBUI_H

# include <SDL2/SDL.h>
# include <SDL2/SDL_ttf.h>
# include <SDL2/SDL_image.h>
# include "libft.h"

# define WIN_W		640
# define WIN_H		480

# define WIN_IS_RESIZABLE	(1 << 0)

/*
** Smart things:
** 1) It's possible to replace t_name_init just by ft_bzero in code;
** ...To be continued...
*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"

#pragma region		t_vec2

typedef struct		s_vec2
{
	int				x;
	int				y;
}					t_vec2;

#pragma endregion
#pragma region		t_fvec2

typedef struct		s_fvec2
{
	float			x;
	float			y;
}					t_fvec2;

#pragma endregion
#pragma region		s_ui_ar

struct				s_ui_ar
{
	t_vec2			abs;
	t_fvec2			rel;
};

typedef struct		s_ui_ar t_ui_size;
typedef struct		s_ui_ar t_ui_pos;

#pragma endregion
#pragma region		t_ui_event

typedef struct		s_ui_event
{
	t_list			*events;
}					t_ui_event;

void				ui_event_init(t_ui_event *e);
int					ui_event_add(t_ui_event *e, void(*f)(const void *i));
void				ui_event_invoke(t_ui_event *e);
void				ui_event_free(t_ui_event *e);

#pragma endregion
#pragma region		t_ui_el_events

typedef struct		s_ui_el_events
{
	t_ui_event		onPointerEnter;
	t_ui_event		onPointerStay;
	t_ui_event		onPointerExit;
	t_ui_event		onPointerLeftButtonPressed;
	t_ui_event		onPointerLeftButtonHold;
	t_ui_event		onPointerLeftButtonReleased;
	t_ui_event		onPointerRightButtonPressed;
	t_ui_event		onPointerRightButtonHold;
	t_ui_event		onPointerRightButtonReleased;
	t_ui_event		onScrollUp;
	t_ui_event		onScrollDown;
}					t_ui_el_events;

#pragma endregion
#pragma region		t_ui_el

typedef struct		s_ui_el
{
	struct t_ui_el	*parent;
	t_list			*children;
	t_ui_size		size;
	t_ui_pos		pos;
	t_ui_el_events	events;
	// TODO: t_ui_graphics
}					t_ui_el;

#pragma endregion
#pragma region		t_ui_win_events

typedef struct		s_ui_win_events
{
	t_ui_event		onSwitchOn;
	t_ui_event		onSwitchOff;
	t_ui_event		onResize;
	t_ui_event		onClose;
}					t_ui_win_events;

#pragma endregion
#pragma region		t_ui_win

typedef struct		s_ui_win
{
	t_ui_size		size;
	t_ui_el			*root;
	t_ui_win_events	events;
	int				properties;
}					t_ui_win;

#pragma endregion
#pragma region		t_ui_main

typedef struct		s_ui_main
{
	t_list			*windows;
}					t_ui_main;

#pragma endregion

#pragma GCC diagnostic pop

typedef struct		s_sdl
{
	SDL_Event		evt;
	SDL_Window		*win;
	SDL_Renderer	*ren;
}					t_sdl;

typedef struct		s_env
{
	int				w;
	int				h;
	int				m_pos[2];
	t_sdl			sdl;
	SDL_Surface		*sur;
	SDL_Texture		*tex;
	void 			(*frame)(struct s_env *e);
	int 			(*event)(struct s_env *e);
	const Uint8		*state;
	int				events;
}					t_env;

void				ui_create_env(t_env *e, const int w, const int h);
void				ui_create_win(t_env *e, const char *name);
SDL_Surface			*ui_create_sur(const int w, const int h);
void				ui_create_frame(t_env *e);

int					ui_get_events(t_env *e);

void				ui_set_pixel(t_env *e, const int x, const int y,
const Uint32 c);

int					ui_loop(void *ev);

void				ui_exit_with_msg(t_env *e, const char *msg);
void				ui_exit(t_env *e);

#endif
