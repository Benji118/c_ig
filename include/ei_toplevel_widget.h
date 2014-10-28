/**
 * @file	ei_toplevel_widget.h
 * @brief	Definition and fonctions of the class "toplevel"
 *
 * \author 
 * Equipe poulet
 * 06.2014
 *
 */

#ifndef EI_TOPLEVEL_WIDGET_H
#define EI_TOPLEVEL_WIDGET_H

#include "ei_widgetclass.h"
#include "ei_widget.h"
#include "ei_event.h"



/* Definition of the widget class "toplevel" */



typedef struct ei_toplevel_widget_t {
    // Premier champ = le widget
    ei_widget_t		widget;
    // Attributs spécifiques
    ei_color_t		color;
    int				border_width;
    char*			title;
    ei_bool_t		closable;
    ei_axis_set_t	resizable;
    ei_size_t*		min_size;
} ei_toplevel_widget_t;



/* Fonctions for the widget class "toplevel" */



/**
 * \brief A function that allocates a block of memory that is big enough to store
 * the attributes of the toplevel widget. After allocation, the function initialize
 * the memory to 0.
 *
 * @return A block of memory with all bytes set to 0.
 */
extern void* ei_toplevel_allocfunc();



/**
 * \brief A function that releases the memory used by a widget before it is destroyed.
 * The \ref ei_widget_t structure itself, passed as parameter, isn't freed by these
 * functions.
 *
 * @param widget The widget which resources are to be freed.
 */
extern void ei_toplevel_releasefunc(struct ei_widget_t* widget);



/**
 * \brief A function that sets the default values for the class "toplevel".
 *
 * @param widget A pointer to the widget instance to intialize.
 */
extern void ei_toplevel_setdefaultsfunc(struct ei_widget_t* widget);



/**
 * \brief A function that draws widgets of the class "toplevel".
 *
 * @param widget A pointer to the widget instance to draw.
 * @param surface Where to draw the widget. The actual location of the widget in the
 * surface is stored in its "screen_location" field.
 * @param clipper If not NULL, the drawing is restricted within this rectangle
 * (expressed in the surface reference frame).
 */
extern void ei_toplevel_drawfunc(	struct ei_widget_t*	widget,
        ei_surface_t		surface,
        ei_surface_t		pick_surface,
        ei_rect_t*			clipper);



/**
 * \brief A function that is called to notify the widget that its geometry has been
 * modified by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *
 * @param widget The widget instance to notify of a geometry change.
 * @param rect The new rectangular screen location of the widget
 * (i.e. = widget->screen_location).
 */
extern void ei_toplevel_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect);



/* Fonctions pour les évènements toplevel */

/**
 * Action : invalidates the whole root widget (in order to bypass
 * the border) and destroys the toplevel widget.
 */
extern ei_bool_t ei_toplevel_close(ei_widget_t* widget, ei_event_t* event, void* user_param);

/* Action : gère le déplacement des toplevels */
extern ei_bool_t ei_toplevel_drag(ei_widget_t* widget, ei_event_t* event, void* user_param);

/* Action : gère le redimensionnement des toplevels */
extern ei_bool_t ei_toplevel_resize(ei_widget_t* widget, ei_event_t* event, void* user_param);

/* Action : handles the keypresses */
extern ei_bool_t ei_toplevel_keys(ei_widget_t* widget, ei_event_t* event, void* user_param);

/* Action : gives focus to the targeted toplevel (or to the root widget) */
extern ei_bool_t ei_toplevel_focus(ei_widget_t* widget, ei_event_t* event, void* user_param);


#endif
