/* functions that manipulate the frame structure.
 * Copyright (C) 2000-2003 Shawn Betts
 *
 * This file is part of ratpoison.
 *
 * ratpoison is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * ratpoison is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 */

#include "ratpoison.h"

int
frame_left (rp_frame *frame)
{
  return frame->x;
}

int
frame_top (rp_frame *frame)
{
  return frame->y;
}

int
frame_right (rp_frame *frame)
{
  return frame->x + frame->width;
}

int
frame_bottom (rp_frame *frame)
{
  return frame->y + frame->height;
}

int
frame_width(rp_frame *frame)
{
  return frame->width;
}

int
frame_height(rp_frame *frame)
{
  return frame->height;
}

void
frame_resize_left (rp_frame *frame, int amount)
{
  frame->x -= amount;
  frame->width += amount;
}

void
frame_resize_right (rp_frame *frame, int amount)
{
  frame->width += amount;
}

void
frame_resize_up (rp_frame *frame, int amount)
{
  frame->y -= amount;
  frame->height += amount;
}

void
frame_resize_down (rp_frame *frame, int amount)
{
  frame->height += amount;
}

void
frame_move_left (rp_frame *frame, int amount)
{
  frame->x -= amount;
}

void
frame_move_right (rp_frame *frame, int amount)
{
  frame->x += amount;
}

void
frame_move_up (rp_frame *frame, int amount)
{
  frame->y -= amount;
}

void
frame_move_down (rp_frame *frame, int amount)
{
  frame->y += amount;
}

rp_frame *
frame_new (rp_screen *s)
{
  rp_frame *f;

  f = xmalloc (sizeof (rp_frame));
  f->number = numset_request (s->frames_numset);
  f->last_access = 0;

  return f;
}

void
frame_free (rp_screen *s, rp_frame *f)
{
  numset_release (s->frames_numset, f->number);
  free (f);
}


rp_frame *
frame_copy (rp_frame *frame)
{
  rp_frame *copy;

  copy = xmalloc (sizeof (rp_frame));

  copy->number = frame->number;
  copy->x = frame->x;
  copy->y = frame->y;
  copy->width = frame->width;
  copy->height = frame->height;
  copy->win_number = frame->win_number;
  copy->last_access = frame->last_access;

  return copy;
}

char *
frame_dump (rp_frame *frame)
{
  rp_window *win;
  char *tmp;
  struct sbuf *s;

  /* rather than use win_number, use the X11 window ID. */
  win = find_window_number (frame->win_number);

  s = sbuf_new (0);
  sbuf_printf (s, "%d %d %d %d %d %ld %d", 
	       frame->number,
	       frame->x,
	       frame->y,
	       frame->width,
	       frame->height,
	       win ? win->w:0,
	       frame->last_access);

  /* Extract the string and return it, and don't forget to free s. */
  tmp = sbuf_get (s);
  free (s);
  return tmp;
}

rp_frame *
frame_read (char *str)
{
  Window w;
  rp_window *win;
  rp_frame *f;

  f = xmalloc (sizeof (rp_frame));
  if (sscanf (str, "%d %d %d %d %d %ld %d", 
	      &f->number,
	      &f->x,
	      &f->y,
	      &f->width,
	      &f->height,
	      &w,
	      &f->last_access) < 7)
    {
      free (f);
      return NULL;
    }

  /* Perform some integrity checks on what we got and fix any
     problems. */
  if (f->number <= 0)
    f->number = 0;
  if (f->x <= 0)
    f->x = 0;
  if (f->y <= 0)
    f->y = 0;
  if (f->width <= defaults.window_border_width*2)
    f->width = defaults.window_border_width*2 + 1;
  if (f->height <= defaults.window_border_width*2)
    f->height = defaults.window_border_width*2 + 1;
  if (f->last_access < 0)
    f->last_access = 0;

  /* Find the window with the X11 window ID. */
  win = find_window_in_list (w, &rp_mapped_window);
  if (win)
    {
      f->win_number = win->number;
    }
  else
    {
      f->win_number = EMPTY;
    }

  return f;
}
