/**
 * @file	ei_geometrymanager.c
 * @brief	Manages the positionning and sizing of widgets on the screen.
 *
 * \author
 * Equipe poulet
 * 06.2014
 *
 */

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include "ei_geometrymanager.h"
#include "ei_application.h"
#include "ei_toplevel_widget.h"



// Macro utilisée dans la configuration du placeur
#define TEST_AND_SET(a,b) if (a != NULL) { b->a = *a; }
// When the argument "a" is passed as NULL,
// the placer uses the default value "default".
// When the argument "a" isn't passed as NULL,
// the placer replaces the previous value with "*a"


extern uint32_t titlebar_height;
extern ei_color_t titlebar_color;



// Définition de la classe "placer" dans les gestionnaires de géométrie
typedef struct ei_placer_param_t {
    ei_geometry_param_t geometry_param;
    ei_anchor_t anchor;
    int x;
    int y;
    int width;
    int height;	
    float rel_x;
    float rel_y;
    float rel_width;
    float rel_height;
} ei_placer_param_t;



/************************************************************************/
/************* LISTE GLOBALE DES GESTIONNAIRES DE GEOMETRIE *************/
/************************************************************************/



/* LISTE DEFINIE EN GLOBAL QUI STOCKE TOUTES LES CLASSES
 * DE GESTIONNAIRES DE GEOMETRIE */

ei_geometrymanager_t* global_geometry_register = NULL;
// La liste est initialement vide


/* FONCTIONS POUR LE TRAITEMENT DE LA LISTE GLOBALE */

// Insertion d'une classe en queue de la liste globale 
static void geo_insertion_tail(ei_geometrymanager_t* geometrymanager)
{
    // 1er cas : liste vide
    if (global_geometry_register == NULL) {
        global_geometry_register = geometrymanager;
        // 2nd cas : liste contenant au moins un élément
    } else {
        ei_geometrymanager_t* courant = global_geometry_register;
        // On se place en derniere position
        while (courant->next != NULL) {
            courant = courant->next;
        }
        // On chaîne le nouvel élément
        courant->next = geometrymanager;
    }
}


/* INSERTION DE NOUVEAUX GESTIONNAIRES DE GEOMETRIE DANS LA LISTE GLOBALE */

void ei_geometrymanager_register(ei_geometrymanager_t* geometrymanager)
{
    geo_insertion_tail(geometrymanager);
}



/************************************************************************/
/***** RETROUVER UN GESTIONNAIRE DE GEOMETRIE DANS LA LISTE GLOBALE *****/
/************************************************************************/



ei_geometrymanager_t* ei_geometrymanager_from_name(
        ei_geometrymanager_name_t name)
{
    // Pointeur courant initialisé au premier élément de la liste globale
    ei_geometrymanager_t* courant = global_geometry_register;
    // // Traitement du gestionnaire de géométrie courant
    while (courant != NULL)
    {
        uint32_t comparaison = strncmp(courant->name, name, 20);
        if (comparaison == 0)
        {
            return courant;
        }
        courant=courant->next;
    }
    return NULL;
}



/************************************************************************/
/******************* REMOVE A WIDGET FROM THE SCREEN ********************/
/************************************************************************/



void ei_geometrymanager_unmap(ei_widget_t* widget)
{

    // Vérification du widget passé en paramètre
    if (widget == NULL) {
        return;
    }

    // Définition des variables nécessaires
    // geo_param = pointeur vers le gestionnaire de géométrie
    ei_geometry_param_t* geo_param = widget->geom_params;

    // Si le widget n'est pas géré par un gestionnaire de géométrie
    // alors il n'est pas à l'écran, il n'y a rien à faire
    if (geo_param == NULL) {
        return;
    }

    // Maintenant on sait que le widget
    // est géré par un gestionnaire de géométrie
    ei_geometrymanager_t* manager = geo_param->manager;

	// Mise en mémoire du rectangle qu'occupait le widget 
	// avant d'être effacé de l'écran
    ei_rect_t* position_before = &widget->screen_location;

    // The "ei_geometrymanager_releasefunc_t" of the geometry manager
    // in charge of this widget is called
    // -> the "geom_param" field of the widget is freed
    manager->releasefunc(widget);


    // The current "screen_location" of the widget is invalided
    // (which will trigger a redraw)
    // -> il faut provoquer un rafraîchissement
    // pour qu'il n'apparaisse plus à l'écran
    ei_app_invalidate_rect(position_before);


    // The "screen_location" of the widget is reset to 0
    widget->screen_location.size.width	= 0;
    widget->screen_location.size.height	= 0;
    widget->screen_location.top_left.x	= 0;
    widget->screen_location.top_left.y	= 0;

}



