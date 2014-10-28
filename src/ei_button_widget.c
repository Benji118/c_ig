/**
 * @file	ei_button_widget.c
 * @brief	Definition and fonctions of the class "button"
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

#include "ei_button_widget.h"
#include "ei_draw_tools.h"
#include "ei_application.h"
#include "ei_mouse.h"



/*****************************************************************************/
/*****************************************************************************/



void* ei_button_allocfunc()
{
    void* allocation = malloc(sizeof(ei_button_widget_t));
    memset(allocation,0,sizeof(ei_button_widget_t));
    return(allocation);
}



/*****************************************************************************/
/*****************************************************************************/



void ei_button_releasefunc(struct ei_widget_t* widget)
{
    ei_button_widget_t* button_widget = (ei_button_widget_t*) widget;
    if (button_widget == NULL) {
        return;
    }
    if (button_widget->img_rect != NULL) {
    	free(button_widget->img_rect);
    }
}



/*****************************************************************************/
/*****************************************************************************/



void ei_button_setdefaultsfunc(struct ei_widget_t* widget)
{
    // Initialisation du champ requested_size
    widget->requested_size.width = 0;
    widget->requested_size.height = 0;
    // Initialisation des attributs spécifiques au bouton
    ei_button_widget_t* button_widget = (ei_button_widget_t*) widget;
    button_widget->color = ei_default_background_color;
    button_widget->border_width = 0;
    button_widget->corner_radius = k_default_button_corner_radius;
    button_widget->relief = ei_relief_none;
    button_widget->text = NULL;
    button_widget->text_font = ei_default_font;
    button_widget->text_color = ei_font_default_color;
    button_widget->text_anchor = ei_anc_center;
    button_widget->img = NULL;
    button_widget->img_rect = NULL;
    button_widget->img_anchor = ei_anc_center;
    button_widget->callback = NULL;
    button_widget->user_param = NULL;
}



/*****************************************************************************/
/*****************************************************************************/



// Fonction statique qui contrôle le dessin d'un bouton en relief
static void ei_draw_relief_button(	struct ei_widget_t*	widget,
        ei_surface_t		surface,
        ei_surface_t		pick_surface,
        ei_rect_t*			clipper			)
{
    ei_button_widget_t* button_widget = (ei_button_widget_t*) widget;

    // Couleurs utilisées dans le dessin du relief
    ei_color_t gray  = {0x33, 0x33, 0x33, 0xAA};
    ei_color_t white = {0xD0, 0xD0, 0xD0, 0xFF};
    ei_color_t black = {0x30, 0x30, 0x30, 0xFF};
    ei_color_t relief_color = ei_clearer_color(button_widget->color);

    // Autres données utilisées
    ei_rect_t rect	= widget->screen_location;
    int border		= button_widget->border_width;
    int corner		= button_widget->corner_radius;

    // Dessin du relief
    if (button_widget->border_width != 0) {
        // Construction des listes de points nécessaires
        ei_linked_point_t* cadre_bg_high = ei_curvy_contour_button(
                EI_TRUE, EI_FALSE, rect, corner, 0);
        ei_linked_point_t* cadre_bg_low  = ei_curvy_contour_button(
                EI_FALSE, EI_TRUE, rect, corner, 0);

        switch (button_widget->relief) { 
            case ei_relief_none : // Pas de relief
                break;
            case ei_relief_raised: // Effet surélévé
                ei_draw_polygon(surface, cadre_bg_high, white, clipper);
                ei_draw_polygon(surface, cadre_bg_high, relief_color, clipper);
                ei_draw_polygon(surface, cadre_bg_low, black, clipper);
                ei_draw_polygon(surface, cadre_bg_low, relief_color, clipper);
                ei_draw_polyline(surface, cadre_bg_low, gray, clipper);
                ei_draw_polyline(surface, cadre_bg_high, gray, clipper);
                break;
            case ei_relief_sunken: // Effet enfoncé
                ei_draw_polygon(surface, cadre_bg_high, black, clipper);
                ei_draw_polygon(surface, cadre_bg_high, relief_color, clipper);
                ei_draw_polygon(surface, cadre_bg_low, white, clipper);
                ei_draw_polygon(surface, cadre_bg_low, relief_color, clipper);
                ei_draw_polyline(surface, cadre_bg_low, gray, clipper);
                ei_draw_polyline(surface, cadre_bg_high, gray, clipper);
                break;
        }

        // Libération de la mémoire
        ei_free_linked_points_list(cadre_bg_high);
        ei_free_linked_points_list(cadre_bg_low);

    }

    // Dessin de l'intérieur du bouton
    ei_linked_point_t* cadre_fg = ei_curvy_contour_button(
            EI_TRUE, EI_TRUE, rect, corner, border);
    ei_draw_polygon(surface, cadre_fg, button_widget->color, clipper);
    ei_draw_polyline(surface, cadre_fg, gray, clipper);

    // Dessin de l'intérieur du bouton dans l'offscreen
    ei_color_t pick_pick = *(widget->pick_color);	
    ei_draw_polygon(pick_surface, cadre_fg, pick_pick, clipper);

    // Libération de la mémoire
    ei_free_linked_points_list(cadre_fg);

}



