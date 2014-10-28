/**
 *  @file	ei_mouse.c
 *  @brief	About the mouse.
 *
 *  \author
 *  Poulet
 *  06.2014
 *
 */

#include "ei_mouse.h"

ei_bool_t get_mouse_down()
{
    return mouse_down;
}

void set_mouse_down(ei_bool_t down)
{
    mouse_down = down;
}

ei_point_t get_mouse_down_coordinates()
{
    return mouse_down_coordinates;
}

void set_mouse_down_coordinates(ei_point_t coordinates)
{
    mouse_down_coordinates = coordinates;
}

uint32_t get_current_id()
{
    return current_id;
}

void set_current_id(uint32_t pick_id)
{
    current_id = pick_id;
}

uint32_t get_current_focus()
{
    return current_focus;
}

void set_current_focus(uint32_t pick_id)
{
    current_focus = pick_id;
}

ei_bool_t find_widget_from_pick_id(ei_widget_t* widget, uint32_t n,
        ei_widget_t** solution)
{
    if (widget == NULL) {
        return(EI_FALSE);
    }
    // 1er cas : "widget" est identifié par n
    if (widget->pick_id == n) {
        *solution = widget;
        return(EI_TRUE);
    }
    // 2nd cas : récursion sur les fils de "widget"
    ei_bool_t res = EI_FALSE;
    ei_widget_t* fils_courant = widget->children_head;
    while ( (fils_courant != NULL) && (res == EI_FALSE) ) {
        res = find_widget_from_pick_id(fils_courant,n,solution);
        // Appel récursif
        fils_courant = fils_courant->next_sibling;
    }
    return res;
}

ei_widget_t* get_focused_widget()
{
	ei_widget_t* widget;
	find_widget_from_pick_id(ei_app_root_widget(), get_current_focus(), &widget);
	return widget;
}