/************************************************************************/
/******************* FONCTIONS SPECIFIQUES AU PLACEUR *******************/
/************************************************************************/



static void placer_setdefaultvalues(ei_widget_t* widget)
{
	ei_geometry_param_t* geopar = widget->geom_params;
	ei_placer_param_t* placeur = (ei_placer_param_t*) geopar;
	placeur->anchor = ei_anc_northwest;
	placeur->x = 0;
	placeur->y = 0;
    placeur->width = widget->requested_size.width;
    placeur->height = widget->requested_size.height;	
    placeur->rel_x = 0.0;
    placeur->rel_y = 0.0;
    placeur->rel_width = 0.0;
    placeur->rel_height = 0.0;
}    
    
void placer_releasefunc(ei_widget_t* widget)
{
    // Vérification du widget passé en paramètre
    if (widget == NULL) {
        return;
    }

    // Libération du champ "geo_params"
    if (widget->geom_params != NULL) {
        // Pas d'attributs spécifiques à desallouer
        widget->geom_params->manager = NULL;
        free(widget->geom_params);
        widget->geom_params = NULL;
    }
}



// Recalcule l'attribut screen_location du widget
// en fonction des paramètres du parent
void placer_runfunc(ei_widget_t* widget) 
{

    // Vérification du widget passé en paramètre
    if (widget == NULL) {
        return;
    }

    // Définition des variables nécessaires
    ei_widget_t* parent = widget->parent;
    ei_rect_t screenlocationfils = widget->screen_location;

    if (widget->parent == NULL) {

        // Cas de la Racine
        screenlocationfils.top_left.x = 0;
        screenlocationfils.top_left.y = 0;
        screenlocationfils.size = hw_surface_get_size(ei_app_root_surface());	

    } else {

        // Définition des variables nécessaires
        ei_rect_t screenlocationparent = parent->screen_location;
        ei_placer_param_t* placerparam =
            (ei_placer_param_t*)widget->geom_params;

        // Si le widget n'est pas géré par un gestionnaire de géometrie, 
        // Alors on ne recalcule pas l'attribut screen location
        if  (widget->geom_params == NULL ) {
            return;
        }

        // Traitement de "size"
        screenlocationfils.size.width =
            (placerparam->rel_width)*(screenlocationparent.size.width)
            + placerparam->width;
        screenlocationfils.size.height =
            (placerparam->rel_height)*(screenlocationparent.size.height)
            + placerparam->height;

        // Traitement de "topleft"
        switch (placerparam->anchor) {
            case ei_anc_none :
                break;
            case ei_anc_center : // Anchor in the center
                screenlocationfils.top_left.x = screenlocationparent.top_left.x
                    + screenlocationparent.size.width * placerparam->rel_x
                    + placerparam->x
                    - screenlocationfils.size.width/2;
                screenlocationfils.top_left.y = screenlocationparent.top_left.y
                    + screenlocationparent.size.height * placerparam->rel_y
                    + placerparam->y
                    - screenlocationfils.size.height/2;
                break;
            case ei_anc_north : // Anchor on the top side, centered horizontally
                screenlocationfils.top_left.x = screenlocationparent.top_left.x
                    + screenlocationparent.size.width * placerparam->rel_x
                    + placerparam->x
                    - screenlocationfils.size.width/2;
                screenlocationfils.top_left.y = screenlocationparent.top_left.y
                    + screenlocationparent.size.height * placerparam->rel_y
                    + placerparam->y;
                break;
            case ei_anc_northeast : // Anchor on the top-right corner
                screenlocationfils.top_left.x = screenlocationparent.top_left.x
                    + screenlocationparent.size.width * placerparam->rel_x
                    + placerparam->x
                    - screenlocationfils.size.width;
                screenlocationfils.top_left.y = screenlocationparent.top_left.y
                    + screenlocationparent.size.height * placerparam->rel_y
                    + placerparam->y;
                break;
            case ei_anc_east : // Anchor on the right side, centered vertically
                screenlocationfils.top_left.x = screenlocationparent.top_left.x
                    + screenlocationparent.size.width * placerparam->rel_x
                    + placerparam->x
                    - screenlocationfils.size.width;
                screenlocationfils.top_left.y = screenlocationparent.top_left.y
                    + screenlocationparent.size.height * placerparam->rel_y
                    + placerparam->y
                    - screenlocationfils.size.height/2;
                break;
            case ei_anc_southeast : // Anchor on the bottom-right corner
                screenlocationfils.top_left.x = screenlocationparent.top_left.x
                    + screenlocationparent.size.width
                    * placerparam->rel_x
                    + placerparam->x
                    - screenlocationfils.size.width;

                screenlocationfils.top_left.y = screenlocationparent.top_left.y
                    + screenlocationparent.size.height * placerparam->rel_y
                    + placerparam->y
                    - screenlocationfils.size.height;
                break;
            case ei_anc_south : 
                // Anchor on the bottom side, centered horizontally
                screenlocationfils.top_left.x = screenlocationparent.top_left.x
                    + screenlocationparent.size.width * placerparam->rel_x
                    + placerparam->x
                    - screenlocationfils.size.width/2;
                screenlocationfils.top_left.y = screenlocationparent.top_left.y
                    + screenlocationparent.size.height * placerparam->rel_y
                    + placerparam->y
                    - screenlocationfils.size.height;
                break;
            case ei_anc_southwest : //  Anchor on the bottom-left corner
                screenlocationfils.top_left.x = screenlocationparent.top_left.x
                    + screenlocationparent.size.width * placerparam->rel_x
                    + placerparam->x;
                screenlocationfils.top_left.y = screenlocationparent.top_left.y
                    + screenlocationparent.size.height * placerparam->rel_y
                    + placerparam->y
                    - screenlocationfils.size.height;
                break;
            case ei_anc_west : // Anchor on the left side, centered vertically
                screenlocationfils.top_left.x = screenlocationparent.top_left.x
                    + screenlocationparent.size.width * placerparam->rel_x
                    + placerparam->x;
                screenlocationfils.top_left.y = screenlocationparent.top_left.y
                    + screenlocationparent.size.height * placerparam->rel_y
                    + placerparam->y
                    - screenlocationfils.size.height/2;
                break;
            case ei_anc_northwest : // Anchor on the top-left corner
                screenlocationfils.top_left.x = screenlocationparent.top_left.x
                    + screenlocationparent.size.width * placerparam->rel_x
                    + placerparam->x;
                screenlocationfils.top_left.y = screenlocationparent.top_left.y
                    + screenlocationparent.size.height * placerparam->rel_y
                    + placerparam->y; 
                break;
        }
    }
    widget->screen_location = screenlocationfils;
    // widget->content_rect = &(widget->screen_location);
}




