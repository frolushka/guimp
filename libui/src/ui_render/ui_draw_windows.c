/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_draw_windows.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edraugr- <edraugr-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 06:06:48 by edraugr-          #+#    #+#             */
/*   Updated: 2019/06/04 15:55:03 by edraugr-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libui.h"

void	ui_draw_windows(t_ui_main *m)
{
	t_list	*node;

	node = m->windows;
	while (node)
	{
		ui_draw_window((t_ui_win *)node->content, m);
		node = node->next;
	}
}
