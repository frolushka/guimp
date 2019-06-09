/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbednar <sbednar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/07 16:09:10 by sbednar           #+#    #+#             */
/*   Updated: 2019/06/10 00:23:00 by sbednar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

static void move_windows(void *a1, void *a2)
{
	t_ui_main   *m;
	t_ui_win    *w;
	Uint32      windowID;
	t_list      *list;
	t_ui_win    *cur_w;
	t_vec2      pos;

	m = (t_ui_main *)a1;
	windowID = *((Uint32 *)a2);
	w = ui_main_find_window_by_id(m, windowID);
	if (w != NULL)
	{
		list = m->windows;
		while (list)
		{
			cur_w = (t_ui_win *)list->content;
			if (cur_w->sdl_windowID == windowID)
			{
				list = list->next;
				continue;
			}
			SDL_GetWindowPosition(w->sdl_window, &pos.x, &pos.y);
			if (cur_w->params & WIN_MAIN)
				pos.x = pos.x + GM_TOOL_WIN_W + 5;
			else
				pos.x = pos.x - GM_TOOL_WIN_W - 5;
			printf("id: %d, (%d, %d)\n", windowID, pos.x, pos.y);
			SDL_SetWindowPosition(cur_w->sdl_window, pos.x, pos.y);
			list = list->next;
		}
	}
}

static void	testOnPtrEnter(void *main, void *el_v)
{
	main = NULL;
	t_ui_el *el = (t_ui_el *)el_v;
	if (el->current_texture != (size_t)ft_strhash("onActive"))
		ui_el_set_current_texture_by_id(el, "onFocus");
}

static void	testOnPtrExit(void *main, void *el_v)
{
	main = NULL;
	t_ui_el *el = (t_ui_el *)el_v;
	if (el->current_texture != (size_t)ft_strhash("onActive"))
		ui_el_set_current_texture_by_id(el, "default");
}

static void	testOnPtrLBD(void *main, void *el_v)
{
	t_list	*layer_elems;
	t_guimp	*g;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	t_ui_el *el = (t_ui_el *)el_v;
	layer_elems = el->parent->children;
	if (el->current_texture != (size_t)ft_strhash("onActive"))
	{
		while (layer_elems)
		{
			if (((t_ui_el *)layer_elems->content)->current_texture == (size_t)ft_strhash("onActive"))
				ui_el_set_current_texture_by_id(((t_ui_el *)layer_elems->content), "default");
			layer_elems = layer_elems->next;
		}
		ui_el_set_current_texture_by_id(el, "onActive");
		g->layers.current_layer = (t_ui_el *)(el->children->content);
	}
}

static void	test_add_layer(void *ui_main, void *el_v)
{
	t_ui_main	*m;
	t_ui_el		*el;
	t_ui_el		*layer_menu;
	t_ui_el		*tmp_el;
	t_guimp		*g;

	el = (t_ui_el *)el_v;
	m = (t_ui_main *)ui_main;
	g = (t_guimp *)m->data;
	layer_menu = ui_win_find_el_by_id(g->main_win, GM_LAYER_ID_MENU);
	if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
	{
		printf("layer malloc error in scrollable menu in layer_win\n");
		return ;
	}
	ui_el_setup_default_scroll_menu_elem(tmp_el, layer_menu);
	tmp_el->id = gm_generate_surf_id(ID_GENERATOR_ADD);
	ui_el_set_pos(tmp_el, 0, 0,
		(t_fvec2){0.1,
			((t_ui_el *)layer_menu->children->content)->relative_rect.y + 0.3 * (float)gm_generator_get_surf_count()});
	ui_el_set_size(tmp_el, 0, 0, (t_fvec2){0.8, 0.25});
	tmp_el->sdl_renderer = g->main_win->sdl_renderer;
	ui_el_add_texture_from_main_by_id(g->ui_main, tmp_el, "layer_place", "default");
	ui_el_add_texture_from_main_by_id(g->ui_main, tmp_el, "layer_onPtr", "onFocus");
	ui_el_add_texture_from_main_by_id(g->ui_main, tmp_el, "layer_active", "onActive");
	ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &testOnPtrLBD);
	ui_event_add_listener(&(tmp_el->events.onPointerEnter), &testOnPtrEnter);
	ui_event_add_listener(&(tmp_el->events.onPointerExit), &testOnPtrExit);

	if (!(el = (t_ui_el *)malloc(sizeof(t_ui_el))))
	{
		printf("layer texture malloc error in scrollable menu in layer_win\n");
		return ;
	}
	ui_el_setup_default(el);
	ui_el_add_child(tmp_el, el);
	ui_el_set_pos(el, 0, 0, (t_fvec2){0.1, 0.1});
	ui_el_set_size(el, 0, 0, (t_fvec2){0.8, 0.8});
	el->id = tmp_el->id * 1000;
	el->params |= EL_IGNOR_RAYCAST | EL_IS_DEPENDENT;
	el->sdl_renderer = g->main_win->sdl_renderer;
	ui_el_add_empty_texture(el, GM_IMAGE_SIZE_X, GM_IMAGE_SIZE_Y, "default");

	t_list	*tmp;
	tmp = ft_lstnew(NULL, 0);
	tmp->content = el->sdl_textures->content;
	tmp->content_size = tmp_el->id;
	ft_lstadd_back(&(g->layers.layers), tmp);
}

static void	test_del_layer(void *main, void *el_v)
{
	t_guimp	*g;
	t_ui_el	*el;
	t_ui_el	*next_active;
	t_list	*tmp;
	t_list	*prev;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	el = g->layers.current_layer->parent;
	(void)el_v;
	if (gm_generator_get_surf_count() == 0 || el->id == GM_LAYER_ID_DEF_LAYER)
		return ;
	if (!(next_active = ui_win_find_el_by_id(g->main_win, el->id + 1)))
		next_active = ui_win_find_el_by_id(g->main_win, el->id - 1);
	ui_el_set_current_texture_by_id(next_active, "onActive");
	g->layers.current_layer = (t_ui_el *)(next_active->children->content);
	tmp = el->parent->children;
	prev = tmp;
	while (tmp)
	{
		next_active = (t_ui_el *)(tmp->content);
		if (next_active->id == el->id)
			prev->next = tmp->next;
		if (next_active->id > el->id)
		{
			next_active->id--;
			ui_el_change_pos(next_active, 0, 0, (t_fvec2){0, -(next_active->relative_rect.h + 0.05)});
		}
		prev = tmp;
		tmp = tmp->next;
	}

	tmp = g->layers.layers;
	prev = tmp;
	while (tmp)
	{
		if ((Uint32)(tmp->content_size) == el->id)
			prev->next = tmp->next;
		if ((Uint32)(tmp->content_size) > el->id)
			tmp->content_size--;
		prev = tmp;
		tmp = tmp->next;
	}

	//del el
	//del texture
	gm_generate_surf_id(ID_GENERATOR_DEL);
}

static void	draw_canvas_renderer(void *el_v, void *main)
{
	t_guimp	*g;
	t_ui_el	*el;
	t_list	*layer;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	el = (t_ui_el *)el_v;
	layer = g->layers.layers;
	while (layer)
	{
		SDL_RenderCopy(el->sdl_renderer, (SDL_Texture *)(layer->content), &g->zoom_rect, &el->rect);
		layer = layer->next;
	}
}

static void	start_draw_with_selected_tool(void *main, void *el_v)
{
	t_guimp	*g;
	t_ui_el	*el;
	int		x;
	int		y;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	el = (t_ui_el *)el_v;
	x = ((float)el->ptr_rel_pos.x / (float)el->rect.w) * g->zoom_rect.w + g->zoom_rect.x;
	y = ((float)el->ptr_rel_pos.y / (float)el->rect.h) * g->zoom_rect.h + g->zoom_rect.y;
	g->draw_tool.prew_point = (t_vec2){x, y};
	if (g->draw_tool.tool == GM_TOOL_ZOOM && g->draw_tool.zoom < GM_ZOOM_MAX_SIZE)
	{
		g->draw_tool.zoom++;
		g->zoom_rect.w = GM_IMAGE_SIZE_X / g->draw_tool.zoom;
		g->zoom_rect.h = GM_IMAGE_SIZE_Y / g->draw_tool.zoom;
		x = x - g->zoom_rect.w / 2;
		y = y - g->zoom_rect.h / 2;
		x = (x + g->zoom_rect.w > GM_IMAGE_SIZE_X) ? GM_IMAGE_SIZE_X - g->zoom_rect.w : x;
		y = (y + g->zoom_rect.h > GM_IMAGE_SIZE_Y) ? GM_IMAGE_SIZE_Y - g->zoom_rect.h : y;
		g->zoom_rect.x = x < 0 ? 0 : x;
		g->zoom_rect.y = y < 0 ? 0 : y;
	}
}

static void	start_alt_with_selected_tool(void *main, void *el_v)
{
	t_guimp	*g;
	t_ui_el	*el;
	int		x;
	int		y;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	el = (t_ui_el *)el_v;
	x = ((float)el->ptr_rel_pos.x / (float)el->rect.w) * g->zoom_rect.w + g->zoom_rect.x;
	y = ((float)el->ptr_rel_pos.y / (float)el->rect.h) * g->zoom_rect.h + g->zoom_rect.y;
	g->draw_tool.prew_point = (t_vec2){x, y};
	if (g->draw_tool.tool == GM_TOOL_ZOOM && g->draw_tool.zoom > 1)
	{
		g->draw_tool.zoom--;
		g->zoom_rect.w = GM_IMAGE_SIZE_X / g->draw_tool.zoom;
		g->zoom_rect.h = GM_IMAGE_SIZE_Y / g->draw_tool.zoom;
		x = x - g->zoom_rect.w / 2;
		y = y - g->zoom_rect.h / 2;
		x = (x + g->zoom_rect.w > GM_IMAGE_SIZE_X) ? GM_IMAGE_SIZE_X - g->zoom_rect.w : x;
		y = (y + g->zoom_rect.h > GM_IMAGE_SIZE_Y) ? GM_IMAGE_SIZE_Y - g->zoom_rect.h : y;
		g->zoom_rect.x = x < 0 || g->draw_tool.zoom == 1 ? 0 : x;
		g->zoom_rect.y = y < 0 || g->draw_tool.zoom == 1 ? 0 : y;
	}
}

static void	start_zoom_in(void *m, void *wid)
{
	int			pt;
	t_guimp		*g;
	t_ui_win	*w;
	Uint32		windowID;

	g = (t_guimp *)(((t_ui_main *)m)->data);
	windowID = *((Uint32 *)wid);
	w = ui_main_find_window_by_id((t_ui_main *)m, windowID);
	pt = g->draw_tool.tool;
	g->draw_tool.tool = GM_TOOL_ZOOM;
	start_draw_with_selected_tool(m, ui_win_find_el_by_id(w, GM_MAIN_ID_DRAW));
	g->draw_tool.tool = pt;
}

static void	start_zoom_out(void *m, void *wid)
{
	int			pt;
	t_guimp		*g;
	t_ui_win	*w;
	Uint32		windowID;

	g = (t_guimp *)(((t_ui_main *)m)->data);
	windowID = *((Uint32 *)wid);
	w = ui_main_find_window_by_id((t_ui_main *)m, windowID);
	pt = g->draw_tool.tool;
	g->draw_tool.tool = GM_TOOL_ZOOM;
	start_alt_with_selected_tool(m, ui_win_find_el_by_id(w, GM_MAIN_ID_DRAW));
	g->draw_tool.tool = pt;
}

static void	draw_with_selected_tool(void *main, void *el_v)
{
	t_guimp	*g;
	t_ui_el	*el;
	int		x;
	int		y;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	el = (t_ui_el *)el_v;
	x = ((float)el->ptr_rel_pos.x / (float)el->rect.w) * g->zoom_rect.w + g->zoom_rect.x;
	y = ((float)el->ptr_rel_pos.y / (float)el->rect.h) * g->zoom_rect.h + g->zoom_rect.y;
	g->draw_tool.cur_point = (t_vec2){x, y};
	if (g->draw_tool.tool == GM_TOOL_BRUSH)
	{
		SDL_SetRenderTarget(el->sdl_renderer, (SDL_Texture *)(g->layers.current_layer->sdl_textures->content));
		SDL_SetTextureColorMod((SDL_Texture *)(el->sdl_textures->content), g->draw_tool.r, g->draw_tool.g, g->draw_tool.b);
		SDL_RenderCopy(el->sdl_renderer, (SDL_Texture *)(el->sdl_textures->content), NULL, &((t_rect){
			x - g->draw_tool.brush_size / 2,
			y - g->draw_tool.brush_size / 2,
			g->draw_tool.brush_size,
			g->draw_tool.brush_size
		}));
	}
	if (g->draw_tool.tool == GM_TOOL_HAND)
	{
		g->zoom_rect.x += (abs(g->draw_tool.prew_point.x - g->draw_tool.cur_point.x) >= GM_HAND_MIN_DIST ? g->draw_tool.prew_point.x - g->draw_tool.cur_point.x : 0) * GM_HAND_MOVE_SPEED;
		g->zoom_rect.y += (abs(g->draw_tool.prew_point.y - g->draw_tool.cur_point.y) >= GM_HAND_MIN_DIST ? g->draw_tool.prew_point.y - g->draw_tool.cur_point.y : 0) * GM_HAND_MOVE_SPEED;
		g->zoom_rect.x = g->zoom_rect.x < 0 ? 0 : g->zoom_rect.x;
		g->zoom_rect.x = g->zoom_rect.x + g->zoom_rect.w > GM_IMAGE_SIZE_X ? GM_IMAGE_SIZE_X - g->zoom_rect.w : g->zoom_rect.x;
		g->zoom_rect.y = g->zoom_rect.y < 0 ? 0 : g->zoom_rect.y;
		g->zoom_rect.y = g->zoom_rect.y + g->zoom_rect.h > GM_IMAGE_SIZE_Y ? GM_IMAGE_SIZE_Y - g->zoom_rect.h : g->zoom_rect.y;
	}
	SDL_SetRenderTarget(el->sdl_renderer, NULL);
	g->draw_tool.prew_point = (t_vec2){x, y};
}

static void choose_brush(void *main, void *el_v)
{
	t_guimp	*g;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	(void)el_v;
	g->draw_tool.tool = GM_TOOL_BRUSH;
}

static void choose_zoom(void *main, void *el_v)
{
	t_guimp	*g;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	(void)el_v;
	g->draw_tool.tool = GM_TOOL_ZOOM;
}

static void choose_hand(void *main, void *el_v)
{
	t_guimp	*g;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	(void)el_v;
	g->draw_tool.tool = GM_TOOL_HAND;
}

static void	choose_color(void *main, void *el_v)
{
	t_guimp	*g;
	t_ui_el	*el;
	t_ui_el	*chil;
	int		res;
	int		max;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	el = (t_ui_el *)el_v;
	chil = ((t_ui_el *)el->children->content);
	max = (el->id == GM_TOOL_ID_SL_HEAD_SZ) ? GM_BRUSH_MAX_SIZE : 255;
	ui_el_set_new_pos(chil, 0, PIXEL, (t_fvec2){el->ptr_rel_pos.x - chil->rect.w / 2, 0});
	res = ((float)(el->ptr_rel_pos.x) / (float)el->rect.w) * (float)max;
	if (chil->id == GM_TOOL_ID_SL_HEAD_RED)
		g->draw_tool.r = res;
	else if (chil->id == GM_TOOL_ID_SL_HEAD_GR)
		g->draw_tool.g = res;
	else if (chil->id == GM_TOOL_ID_SL_HEAD_BL)
		g->draw_tool.b = res;
	else if (chil->id == GM_TOOL_ID_SL_HEAD_SZ)
		g->draw_tool.brush_size = res;
}

static void	move_draw_canvas_with_zoom(void *main, void *el_v)
{
	t_guimp	*g;
	t_ui_el	*el;
	float	x;
	float	y;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	el = (t_ui_el *)el_v;
	x = 0;
	y = 0;
	if (g->draw_tool.tool == GM_TOOL_ZOOM)
	{
		if (el->ptr_rel_pos.x > el->rect.w * (1.0 - GM_ZOOM_MOVE_ZONE))
			x = ((float)el->ptr_rel_pos.x / (float)el->rect.w - (1.0 - GM_ZOOM_MOVE_ZONE)) / (GM_ZOOM_MOVE_ZONE);
		if (el->ptr_rel_pos.x < el->rect.w * GM_ZOOM_MOVE_ZONE)
			x = -(GM_ZOOM_MOVE_ZONE - (float)el->ptr_rel_pos.x / (float)el->rect.w) / GM_ZOOM_MOVE_ZONE;
		if (el->ptr_rel_pos.y > el->rect.h * (1.0 - GM_ZOOM_MOVE_ZONE))
			y = ((float)el->ptr_rel_pos.y / (float)el->rect.h - (1.0 - GM_ZOOM_MOVE_ZONE)) / (GM_ZOOM_MOVE_ZONE);
		if (el->ptr_rel_pos.y < el->rect.h * GM_ZOOM_MOVE_ZONE)
			y = -(GM_ZOOM_MOVE_ZONE - (float)el->ptr_rel_pos.y / (float)el->rect.h) / GM_ZOOM_MOVE_ZONE;
		g->zoom_rect.x += x * (GM_ZOOM_MOVE_SPEED / g->draw_tool.zoom);
		g->zoom_rect.y += y * (GM_ZOOM_MOVE_SPEED / g->draw_tool.zoom);
		g->zoom_rect.x = g->zoom_rect.x < 0 ? 0 : g->zoom_rect.x;
		g->zoom_rect.x = g->zoom_rect.x + g->zoom_rect.w > GM_IMAGE_SIZE_X ? GM_IMAGE_SIZE_X - g->zoom_rect.w : g->zoom_rect.x;
		g->zoom_rect.y = g->zoom_rect.y < 0 ? 0 : g->zoom_rect.y;
		g->zoom_rect.y = g->zoom_rect.y + g->zoom_rect.h > GM_IMAGE_SIZE_Y ? GM_IMAGE_SIZE_Y - g->zoom_rect.h : g->zoom_rect.y;
	}
}

static void	draw_color_rect(void *el_v, void *main)
{
	t_guimp	*g;
	t_ui_el	*el;

	g = (t_guimp *)(((t_ui_main *)main)->data);
	el = (t_ui_el *)el_v;
	SDL_SetRenderTarget(el->sdl_renderer, ui_el_get_current_texture(el));
	SDL_SetRenderDrawColor(el->sdl_renderer, g->draw_tool.r, g->draw_tool.g, g->draw_tool.b, 255);
	SDL_RenderFillRect(el->sdl_renderer, NULL);
	SDL_SetRenderTarget(el->sdl_renderer, NULL);
	SDL_RenderCopy(el->sdl_renderer, ui_el_get_current_texture(el), NULL, &el->rect);
}



int		main()
{
	t_guimp	g_main;
	t_ui_el	*tmp_el;
	t_ui_el	*tmp_el_p1;
	t_ui_el	*tmp_el_p2;


	// char *res = NULL;
	// // ui_open_file_dialog(&res);
	// // printf("%s\n", res);
	// // ui_save_file_dialog(&res);
	// // printf("%s\n", res);

	/********/
	/* INIT */
	/********/
	ui_sdl_init();
	gm_init(&g_main);
	if (!(g_main.ui_main = (t_ui_main *)malloc(sizeof(t_ui_main))))
	{
		printf("ui_main malloc error in struct g_main\n");
		return (0);
	}
	ui_main_init(g_main.ui_main);
	ui_main_fill_default_surfaces(g_main.ui_main);
	g_main.ui_main->data = (void *)(&g_main);
	g_main.draw_tool.brush_size = GM_BRUSH_DEF_SIZE;
	g_main.draw_tool.zoom = 1;
	g_main.draw_tool.tool = GM_TOOL_BRUSH;
	g_main.zoom_rect.x = 0;
	g_main.zoom_rect.y = 0;
	g_main.zoom_rect.w = GM_IMAGE_SIZE_X;
	g_main.zoom_rect.h = GM_IMAGE_SIZE_Y;


	/************/
	/* MAIN_WIN */
	/************/
	if (!(g_main.main_win = (t_ui_win *)malloc(sizeof(t_ui_win))))
	{
		printf("main_win malloc error in struct g_main\n");
		return (0);
	}
	ui_win_init(g_main.main_win);
	g_main.main_win->title = ft_strdup("GUIMP");
	g_main.main_win->params = WIN_MAIN | WIN_RESIZABLE;
	g_main.main_win->size = (t_vec2){GM_MAIN_WIN_W, GM_MAIN_WIN_H};
	g_main.main_win->pos = (t_vec2){GM_MAIN_WIN_X, GM_MAIN_WIN_Y};
	ui_win_setup_default(g_main.main_win);
	ui_win_create(g_main.main_win);
	ui_main_add_window(g_main.ui_main, g_main.main_win);
	ui_event_add_listener(&(g_main.main_win->events.onResize), &ui_win_update_size);
	ui_event_add_listener(&(g_main.main_win->events.onMoved), &move_windows);
	ui_event_add_listener(&(g_main.main_win->events.onScrollUp), &start_zoom_in);
	ui_event_add_listener(&(g_main.main_win->events.onScrollDown), &start_zoom_out);
	ui_el_add_texture_from_main_by_id(g_main.ui_main, &(g_main.main_win->canvas), "flower", "default");
	ui_event_add_listener(&(g_main.main_win->canvas.events.onRender), &ui_el_draw_event);


	/*MAIN ELEM*/
		if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
		{
			printf("main_elem malloc error in canvas in main_win\n");
			return (0);
		}
		ui_el_init(tmp_el);
		ui_el_setup_default(tmp_el);
		ui_el_add_child(&(g_main.main_win->canvas), tmp_el);
		ui_el_set_pos(tmp_el, 0, 0, (t_fvec2){0.05, 0.05});
		ui_el_set_size(tmp_el, 0, 0, (t_fvec2){0.69, 0.9});
		tmp_el->id = GM_MAIN_ID_DRAW;
		ui_el_set_default_resize(tmp_el);
		ui_event_clear(&(tmp_el->events.onRender));
		ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el, "brush", "brush");
		ui_event_add_listener(&(tmp_el->events.onRender), &draw_canvas_renderer);
		ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonHold), &draw_with_selected_tool);
		ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &start_draw_with_selected_tool);
		ui_event_add_listener(&(tmp_el->events.onPointerRightButtonPressed), &start_alt_with_selected_tool);
		ui_event_add_listener(&(tmp_el->events.onPointerStay), &move_draw_canvas_with_zoom);


	/*LAYERS SCROLLABLE MENU*/
		if (!(tmp_el_p2 = (t_ui_el *)malloc(sizeof(t_ui_el))))
		{
			printf("scrollable menu malloc error in canvas in layer_win\n");
			return (0);
		}
		ui_el_setup_default_scroll_menu(tmp_el_p2);
		ui_el_add_child(&(g_main.main_win->canvas), tmp_el_p2);
		ui_el_set_pos(tmp_el_p2, 0, 0, (t_fvec2){0.75, 0.01});
		ui_el_set_size(tmp_el_p2, 0, 0, (t_fvec2){0.245, 0.89});
		tmp_el_p2->id = GM_LAYER_ID_MENU;
		ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el_p2, "priso", "default");
		ui_el_set_menu_resize(tmp_el_p2);
		g_main.layers.layers = tmp_el_p2->children;

	/*DEFAULT LAYER*/
			if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
			{
				printf("default layer malloc error in scrollable menu in layer_win\n");
				return (0);
			}
			ui_el_setup_default_scroll_menu_elem(tmp_el, tmp_el_p2);
			ui_el_set_pos(tmp_el, 0, 0, (t_fvec2){0.1, 0.05});
			ui_el_set_size(tmp_el, 0, 0, (t_fvec2){0.8, 0.25});
			tmp_el->id = GM_LAYER_ID_DEF_LAYER;
			ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el, "layer_place", "default");
			ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el, "layer_onPtr", "onFocus");
			ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el, "layer_active", "onActive");
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &testOnPtrLBD);
			ui_event_add_listener(&(tmp_el->events.onPointerEnter), &testOnPtrEnter);
			ui_event_add_listener(&(tmp_el->events.onPointerExit), &testOnPtrExit);
			ui_el_set_current_texture_by_id(tmp_el, "onActive");

	/*DEFAULT LAYER TEXTURE*/
				if (!(tmp_el_p1 = (t_ui_el *)malloc(sizeof(t_ui_el))))
				{
					printf("default layer malloc error in scrollable menu in layer_win\n");
					return (0);
				}
				ui_el_setup_default(tmp_el_p1);
				ui_el_add_child(tmp_el, tmp_el_p1);
				ui_el_set_pos(tmp_el_p1, 0, 0, (t_fvec2){0.1, 0.1});
				ui_el_set_size(tmp_el_p1, 0, 0, (t_fvec2){0.8, 0.8});
				tmp_el_p1->id = GM_LAYER_ID_DEF_LAYER * 1000;
				tmp_el_p1->params |= EL_IGNOR_RAYCAST | EL_IS_DEPENDENT;
				ui_el_add_white_texture(tmp_el_p1, GM_IMAGE_SIZE_X, GM_IMAGE_SIZE_Y, "default");
				ui_el_set_current_texture_by_id(tmp_el_p1, "default");
				g_main.layers.current_layer = tmp_el_p1;

				t_list	*tmp;
				tmp = ft_lstnew(NULL, 0);
				tmp->content = tmp_el_p1->sdl_textures->content;
				tmp->content_size = tmp_el->id;
				ft_lstadd_back(&(g_main.layers.layers), tmp);

	/*ADD BUTTON*/
		if (!(tmp_el_p1 = (t_ui_el *)malloc(sizeof(t_ui_el))))
		{
			printf("add button malloc error in canvas in layer_win\n");
			return (0);
		}
		ui_el_setup_default(tmp_el_p1);
		ui_el_add_child(&(g_main.main_win->canvas), tmp_el_p1);
		ui_el_set_pos(tmp_el_p1, 0, 0, (t_fvec2){0.75, 0.91});
		ui_el_set_size(tmp_el_p1, 0, 0, (t_fvec2){0.12, 0.08});
		tmp_el_p1->id = GM_LAYER_ID_ADD;
		ui_el_set_default_resize(tmp_el_p1);
		ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el_p1, "priso", "default");
		ui_event_add_listener(&(tmp_el_p1->events.onPointerLeftButtonPressed), &test_add_layer);

	/*DEL BUTTON*/
		if (!(tmp_el_p1 = (t_ui_el *)malloc(sizeof(t_ui_el))))
		{
			printf("del button malloc error in canvas in layer_win\n");
			return (0);
		}
		ui_el_setup_default(tmp_el_p1);
		ui_el_add_child(&(g_main.main_win->canvas), tmp_el_p1);
		ui_el_set_pos(tmp_el_p1, 0, 0, (t_fvec2){0.875, 0.91});
		ui_el_set_size(tmp_el_p1, 0, 0, (t_fvec2){0.12, 0.08});
		tmp_el_p1->id = GM_LAYER_ID_DEL;
		ui_el_set_default_resize(tmp_el_p1);
		ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el_p1, "priso", "default");
		ui_event_add_listener(&(tmp_el_p1->events.onPointerLeftButtonPressed), &test_del_layer);



	/************/
	/* TOOL_WIN */
	/************/
	if (!(g_main.tool_win = (t_ui_win *)malloc(sizeof(t_ui_win))))
	{
		printf("tool_win malloc error in struct g_main\n");
		return (0);
	}
	ui_win_init(g_main.tool_win);
	g_main.tool_win->title = ft_strdup("TOOLS");
	g_main.tool_win->params = 0;
	g_main.tool_win->size = (t_vec2){GM_TOOL_WIN_W, GM_TOOL_WIN_H};
	g_main.tool_win->pos = (t_vec2){GM_TOOL_WIN_X, GM_TOOL_WIN_Y};
	ui_win_setup_default(g_main.tool_win);
	ui_win_create(g_main.tool_win);
	ui_main_add_window(g_main.ui_main, g_main.tool_win);
	ui_event_add_listener(&(g_main.tool_win->events.onMoved), &move_windows);
	ui_el_add_texture_from_main_by_id(g_main.ui_main, &(g_main.tool_win->canvas), "flower", "default");
	ui_event_add_listener(&(g_main.tool_win->canvas.events.onRender), &ui_el_draw_event);

	/*DRAW BUTTONS SCROLLABLE MENU*/
		if (!(tmp_el_p1 = (t_ui_el *)malloc(sizeof(t_ui_el))))
		{
			printf("scrollable menu malloc error in canvas in tool_win\n");
			return (0);
		}
		ui_el_setup_default_scroll_menu(tmp_el_p1);
		ui_el_add_child(&(g_main.tool_win->canvas), tmp_el_p1);
		ui_el_set_pos(tmp_el_p1, 0, 0, (t_fvec2){0.01, 0.01});
		ui_el_set_size(tmp_el_p1, 0, 0, (t_fvec2){0.98, 0.59});
		tmp_el_p1->id = GM_TOOL_ID_BUT_MENU;
		ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el_p1, "priso", "default");

	/*SELECT BRUSH BUTTON*/
			if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
			{
				printf("draw_brush malloc error in scrollable menu in tool_win\n");
				return (0);
			}
			ui_el_setup_default_scroll_menu_elem(tmp_el, tmp_el_p1);
			ui_el_set_pos(tmp_el, 0, 0, (t_fvec2){0.1, 0.05});
			ui_el_set_size(tmp_el, 0, PIXEL, (t_fvec2){GM_TOOL_WIN_W * 0.35, GM_TOOL_WIN_W * 0.35});
			tmp_el->id = GM_TOOL_ID_BRUSH;
			tmp_el->sdl_renderer = g_main.tool_win->sdl_renderer;
			ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el, "brush_icon", "default");
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &choose_brush);

	/*ERASER BUTTON*/
			if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
			{
				printf("eraser_brush malloc error in scrollable menu in tool_win\n");
				return (0);
			}
			ui_el_setup_default_scroll_menu_elem(tmp_el, tmp_el_p1);
			ui_el_set_pos(tmp_el, 0, 0, (t_fvec2){0.55, 0.05});
			ui_el_set_size(tmp_el,  0, PIXEL, (t_fvec2){GM_TOOL_WIN_W * 0.35, GM_TOOL_WIN_W * 0.35});
			tmp_el->id = GM_TOOL_ID_ERASER;
			ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el, "eraser_icon", "default");
			//ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &choose_zoom);

	/*ZOOM BUTTON*/
			if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
			{
				printf("zoom_brush malloc error in scrollable menu in tool_win\n");
				return (0);
			}
			ui_el_setup_default_scroll_menu_elem(tmp_el, tmp_el_p1);
			ui_el_set_pos(tmp_el, 0, 0, (t_fvec2){0.1, 0.33});
			ui_el_set_size(tmp_el,  0, PIXEL, (t_fvec2){GM_TOOL_WIN_W * 0.35, GM_TOOL_WIN_W * 0.35});
			tmp_el->id = GM_TOOL_ID_ZOOM;
			ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el, "zoom_icon", "default");
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &choose_zoom);

	/*HAND BUTTON*/
			if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
			{
				printf("zoom_brush malloc error in scrollable menu in tool_win\n");
				return (0);
			}
			ui_el_setup_default_scroll_menu_elem(tmp_el, tmp_el_p1);
			ui_el_set_pos(tmp_el, 0, 0, (t_fvec2){0.55, 0.33});
			ui_el_set_size(tmp_el,  0, PIXEL, (t_fvec2){GM_TOOL_WIN_W * 0.35, GM_TOOL_WIN_W * 0.35});
			tmp_el->id = GM_TOOL_ID_ZOOM;
			ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el, "hand_icon", "default");
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &choose_hand);

	/*SETTINGS MENU*/
		if (!(tmp_el_p1 = (t_ui_el *)malloc(sizeof(t_ui_el))))
		{
			printf("settings menu malloc error in canvas in tool_win\n");
			return (0);
		}
		ui_el_setup_default(tmp_el_p1);
		ui_el_add_child(&(g_main.tool_win->canvas), tmp_el_p1);
		ui_el_set_pos(tmp_el_p1, 0, 0, (t_fvec2){0.01, 0.61});
		ui_el_set_size(tmp_el_p1, 0, 0, (t_fvec2){0.98, 0.38});
		tmp_el_p1->id = GM_TOOL_ID_SET_MENU;
		ui_el_add_texture_from_main_by_id(g_main.ui_main, tmp_el_p1, "frolushka", "default");

	/*SLIDER ROOT*/
