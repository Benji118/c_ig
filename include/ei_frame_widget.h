/**
 * @file	ei_frame_widget.h
 * @brief	Definition and fonctions of the class "frame"
 *
 * \author 
 * Equipe poulet
 * 06.2014
 *
 */

#ifndef EI_FRAME_WIDGET_H
#define EI_FRAME_WIDGET_H

#include "ei_widgetclass.h"
#include "ei_widget.h"



/* Definition of the widget class "frame" */



typedef struct ei_frame_widget_t {
    // Premier champ = le widget
    ei_widget_t		widget;
    // Attributs spécifiques
    ei_color_t		color;
    int				border_width;
    ei_relief_t		relief;
    char*			text;
    ei_font_t		text_font;
    ei_color_t		text_color;
    ei_anchor_t		text_anchor;
    ei_surface_t	img;
    ei_rect_t*		img_rect;
    ei_anchor_t		img_anchor;
} ei_frame_widget_t;



/* Fonctions for the widget class "frame" */



/**
 * \brief A function that allocates a block of memory that is big enough to store
 * the attributes of the frame widget. After allocation, the function initialize
 * the memory to 0.
 *
 * @return A block of memory with all bytes set to 0.
 */
extern void* ei_frame_allocfunc();



/**
 * \brief A function that releases the memory used by a widget before it is destroyed.
 * The \ref ei_widget_t structure itself, passed as parameter, isn't freed by these
 * functions.
 *
 * @param widget The widget which resources are to be freed.
 */
extern void ei_frame_releasefunc(struct ei_widget_t* widget);



/**
 * \brief A function that sets the default values for the class "frame".
 *
 * @param widget A pointer to the widget instance to intialize.
 */
extern void ei_frame_setdefaultsfunc(struct ei_widget_t* widget);



/* Initialization for the root widget (of class frame) */
extern void ei_root_frame_setdefaultsfunc(struct ei_widget_t* widget, ei_surface_t display_surface);



/**
 * \brief A function that draws widgets of the class "frame"
 *
 * @param widget A pointer to the widget instance to draw.
 * @param surface Where to draw the widget. The actual location of the widget in the
 * surface is stored in its "screen_location" field.
 * @param clipper If not NULL, the drawing is restricted within this rectangle
 * (expressed in the surface reference frame).
 */
extern void ei_frame_drawfunc(	struct ei_widget_t*	widget,
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
extern void ei_frame_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect);


#endif
