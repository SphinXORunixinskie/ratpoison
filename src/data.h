/* our datatypes and global variables 
 * Copyright (C) 2000, 2001 Shawn Betts
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

#ifndef _RATPOISON_DATA_H
#define _RATPOISON_DATA_H

#include "linkedlist.h"
#include "number.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define FONT_HEIGHT(f) ((f)->max_bounds.ascent + (f)->max_bounds.descent)

#define WIN_EVENTS (StructureNotifyMask | PropertyChangeMask | ColormapChangeMask | FocusChangeMask)
/* EMPTY is used when a frame doesn't contain a window, or a window
   doesn't have a frame. Any time a field refers to the number of a
   window/frame/screen/etc, Use EMPTY to denote a lack there of. */
#define EMPTY -1

/* Possible values for defaults.window_list_style */
#define STYLE_ROW    0 
#define STYLE_COLUMN 1

/* Possible values for defaults.win_name */
#define WIN_NAME_TITLE 		0
#define WIN_NAME_RES_CLASS 	1
#define WIN_NAME_RES_NAME 	2

typedef struct rp_window rp_window;
typedef struct rp_screen rp_screen;
typedef struct rp_action rp_action;
typedef struct rp_frame rp_frame;
typedef struct rp_child_info rp_child_info;

struct rp_frame
{
  int number;
  int x, y, width, height;

  /* The number of the window that is focused in this frame. */
  int win_number;

  /* For determining the last frame. */
  int last_access;

  struct list_head node;
};

struct rp_window
{
  rp_screen *scr;
  Window w;
  int number;
  int state;
  int last_access;
  int named;
  
  /* Window name hints. */
  char *user_name;
  char *wm_name;
  char *res_name;
  char *res_class;

  /* Dimensions */
  int x, y, width, height, border;

  /* WM Hints */
  XSizeHints *hints;

  /* Colormap */
  Colormap colormap;

  /* Is this a transient window? */
  int transient;
  Window transient_for;

  /* Saved mouse position */
  int mouse_x, mouse_y;

  /* The alignment of the window. Decides to what side or corner the
     window sticks to. */
  int gravity;

  /* A window can be visible inside a frame but not the frame's
     current window. This keeps track of what frame the window was
     mapped into. */
  int frame_number;

  struct list_head node;
};

struct rp_screen
{
  GC normal_gc;
  XWindowAttributes root_attr;
  Window root, bar_window, key_window, input_window, frame_window, help_window;
  int bar_is_raised;
  int screen_num;		/* Our screen number as dictated my X */
  Colormap def_cmap;
  Cursor rat;
  unsigned long fg_color, bg_color; /* The pixel color. */

  char *display_string;

  /* A list of frames that may or may not contain windows. There should
     always be one in the list. */
  struct list_head frames;

  /* Keep track of which numbers have been given to frames. */
  struct numset *frames_numset;

  /* The number of the currently focused frame. One for each screen so
     when you switch screens the focus doesn't get frobbed. */
  int current_frame;
};

struct rp_action
{
  KeySym key;
  unsigned int state;
  void *data;			/* misc data to be passed to the function */
/*   void (*func)(void *); */
};

struct rp_key
{
  KeySym sym;
  unsigned int state;
};

struct rp_defaults
{
  /* Default positions for new normal windows, transient windows, and
     normal windows with maxsize hints. */
  int win_gravity;
  int trans_gravity;
  int maxsize_gravity;

  int input_window_size;
  int window_border_width;

  int bar_x_padding;
  int bar_y_padding;
  int bar_location;
  int bar_timeout;
  int bar_border_width;
  
  int frame_indicator_timeout;
  int frame_resize_unit;

  int padding_left;
  int padding_right;
  int padding_top;
  int padding_bottom;

  XFontStruct *font;

  int wait_for_key_cursor;

  char *window_fmt;

  /* Which name to use: wm_name, res_name, res_class. */
  int win_name;

  int startup_message;

  /* Decides whether the window list is displayed in a row or a
     column. */
  int window_list_style;

  /* Pointer warping toggle. */
  int warp;
};

/* Information about a child process. */
struct rp_child_info
{
  /* The command that was executed. */
  char *cmd;

  /* PID of the process. */
  int pid;

  /* Return status when the child process finished. */
  int status;

  /* When this is != 0 then the process finished. */
  int terminated;

  /* This structure can exist in a list. */
  struct list_head node;
};

/* Each child process is stored in this list. spawn, creates a new
   entry in this list, the SIGCHLD handler sets child.terminated to be
   true and handle_signals in events.c processes each terminated
   process by printing a message saying the process ended and
   displaying it's exit code. */
extern struct list_head rp_children;

extern struct rp_defaults defaults;

/* The prefix key also known as the command character under screen. */
extern struct rp_key prefix_key;

/* A list of mapped windows. These windows show up in the window
   list and have a number assigned to them. */
extern struct list_head rp_mapped_window;

/* A list of unmapped windows. These windows do not have a number
   assigned to them and are not visible/active. */
extern struct list_head rp_unmapped_window;

extern int rp_current_screen;
extern rp_screen *screens;
extern int num_screens;

extern XEvent rp_current_event;

extern Display *dpy;
extern Atom rp_command;
extern Atom rp_command_request;
extern Atom rp_command_result;

extern Atom wm_name;
extern Atom wm_state;
extern Atom wm_change_state;
extern Atom wm_protocols;
extern Atom wm_delete;
extern Atom wm_take_focus;
extern Atom wm_colormaps;

/* mouse properties */
extern int rat_x;
extern int rat_y;
extern int rat_visible;

/* When unmapping or deleting windows, it is sometimes helpful to
   ignore a bad window when attempting to clean the window up. This
   does just that when set to 1 */
extern int ignore_badwindow;

/* Arguments passed to ratpoison. */
extern char **myargv;

/* These defines should be used to specify the modifier mask for keys
   and they are translated into the X11 modifier mask when the time
   comes to compare modifier masks. */
#define RP_CONTROL_MASK 1
#define RP_META_MASK 	2
#define RP_ALT_MASK 	4
#define RP_SUPER_MASK 	8
#define RP_HYPER_MASK 	16

struct modifier_info
{
/*   unsigned int mode_switch_mask; */
  unsigned int meta_mod_mask;
  unsigned int alt_mod_mask;
  unsigned int super_mod_mask;
  unsigned int hyper_mod_mask;

  /* Keep track of these because they mess up the grab and should be
     ignored. */  
  unsigned int num_lock_mask;
  unsigned int scroll_lock_mask; 
};

/* Keeps track of which mod mask each modifier is under. */
extern struct modifier_info rp_modifier_info;

/* nonzero if an alarm signal was raised. This means ratpoison should
   hide its popup windows. */
extern int alarm_signalled;
extern int kill_signalled;
extern int hup_signalled;
extern int chld_signalled;

/* rudeness levels */
extern int rp_honour_transient_raise;
extern int rp_honour_normal_raise;
extern int rp_honour_transient_map;
extern int rp_honour_normal_map;

/* Keep track of X11 error messages. */
extern char *rp_error_msg;

/* Number sets for windows. */
extern struct numset *rp_window_numset;

#endif /* _RATPOISON_DATA_H */