/*RED*/
			if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
			{
				printf("eraser_brush malloc error in scrollable menu in tool_win\n");
				return (0);
			}
			ui_el_setup_default(tmp_el);
			ui_el_add_child(tmp_el_p1, tmp_el);
			ui_el_set_pos(tmp_el, 0, 0, (t_fvec2){0.05, 0.08});
			ui_el_set_size(tmp_el, 0, 0, (t_fvec2){0.7, 0.15});
			tmp_el->id = GM_TOOL_ID_SL_ROOT_RED;
			ui_el_add_gradient_texture(tmp_el, (t_vec2){tmp_el->rect.w, tmp_el->rect.h}, 0xFF0000, "default");
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonHold), &choose_color);
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &choose_color);

	/*SLIDER HEAD*/
				if (!(tmp_el_p2 = (t_ui_el *)malloc(sizeof(t_ui_el))))
				{
					printf("eraser_brush malloc error in scrollable menu in tool_win\n");
					return (0);
				}
				ui_el_setup_default(tmp_el_p2);
				ui_el_add_child(tmp_el, tmp_el_p2);
				ui_el_set_pos(tmp_el_p2, 0, 0, (t_fvec2){0, 0});
				ui_el_set_size(tmp_el_p2, 0, 0, (t_fvec2){0.06, 1});
				tmp_el_p2->id = GM_TOOL_ID_SL_HEAD_RED;
				ui_el_add_color_texture(tmp_el_p2, (t_vec2){tmp_el->rect.w, tmp_el->rect.h}, 0xAAAAAA, "default");
				tmp_el_p2->params |= EL_IGNOR_RAYCAST;

	/*SLIDER ROOT*/
