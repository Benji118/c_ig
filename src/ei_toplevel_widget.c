/*
 * @file	ei_toplevel_widget.c
 * @brief	Definition and fonctions of the class "toplevel"
 *
 * \author 
 * Equipe poulet
 * 06.2014
 *
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "ei_toplevel_widget.h"
#include "ei_draw_tools.h"
#include "ei_application.h"
#include "ei_mouse.h"
#include "ei_geometrymanager.h"



/* Variables globales pour paramétrer les toplevels */
uint32_t titlebar_height = 50;
ei_color_t titlebar_color = {0x44, 0x44, 0x77, 0xff};



/*****************************************************************************/
/*****************************************************************************/



void* ei_toplevel_allocfunc()
{
    void* allocation = malloc(sizeof(ei_toplevel_widget_t));
    memset(allocation,0,sizeof(ei_toplevel_widget_t));
    return(allocation);
}



/*****************************************************************************/
/*****************************************************************************/



void ei_toplevel_releasefunc(struct ei_widget_t* widget)
{
    ei_toplevel_widget_t* top_widget = (ei_toplevel_widget_t*) widget;
    if (top_widget == NULL) {
        return;
    }
    if (top_widget->min_size != NULL) {
        free(top_widget->min_size);
    }
}



/*****************************************************************************/
/*****************************************************************************/



void ei_toplevel_setdefaultsfunc(struct ei_widget_t* widget)
{
    widget->requested_size.width = 320;
    widget->requested_size.height = 240;
    ei_toplevel_widget_t* toplevel_widget = (ei_toplevel_widget_t*) widget;
    toplevel_widget->color = ei_default_background_color;
    toplevel_widget->border_width = 4;
    toplevel_widget->title = "Toplevel";
    toplevel_widget->closable = EI_TRUE;
    toplevel_widget->resizable = ei_axis_both;
    toplevel_widget->min_size = malloc(sizeof(ei_size_t));
    toplevel_widget->min_size->width = 160;
    toplevel_widget->min_size->height = 120;
}



/*****************************************************************************/
/*****************************************************************************/



void ei_toplevel_drawfunc(	struct ei_widget_t* widget,
        ei_surface_t surface,
        ei_surface_t pick_surface,
        ei_rect_t* clipper			)
{

}



/*****************************************************************************/
/*****************************************************************************/



void ei_toplevel_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect)
{
	// Parcours d'arbre
   //widget->screen_location = rect;
}