/************************************************************************/
/**** ENREGISTREMENT DES ATTRIBUTS DU PLACEUR DANS LA LISTE GLOBALE *****/
/************************************************************************/



void ei_register_placer_manager()
{
    // Définition du gestionnaire de géometrie
    ei_geometrymanager_t* placer = malloc(sizeof(ei_geometrymanager_t));
    // Définition des attributs du placeur
    strncpy(placer->name, "placer", 20);
    placer->runfunc = placer_runfunc;
    placer->releasefunc = placer_releasefunc;
    placer->next = NULL;
    // Insertion dans la liste globale
    ei_geometrymanager_register(placer);
}



/************************************************************************/
/* CONFIGURATION DU PLACEUR COMME GESTIONNAIRE DE GEOMETRIE D'UN WIDGET */
/************************************************************************/



// Configures the geometry of a widget using the "placer" geometry manager
void ei_place(	ei_widget_t* widget,
        ei_anchor_t* anchor,
        int* x,
        int* y,
        int* width,
        int* height,
        float* rel_x,
        float* rel_y,
        float* rel_width,
        float* rel_height)
{

    // Vérification du widget passé en paramètre
    if (widget == NULL) {
        return;
    }

    ei_placer_param_t* placer;

    // CAS 1 : the widget is not managed by a geometry manager
    if (widget->geom_params == NULL) {

        // On alloue de la mémoire pour le placeur 
        // on le définit comme le gestionnaire de géométrie du widget
        placer = malloc(sizeof(ei_placer_param_t));
        widget->geom_params = (ei_geometry_param_t*) placer;
        placer_setdefaultvalues(widget);

        // CAS 2 : the widget is already managed by a geometry manager
    } else {

        // Deux sous cas possibles :
        // - soit le widget est géré par le placeur
        // - soit le widget est géré par un autre gestionnaire de géométrie

        ei_geometrymanager_t* manager = widget->geom_params->manager;
        uint32_t comparaison = strncmp(manager->name, "placer", 20);

        // releasefunc

        // CAS 2.1 : the widget is already managed by the placer
        if (comparaison == 0) {

            placer = (ei_placer_param_t*) widget->geom_params;

            // CAS 2.2 : the widget is managed by another geometry manager
        } else {
            // The previous geometry manager is removed
            manager->releasefunc(widget);
            // On alloue de la mémoire pour le placeur et on le définit comme le
            // nouveau gestionnaire de géométrie du widget
            placer = malloc(sizeof(ei_placer_param_t));
            widget->geom_params = (ei_geometry_param_t*) placer;
            placer_setdefaultvalues(widget);
        }
    }

    // We update the placer parameters
    TEST_AND_SET(anchor,placer);
    TEST_AND_SET(x,placer);
    TEST_AND_SET(y,placer);
    TEST_AND_SET(width,placer);
    TEST_AND_SET(height,placer);
    TEST_AND_SET(rel_x,placer);
    TEST_AND_SET(rel_y,placer);
    TEST_AND_SET(rel_width,placer);
    TEST_AND_SET(rel_height,placer);

    // Appel à "runfunc" du placeur
    widget->geom_params->manager = ei_geometrymanager_from_name("placer");
    widget->geom_params->manager->runfunc(widget);


    /***********************************************************************/
    /* CONFIG ET APPEL DU PLACEUR POUR LES FILS OBLIGATOIRES D'UN TOPLEVEL */
    /***********************************************************************/
    uint32_t comp = strncmp(widget->wclass->name, "toplevel", 20);

    if (comp == 0) {

        // Configuration de ses fils

        // Déclaration des enfants obligatoires du toplevel
        ei_toplevel_widget_t* toplevel_widget = (ei_toplevel_widget_t*) widget;
        ei_widget_t* titlebar = widget->children_head;
        ei_widget_t* inside	= titlebar->next_sibling;
        ei_widget_t* resize = widget->children_tail;
        ei_widget_t* close = titlebar->children_head;

        // Configuration du titlebar
        ei_size_t titlebar_size;
        titlebar_size.width = widget->screen_location.size.width;
        titlebar_size.height = titlebar_height;
        int titlebar_border = toplevel_widget->border_width;
        ei_relief_t titlebar_relief = ei_relief_none;
        ei_color_t ei_font_default_color_without_const =
            (ei_color_t)ei_font_default_color;
        ei_frame_configure(titlebar, &titlebar_size, &titlebar_color,
                &titlebar_border, &titlebar_relief,
                &toplevel_widget->title, NULL,
                &ei_font_default_color_without_const, NULL,NULL, NULL, NULL);
        ei_place(titlebar,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

        // Configuration de l'inside
        ei_size_t inside_size;
        inside_size.width = widget->screen_location.size.width;
        inside_size.height = widget->screen_location.size.width-titlebar_height;
        ei_color_t inside_color = toplevel_widget->color;
        int inside_border = toplevel_widget->border_width;
        ei_relief_t inside_relief = ei_relief_none;
        ei_frame_configure(inside, &inside_size, &inside_color,
                &inside_border, &inside_relief,
                NULL, NULL, NULL, NULL,NULL, NULL, NULL);
        int y = titlebar_height;
        ei_place(inside, NULL, NULL, &y, &inside_size.width,
                &inside_size.height, NULL, NULL, NULL, NULL);


        // Configuration de resize
        if (toplevel_widget->resizable != ei_axis_none) {
            ei_relief_t	resize_relief = ei_relief_none;
            int	resize_border = 0;
            ei_frame_configure(resize, NULL, &titlebar_color,
                    &resize_border, &resize_relief,
                    NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            ei_anchor_t anc = ei_anc_southeast;
            int resize_width = 12;
            int resize_height = 12;
            float rel_x = 1.0;
            float rel_y = 1.0;
            ei_place(resize, &anc, NULL, NULL, &resize_width,
                    &resize_height, &rel_x, &rel_y, NULL, NULL);
        }

        // Configuration de close (bouton de fermeture)
        if (toplevel_widget->closable == EI_TRUE) {
            ei_color_t	close_color = {0xff, 0x00, 0x00, 0xff};
            int close_border = 0;
            ei_relief_t	close_relief = ei_relief_raised;
            ei_callback_t button_press = ei_toplevel_close;
            char* close_text = "×";
            ei_color_t black = {0x00, 0x00, 0x00, 0xFF};
            ei_anchor_t text_anc = ei_anc_south;
            ei_button_configure(close, NULL, &close_color,
                    &close_border, NULL, &close_relief,
                    &close_text, NULL, &black, &text_anc, NULL, NULL, NULL,
                    &button_press, NULL);
            int	close_width  = 22;
            int	close_height = 22;
            int close_x = 2 * toplevel_widget->border_width; 
            int close_y = 2 * toplevel_widget->border_width;
            ei_place(close, NULL, &close_x, &close_y, &close_width,
                    &close_height, NULL, NULL, NULL, NULL);
        }
    }
}