/*GREEN*/
			if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
			{
				printf("eraser_brush malloc error in scrollable menu in tool_win\n");
				return (0);
			}
			ui_el_setup_default(tmp_el);
			ui_el_add_child(tmp_el_p1, tmp_el);
			ui_el_set_pos(tmp_el, 0, 0, (t_fvec2){0.05, 0.31});
			ui_el_set_size(tmp_el, 0, 0, (t_fvec2){0.7, 0.15});
			tmp_el->id = GM_TOOL_ID_SL_ROOT_GR;
			ui_el_add_gradient_texture(tmp_el, (t_vec2){tmp_el->rect.w, tmp_el->rect.h}, 0x00FF00, "default");
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonHold), &choose_color);
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &choose_color);

	/*SLIDER HEAD*/
				if (!(tmp_el_p2 = (t_ui_el *)malloc(sizeof(t_ui_el))))
				{
					printf("eraser_brush malloc error in scrollable menu in tool_win\n");
					return (0);
				}
				ui_el_setup_default(tmp_el_p2);
				ui_el_add_child(tmp_el, tmp_el_p2);
				ui_el_set_pos(tmp_el_p2, 0, 0, (t_fvec2){0, 0});
				ui_el_set_size(tmp_el_p2, 0, 0, (t_fvec2){0.06, 1});
				tmp_el_p2->id = GM_TOOL_ID_SL_HEAD_GR;
				ui_el_add_color_texture(tmp_el_p2, (t_vec2){tmp_el->rect.w, tmp_el->rect.h}, 0xAAAAAA, "default");
				tmp_el_p2->params |= EL_IGNOR_RAYCAST;

	/*SLIDER ROOT*/
