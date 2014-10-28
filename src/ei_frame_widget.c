/**
 * @file	ei_frame_widget.c
 * @brief	Definition and fonctions of the class "frame"
 *
 * \author 
 * Equipe poulet
 * 06.2014
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ei_frame_widget.h"
#include "ei_draw_tools.h"



/**********************************************************************************/
/**********************************************************************************/



void* ei_frame_allocfunc()
{
    void* allocation = malloc(sizeof(ei_frame_widget_t));
    memset(allocation,0,sizeof(ei_frame_widget_t));
    return(allocation);
}



/****************************************************************************/
/****************************************************************************/



void ei_frame_releasefunc(struct ei_widget_t* widget)
{
    ei_frame_widget_t* frame_widget = (ei_frame_widget_t*) widget;
    if (frame_widget == NULL) {
        return;
    }

}



/****************************************************************************/
/****************************************************************************/



void ei_frame_setdefaultsfunc(struct ei_widget_t* widget)
{
    // Paramètres en tant que widget
    widget->requested_size.width  = 0;
    widget->requested_size.height = 0;
    // Paramètres en tant que frame widget
    ei_frame_widget_t* frame_widget = (ei_frame_widget_t*) widget;
    frame_widget->color = ei_default_background_color;
    frame_widget->border_width = 0;
    frame_widget->relief = ei_relief_none;
    frame_widget->text = NULL;
    frame_widget->text_font = ei_default_font;
    frame_widget->text_color = ei_font_default_color;
    frame_widget->text_anchor = ei_anc_center;
    frame_widget->img = NULL;
    frame_widget->img_rect = NULL;
    frame_widget->img_anchor = ei_anc_center;
}


void ei_root_frame_setdefaultsfunc(struct ei_widget_t* widget,
        ei_surface_t display_surface)
{
    // Paramètres en tant que widget
    ei_size_t main_size = hw_surface_get_size(display_surface);
    widget->requested_size.width = main_size.width;
    widget->requested_size.height = main_size.height;
    ei_rect_t main_rect = hw_surface_get_rect(display_surface);
    widget->screen_location = main_rect;
    widget->content_rect = &(widget->screen_location);
   
    // Paramètres en tant que frame widget
    ei_frame_widget_t* frame_widget = (ei_frame_widget_t*) widget;
    frame_widget->border_width = 0;
    frame_widget->relief = ei_relief_none;	
}



/****************************************************************************/
/****************************************************************************/



void ei_frame_drawfunc(	struct ei_widget_t*	widget,
        ei_surface_t		surface,
        ei_surface_t		pick_surface,
        ei_rect_t*			clipper			)
{
    ei_frame_widget_t* frame_widget = (ei_frame_widget_t*) widget;
    ei_color_t gris  = { 0x33, 0x33, 0x33, 0xAA };

    // Etape 1 : dessin du cadre de fond si on souhaite du relief

    if (frame_widget->border_width != 0) {

        // Couleurs utilisées dans le relief
        ei_color_t white = { 0xD0, 0xD0, 0xD0, 0xFF };
        ei_color_t noir  = { 0x30, 0x30, 0x30, 0xFF };
        ei_color_t relief_color = ei_clearer_color(frame_widget->color);
        ei_linked_point_t* cadre_bg =
            ei_frame_calculation(widget,frame_widget->border_width);

        // Différents cas de bordure
        switch (frame_widget->relief) {
            case (ei_relief_none): // Pas de bordure
                break;
            case (ei_relief_raised): // Bordure plus claire
                ei_draw_polygon(surface, cadre_bg, white, clipper);
                ei_draw_polygon(surface, cadre_bg, relief_color, clipper);
                ei_draw_polyline(surface, cadre_bg, gris, clipper);
                break;
            case (ei_relief_sunken): // Bordure plus foncée
                ei_draw_polygon(surface, cadre_bg, noir, clipper);
                ei_draw_polygon(surface, cadre_bg, relief_color, clipper);
                ei_draw_polyline(surface, cadre_bg, gris, clipper);
                break;
        }

        // Libération mémoire
        ei_free_linked_points_list(cadre_bg);

    }

    // Etape 2 : dessin du cadre de premier plan

    // Surface d'affichage
    ei_linked_point_t* cadre_fg = ei_frame_calculation(widget,0);
    ei_draw_polygon(surface, cadre_fg, frame_widget->color, clipper);
    ei_draw_polyline(surface, cadre_fg, gris, clipper);

    // Offsreen
    ei_color_t pick_pick = *(widget->pick_color);
    ei_draw_polygon(pick_surface, cadre_fg, pick_pick, clipper);

    // Libération mémoire
    ei_free_linked_points_list(cadre_fg);

    // Etape 3 : dessin du texte

    if (frame_widget->text != NULL) {
        ei_draw_widget_text(
                surface, widget, frame_widget->text, frame_widget->text_font,
                frame_widget->text_color, frame_widget->text_anchor);
    }

    // Etape 4 : chargement d'une image

    if (frame_widget->img != NULL) {
        ei_draw_widget_img( surface, widget, frame_widget->img,
                frame_widget->img_rect, frame_widget->img_anchor, 0 );
    }

}



/*****************************************************************************/
/*****************************************************************************/



void ei_frame_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect)
{
    widget->screen_location = rect;
}