void ei_button_drawfunc(struct ei_widget_t*	widget,
        ei_surface_t		surface,
        ei_surface_t		pick_surface,
        ei_rect_t*			clipper			)
{
    ei_button_widget_t* button_widget = (ei_button_widget_t *) widget;
    // Etape 1 : Dessin du bouton
    ei_draw_relief_button(widget,surface,pick_surface,clipper);
    // Etape 2 : chargement d'une image
    if (button_widget->img != NULL) {
        ei_draw_widget_img(surface, widget, button_widget->img,
                button_widget->img_rect,
                button_widget->img_anchor, button_widget->border_width);
    }
    // Etape 3 : écriture du texte
    if (button_widget->text != NULL){
        ei_draw_widget_text(surface, widget, button_widget->text,
                button_widget->text_font,
                button_widget->text_color, button_widget->text_anchor);
    }
}



/*****************************************************************************/
/*****************************************************************************/



void ei_button_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect)
{

}



/*****************************************************************************/
/*****************************************************************************/


/* Action : remet un bouton en position élevée */
ei_bool_t ei_raise_button(ei_widget_t* widget, ei_event_t* event,
        void* user_param)
{
    ((ei_button_widget_t*)widget)->relief = ei_relief_raised;
    return EI_TRUE;
}


/* Action : remet un bouton en position enfoncée */
ei_bool_t ei_sink_button(ei_widget_t* widget, ei_event_t* event,
        void* user_param)
{
    ((ei_button_widget_t*)widget)->relief = ei_relief_sunken;
    return EI_TRUE;
}


/* Action : enfonce ou élève le bouton selon l'état de la souris */
ei_bool_t ei_toggle_relief(ei_widget_t* widget, ei_event_t* event,
        void* user_param)
{
    if ( get_mouse_down() && ( get_current_id() == widget->pick_id ) ) {
        return ei_sink_button(widget,event,user_param);
    } else {
        return ei_raise_button(widget,event,user_param);
    }
}


static ei_bool_t ei_reset_all_buttons_rec(ei_widget_t* widget)
{
    if (widget == NULL) {
        return EI_TRUE;
    }

    if ( !(strncmp(widget->wclass->name, "button", 20)) ) {
        ei_raise_button(widget,NULL,NULL);
    }

    ei_reset_all_buttons_rec(widget->children_head);
    ei_reset_all_buttons_rec(widget->next_sibling);

    return EI_TRUE;
}


/* Action : élève tous les boutons */
ei_bool_t ei_reset_all_buttons(ei_widget_t* widget, ei_event_t* event,
        void* user_param)
{
    ei_reset_all_buttons_rec(ei_app_root_widget());
    return EI_TRUE;
}


/* Action : appelle le callback lié au bouton */
ei_bool_t ei_button_click(ei_widget_t* widget, ei_event_t* event,
        void* user_param)
{
    if (strncmp(widget->wclass->name, "button", 20)) {
        return EI_FALSE;
    }

    if (get_current_id() != widget->pick_id) {
        return EI_FALSE;
    }

    ei_button_widget_t* button = (ei_button_widget_t*) widget;
    button->callback(widget,event,button->user_param);
    return EI_TRUE;
}


