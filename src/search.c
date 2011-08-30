/* vifm
 * Copyright (C) 2001 Ken Steen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <sys/types.h>

#include <curses.h>
#include <regex.h>

#include <string.h>

#include "config.h"
#include "filelist.h"
#include "ui.h"
#include "utils.h"

#include "search.h"

enum
{
	PREVIOUS,
	NEXT
};

static int
find_next_pattern_match(FileView *view, int start, int direction)
{
	int found = 0;
	int x;

	if(direction == PREVIOUS)
	{
		for(x = start - 1; x > 0; x--)
		{
			if(view->dir_entry[x].search_match)
			{
				found = 1;
				view->list_pos = x;
				break;
			}
		}
	}
	else if(direction == NEXT)
	{
		for(x = start + 1; x < view->list_rows; x++)
		{
			if(view->dir_entry[x].search_match)
			{
				found = 1;
				view->list_pos = x;
				break;
			}
		}
	}
	return found;
}

void
find_previous_pattern(FileView *view, int wrap)
{
	if(find_next_pattern_match(view, view->list_pos, PREVIOUS))
		moveto_list_pos(view, view->list_pos);
	else if(wrap && find_next_pattern_match(view, view->list_rows, PREVIOUS))
		moveto_list_pos(view, view->list_pos);
}

void
find_next_pattern(FileView *view, int wrap)
{
	if(find_next_pattern_match(view, view->list_pos, NEXT))
		moveto_list_pos(view, view->list_pos);
	else if(wrap && find_next_pattern_match(view, 0, NEXT))
		moveto_list_pos(view, view->list_pos);
}

int
find_pattern(FileView *view, const char *pattern, int backward, int move)
{
	int cflags;
	int found = 0;
	regex_t re;
	int x;
	int err;

	if(move)
		clean_selected_files(view);
	for(x = 0; x < view->list_rows; x++)
		view->dir_entry[x].search_match = 0;

	cflags = get_regexp_cflags(pattern);
	if((err = regcomp(&re, pattern, cflags)) == 0)
	{
		if(pattern != view->regexp)
			snprintf(view->regexp, sizeof(view->regexp), "%s", pattern);

		for(x = 0; x < view->list_rows; x++)
		{
			char buf[NAME_MAX];

			if(strcmp(view->dir_entry[x].name, "../") == 0)
				continue;

			strncpy(buf, view->dir_entry[x].name, sizeof(buf));
			chosp(buf);
			if(regexec(&re, buf, 0, NULL, 0) != 0)
				continue;

			view->dir_entry[x].search_match = 1;
			if(cfg.hl_search)
			{
				view->dir_entry[x].selected = 1;
				view->selected_files++;
			}
			found++;
		}
		regfree(&re);
	}
	else
	{
		status_bar_messagef("Regexp error: %s", get_regexp_error(err, &re));
		regfree(&re);
		return 1;
	}

	/* Need to redraw the list so that the matching files are highlighted */
	draw_dir_list(view, view->top_line);

	if(found > 0)
	{
		if(move)
		{
			if(backward)
				find_previous_pattern(view, 1);
			else
				find_next_pattern(view, 1);
		}
		if(!cfg.hl_search)
		{
			view->matches = found;
			status_bar_messagef("%d matching files for %s", found, view->regexp);
			return 1;
		}
		return 0;
	}
	else
	{
		moveto_list_pos(view, view->list_pos);
		status_bar_messagef("No matching files for %s", view->regexp);
		return 1;
	}
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 : */