static void ei_geomnotify_family(ei_widget_t* widget)
{
	if (widget == NULL) {
		return;
	}
	ei_place(widget,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
	ei_geomnotify_family(widget->children_head);
	ei_geomnotify_family(widget->next_sibling);
}



/*****************************************************************************/
/*****************************************************************************/


// Requiert : un pointeur sur un widget
// Retourne :	- son premier ancêtre de classe toplevel s'il existe
//				- la racine sinon
static ei_widget_t* ei_get_toplevel_parent(ei_widget_t* widget)
{
    ei_widget_t* handle = widget;
    while (handle->pick_id != ei_app_root_widget()->pick_id) {
        if (!strncmp(handle->wclass->name, "toplevel", 20)) {
            return handle;
        }
        handle = handle->parent;
    }
    return ei_app_root_widget();
}


/**
 * Action : invalidates the whole root widget (in order to bypass
 * the border) and destroys the toplevel widget.
 */
ei_bool_t ei_toplevel_close(ei_widget_t* widget, ei_event_t* event,
        void* user_param)
{
    if (ei_get_toplevel_parent(widget)->pick_id
            == ei_app_root_widget()->pick_id) {
        return EI_FALSE;
    }
    ei_app_invalidate_rect(&ei_app_root_widget()->screen_location);
    ei_widget_destroy(ei_get_toplevel_parent(widget));
    return EI_TRUE;
}


/* Action : gère le déplacement des toplevels */
ei_bool_t ei_toplevel_drag(ei_widget_t* widget, ei_event_t* event,
        void* user_param)
{
    // Ce n'est pas le widget racine
    if (widget->pick_id == ei_app_root_widget()->pick_id) {
        return EI_FALSE;
    }
    // C'est un frame (titlebar de classe frame)
    if (strncmp(widget->wclass->name, "frame", 20)) {
        return EI_FALSE;
    }
    // Son parent est un toplevel
    if (strncmp(widget->parent->wclass->name, "toplevel", 20)) {
        return EI_FALSE;
    }
    // Il est le premier fils de son parent (construction des toplevels)
    if (widget->parent->children_head->pick_id != widget->pick_id) {
        return EI_FALSE;
    }
    // Maintenant le widget pris en argument est une barre de titre
    // d'une fenetre
    if (get_mouse_down() && (get_current_id() == widget->pick_id)) {
        ei_rect_t clipped_rect =
            ei_rectangles_intersect(*widget->parent->content_rect,
                    ei_app_root_widget()->screen_location);
                    
        ei_app_invalidate_rect(&clipped_rect);
        widget->parent->screen_location.top_left.x -=
            (get_mouse_down_coordinates().x
             - event->param.mouse.where.x);
        widget->parent->screen_location.top_left.y -=
            (get_mouse_down_coordinates().y
             - event->param.mouse.where.y);
             
        ei_geomnotify_family(widget);
        set_mouse_down_coordinates(event->param.mouse.where);
        return EI_TRUE;
    }
    return EI_FALSE;
}


/* Action : gère le redimensionnement des toplevels */
ei_bool_t ei_toplevel_resize(ei_widget_t* widget, ei_event_t* event,
        void* user_param)
{
    // Ce n'est pas le widget racine
    if (widget->pick_id == ei_app_root_widget()->pick_id) {
        return EI_FALSE;
    }
    // C'est un frame (titlebar de classe frame)
    if (strncmp(widget->wclass->name, "frame", 20)) {
        return EI_FALSE;
    }
    // Son parent est un toplevel
    if (strncmp(widget->parent->wclass->name, "toplevel", 20)) {
        return EI_FALSE;
    }
    ei_widget_t* toplevel = widget->parent;
    ei_widget_t* titlebar = toplevel->children_head;

    // Il est le premier fils de son parent (construction des toplevels)
    if (toplevel->children_tail->pick_id != widget->pick_id) {
        return EI_FALSE;
    }
    // Maintenant le widget pris en argument est une barre de titre
    // d'une fenetre
    if (get_mouse_down() && (get_current_id() == widget->pick_id)) {
        ei_rect_t clipped_rect =
            ei_rectangles_intersect(*toplevel->content_rect,
                    ei_app_root_widget()->screen_location);
        ei_app_invalidate_rect(&clipped_rect);

        toplevel->screen_location.size.width -=
            (get_mouse_down_coordinates().x - event->param.mouse.where.x);
        toplevel->screen_location.size.width =
            fmax(toplevel->screen_location.size.width,
                    ((ei_toplevel_widget_t*)(toplevel))->min_size->width);
        toplevel->screen_location.size.height -=
            (get_mouse_down_coordinates().y - event->param.mouse.where.y);
        toplevel->screen_location.size.height =
            fmax(toplevel->screen_location.size.height,
                   ((ei_toplevel_widget_t*)(toplevel))->min_size->height);


        titlebar->screen_location.size.width = toplevel->screen_location.size.width;
        titlebar->next_sibling->screen_location.size.width = toplevel->screen_location.size.width;
        titlebar->next_sibling->screen_location.size.height = toplevel->screen_location.size.height;


        ei_geomnotify_family(widget);
        set_mouse_down_coordinates(event->param.mouse.where);
        return EI_TRUE;
    }
    return EI_FALSE;
}


/* Action : handles the keypresses */
ei_bool_t ei_toplevel_keys(ei_widget_t* widget, ei_event_t* event,
        void* user_param)
{
    uint32_t    cmd_mask = (1 << ei_mod_ctrl_left) | (1 << ei_mod_ctrl_right);

    if (event->param.key.modifier_mask & cmd_mask) {
        if (event->param.key.key_sym == SDLK_w) {
            ei_toplevel_close(get_focused_widget(), event, user_param);
            return EI_TRUE;
        }
    }
    return EI_FALSE;
}



/* Action : gives focus to the targeted toplevel (or to the root widget) */
ei_bool_t ei_toplevel_focus(ei_widget_t* widget, ei_event_t* event,
        void* user_param)
{
    set_current_focus(ei_get_toplevel_parent(widget)->pick_id);
    return EI_TRUE;
}


