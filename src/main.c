/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbednar <sbednar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/07 16:09:10 by sbednar           #+#    #+#             */
/*   Updated: 2019/04/04 01:38:44 by sbednar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

// void	ui_loop22(t_env *e1, t_env *e2)
// {
// 	ui_create_frame(e1);
// 	ui_create_frame(e2);
// 	while (1)
// 	{
// 		if (ui_get_events(e1))
// 			ui_create_frame(e1);if (ui_get_events(e1))
// 			ui_create_frame(e1);
// 	}
// }

// void	f1(void *a)
// {
// 	(void)a;
// 	printf("1\n");
// }

// void	f3(void *a)
// {(void)a;
// 	printf("3\n");
// }

// void	f2(void *a)
// {(void)a;
// 	printf("2\n");
// }

static void	test_for_notmain(void *a1, void *a2)
{
	Uint32		windowID;
	t_ui_main	*m;

	m = (t_ui_main *)a1;
	windowID = *((Uint32 *)a2);
	ui_main_remove_window_by_id(m, windowID);
}

static void	test_for_main(void *a1, void *a2)
{
	(void)a1;
	(void)a2;
	ui_sdl_deinit(EXIT_SUCCESS);
}

void	log_setup(t_ui_win *w)
{
	ui_event_add_listener(&(w->events.onMouseMoved), &ui_log_mouse_motion);
	ui_event_add_listener(&(w->events.onMouseButtonDown), &ui_log_mouse_button_down);
	ui_event_add_listener(&(w->events.onMouseButtonUp), &ui_log_mouse_button_up);
	ui_event_add_listener(&(w->events.onFocusGained), &ui_log_window_focus_gained);
	ui_event_add_listener(&(w->events.onFocusLost), &ui_log_window_focus_lost);
	ui_event_add_listener(&(w->events.onResize), &ui_log_window_resized);
	ui_event_add_listener(&(w->events.onClose), &ui_log_window_closed);
}

int		main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	// t_env	e1;
	// t_env	e2;

	ui_sdl_init();

	t_ui_main m;
	ui_main_init(&m);

	t_ui_win w;
	ui_win_init(&w);
	w.title = ft_strdup("TEST1");
	w.canvas.rect.w = 640;
	w.canvas.rect.h = 480;
	w.params = WIN_MAIN | WIN_RESIZABLE;
	log_setup(&w);
	ui_event_add_listener(&(w.events.onClose), &test_for_main);
	// w.properties = WIN_RESIZABLE;
	ui_win_create(&w);

	t_ui_win w1;
	ui_win_init(&w1);
	w1.title = ft_strdup("TEST2");
	w1.canvas.rect.w = 200;
	w1.canvas.rect.h = 100;
	w1.params = WIN_RESIZABLE;
	log_setup(&w1);
	ui_event_add_listener(&(w1.events.onClose), &test_for_notmain);
	// w1.properties = WIN_RESIZABLE;
	ui_win_create(&w1);

	t_ui_win w2;
	ui_win_init(&w2);
	w2.title = ft_strdup("TEST3");
	w2.canvas.rect.w = 200;
	w2.canvas.rect.h = 100;
	w2.params = WIN_RESIZABLE;
	log_setup(&w2);
	ui_event_add_listener(&(w2.events.onClose), &test_for_notmain);
	// w1.properties = WIN_RESIZABLE;
	ui_win_create(&w2);

	ui_main_add_window(&m, &w);
	ui_main_add_window(&m, &w1);
	ui_main_add_window(&m, &w2);

	SDL_SetRenderDrawColor(w.sdl_renderer, 255, 0, 0, 255);
	SDL_RenderClear(w.sdl_renderer);
	SDL_RenderPresent(w.sdl_renderer);

	SDL_SetRenderDrawColor(w1.sdl_renderer, 0, 255, 0, 255);
	SDL_RenderClear(w1.sdl_renderer);
	SDL_RenderPresent(w1.sdl_renderer);

	SDL_SetRenderDrawColor(w2.sdl_renderer, 0, 0, 255, 255);
	SDL_RenderClear(w2.sdl_renderer);
	SDL_RenderPresent(w2.sdl_renderer);
	// SDL_Surface *helloWorld = IMG_Load("test2.jpg");
	// SDL_Rect r1;
	// r1.w = 600;
	// r1.h = 200;
	// r1.x = 0;
	// r1.y = 0;
	// SDL_BlitScaled(helloWorld, NULL, w.sdl_surface, &r1);
	// SDL_UpdateWindowSurface(w.sdl_window);
	// SDL_Delay( 6000 );
	// ui_win_close(&w);
	ui_main_loop(&m);
	exit(0);
	// SDL_Delay( 2000 );
	// ui_deinit_enviroment();
    // SDL_Thread *thread1;
	// SDL_Thread *thread2;
    // int         threadReturnValue1;
    // int         threadReturnValue2;
	// int			i_events1;
	// int			i_events2;

	// ft_putchar('a');
	// ui_create_env(&e1, WIN_W, WIN_H);
	// ui_create_win(&e1, "kek1");

	// ui_create_env(&e2, 400, 300);
	// ui_create_win(&e2, "kek2");

	// e1.event = &events;
	// e1.frame = &test;

	// e2.event = &events;
	// e2.frame = &test2;

	// SDL_SetWindowResizable(e1.sdl.win, 1);
	// SDL_SetWindowResizable(e2.sdl.win, 1);
	// while (1)
	// {
	// 	i_events1 = ui_get_events(&e1);
	// 	i_events2 = ui_get_events(&e2);
	// 	e1.events = i_events1;
	// 	e2.events = i_events2;

	// 	thread1 = SDL_CreateThread(ui_perframe, "TestThread1", (void *)&e1);
	// 	thread2 = SDL_CreateThread(ui_perframe, "TestThread2", (void *)&e2);

	// 	SDL_WaitThread(thread1, &threadReturnValue1);
	// 	SDL_WaitThread(thread2, &threadReturnValue2);
	// }
	// exit(EXIT_SUCCESS);

	// t_ui_event ev;
	// ui_event_init(&ev);
	// ui_event_add_listener(&ev, &f1);
	// ui_event_add_listener(&ev, &f3);
	// ui_event_add_listener(&ev, &f2);
	// ui_event_add_listener(&ev, &f3);
	// ui_event_add_listener(&ev, &f2);
	// ui_event_add_listener(&ev, &f3);
	// ui_event_add_listener(&ev, &f2);
	// ui_event_add_listener(&ev, &f3);
	// ui_event_add_listener(&ev, &f2);
	// ui_event_add_listener(&ev, &f3);
	// ui_event_add_listener(&ev, &f2);
	// ui_event_add_listener(&ev, &f3);
	// ui_event_add_listener(&ev, &f2);
	// ui_event_invoke(&ev, NULL);

	return (0);
}
