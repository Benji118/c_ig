/**
 *  @file	ei_mouse.h
 *  @brief	About the mouse.
 *
 *  \author
 *  Poulet
 *  06.2014
 *
 */
#ifndef EI_MOUSE_H
#define EI_MOUSE_H

#include <stdlib.h>

#include "ei_types.h"
#include "ei_application.h"

// is mouse down ?
ei_bool_t mouse_down;

// coordinates when mouse button was pressed
ei_point_t mouse_down_coordinates;

// pick value of the current held widget
uint32_t current_id;

// defines which toplevel has the focus
uint32_t current_focus;

ei_bool_t get_mouse_down();

void set_mouse_down(ei_bool_t down);

ei_point_t get_mouse_down_coordinates();

void set_mouse_down_coordinates(ei_point_t coordinates);

uint32_t get_current_id();

void set_current_id(uint32_t pick_id);

uint32_t get_current_focus();

void set_current_focus(uint32_t pick_id);

// Procédure récursive
// Requiert : un entier positif n
// Retourne : - le widget de l'arbre de descendance de "widget"
//				identifié par cet entier s'il existe
//			  - NULL sinon
ei_bool_t find_widget_from_pick_id(ei_widget_t* widget, uint32_t n,
        ei_widget_t** solution);

ei_widget_t* get_focused_widget();

#endif