/*BLUE*/
			if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
			{
				printf("eraser_brush malloc error in scrollable menu in tool_win\n");
				return (0);
			}
			ui_el_setup_default(tmp_el);
			ui_el_add_child(tmp_el_p1, tmp_el);
			ui_el_set_pos(tmp_el, 0, 0, (t_fvec2){0.05, 0.54});
			ui_el_set_size(tmp_el, 0, 0, (t_fvec2){0.7, 0.15});
			tmp_el->id = GM_TOOL_ID_SL_ROOT_BL;
			ui_el_add_gradient_texture(tmp_el, (t_vec2){tmp_el->rect.w, tmp_el->rect.h}, 0x0000FF, "default");
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonHold), &choose_color);
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &choose_color);

	/*SLIDER HEAD*/
				if (!(tmp_el_p2 = (t_ui_el *)malloc(sizeof(t_ui_el))))
				{
					printf("eraser_brush malloc error in scrollable menu in tool_win\n");
					return (0);
				}
				ui_el_setup_default(tmp_el_p2);
				ui_el_add_child(tmp_el, tmp_el_p2);
				ui_el_set_pos(tmp_el_p2, 0, 0, (t_fvec2){0, 0});
				ui_el_set_size(tmp_el_p2, 0, 0, (t_fvec2){0.06, 1});
				tmp_el_p2->id = GM_TOOL_ID_SL_HEAD_BL;
				ui_el_add_color_texture(tmp_el_p2, (t_vec2){tmp_el->rect.w, tmp_el->rect.h}, 0xAAAAAA, "default");
				tmp_el_p2->params |= EL_IGNOR_RAYCAST;

	/*SLIDER ROOT*/
