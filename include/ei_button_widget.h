/**
 * @file	ei_button_widget.h
 * @brief	Definition and fonctions of the class "button"
 *
 * \author 
 * Equipe poulet
 * 06.2014
 *
 */

#ifndef EI_BUTTON_WIDGET_H
#define EI_BUTTON_WIDGET_H

#include "ei_widgetclass.h"
#include "ei_widget.h"
#include "ei_event.h"



/* Definition of the widget class "button" */



typedef struct ei_button_widget_t {
    // Premier champ = le widget
    ei_widget_t		widget;	
    // Attributs spécifiques
    ei_color_t		color;
    int				border_width;
    int				corner_radius;
    ei_relief_t		relief;
    char*			text;
    ei_font_t		text_font;
    ei_color_t		text_color;
    ei_anchor_t		text_anchor;
    ei_surface_t	img;
    ei_rect_t*		img_rect;
    ei_anchor_t		img_anchor;
    ei_callback_t	callback;
    void*			user_param;
} ei_button_widget_t;



/* Fonctions for the widget class "button" */



/**
 * \brief A function that allocates a block of memory that is big enough to store
 * the attributes of the button widget. After allocation, the function initialize
 * then memory to 0.
 *
 * @return A block of memory with all bytes set to 0.
 */
extern void* ei_button_allocfunc();



/**
 * \brief A function that releases the memory used by a button before it is destroyed.
 * The \ref ei_widget_t structure itself, passed as parameter, isn't freed by these
 * functions.
 *
 * @param widget The widget which resources are to be freed.
 */
extern void ei_button_releasefunc(struct ei_widget_t* widget);



/**
 * \brief A function that sets the default values for the class "button".
 *
 * @param widget A pointer to the widget instance to intialize.
 */
extern void ei_button_setdefaultsfunc(struct ei_widget_t* widget);



/**
 * \brief A function that draws widgets of the class "button".
 *
 * @param widget A pointer to the widget instance to draw.
 * @param surface Where to draw the widget. The actual location of the widget in the
 * surface is stored in its "screen_location" field.
 * @param clipper If not NULL, the drawing is restricted within this rectangle
 * (expressed in the surface reference frame).
 */
extern void ei_button_drawfunc(	struct ei_widget_t*	widget,
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
extern void ei_button_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect);



/* Fonctions pour les évènements bouton */

/* Action : remet un bouton en position élevée */
extern ei_bool_t ei_raise_button(ei_widget_t* widget, ei_event_t* event, void* user_param);

/* Action : remet un bouton en position enfoncée */
extern ei_bool_t ei_sink_button(ei_widget_t* widget, ei_event_t* event, void* user_param);

/* Action : enfonce ou élève le bouton selon l'état de la souris */
extern ei_bool_t ei_toggle_relief(ei_widget_t* widget, ei_event_t* event, void* user_param);

/* Action : élève tous les boutons */
extern ei_bool_t ei_reset_all_buttons(ei_widget_t* widget, ei_event_t* event, void* user_param);

/* Action : appelle le callback lié au bouton */
extern ei_bool_t ei_button_click(ei_widget_t* widget, ei_event_t* event, void* user_param);



#endif