/*SIZE*/
			if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
			{
				printf("eraser_brush malloc error in scrollable menu in tool_win\n");
				return (0);
			}
			ui_el_setup_default(tmp_el);
			ui_el_add_child(tmp_el_p1, tmp_el);
			ui_el_set_pos(tmp_el, 0, 0, (t_fvec2){0.05, 0.77});
			ui_el_set_size(tmp_el, 0, 0, (t_fvec2){0.7, 0.15});
			tmp_el->id = GM_TOOL_ID_SL_ROOT_SZ;
			ui_el_add_gradient_texture(tmp_el, (t_vec2){tmp_el->rect.w, tmp_el->rect.h}, 0x000000, "default");
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonHold), &choose_color);
			ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &choose_color);

	/*SLIDER HEAD*/
				if (!(tmp_el_p2 = (t_ui_el *)malloc(sizeof(t_ui_el))))
				{
					printf("eraser_brush malloc error in scrollable menu in tool_win\n");
					return (0);
				}
				ui_el_setup_default(tmp_el_p2);
				ui_el_add_child(tmp_el, tmp_el_p2);
				ui_el_set_pos(tmp_el_p2, 0, 0, (t_fvec2){0, 0});
				ui_el_set_size(tmp_el_p2, 0, 0, (t_fvec2){0.06, 1});
				tmp_el_p2->id = GM_TOOL_ID_SL_HEAD_SZ;
				ui_el_add_color_texture(tmp_el_p2, (t_vec2){tmp_el->rect.w, tmp_el->rect.h}, 0xAAAAAA, "default");
				tmp_el_p2->params |= EL_IGNOR_RAYCAST;
	/*COLOR_RECT*/
			if (!(tmp_el = (t_ui_el *)malloc(sizeof(t_ui_el))))
			{
				printf("eraser_brush malloc error in scrollable menu in tool_win\n");
				return (0);
			}
			ui_el_setup_default(tmp_el);
			ui_el_add_child(tmp_el_p1, tmp_el);
			ui_el_set_pos(tmp_el, 0, 0, (t_fvec2){0.825, 0.45});
			ui_el_set_size(tmp_el, 0, PIXEL, (t_fvec2){0.1 * (float)GM_TOOL_WIN_W, 0.1 * (float)GM_TOOL_WIN_W});
			tmp_el->id = GM_TOOL_ID_CL_RECT;
			ui_el_add_color_texture(tmp_el, (t_vec2){tmp_el->rect.w, tmp_el->rect.h}, 0x000000, "default");
			ui_event_clear(&(tmp_el->events.onRender));
			ui_event_add_listener(&(tmp_el->events.onRender), &draw_color_rect);
			// ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonHold), &choose_color);
			// ui_event_add_listener(&(tmp_el->events.onPointerLeftButtonPressed), &choose_color);


	/*************/
	/* MAIN_LOOP */
	/*************/
	ui_main_loop(g_main.ui_main);
	return (0);
}
