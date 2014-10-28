/**
 * @file	ei_application.c
 * @brief	Manages the main steps of a graphical application :
 *			- initialization
 *			- main window
 *			- main loop
 *			- quitting
 *			- resource freeing
 *
 * \author 
 * Equipe poulet
 * 06.2014
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ei_application.h"

#include "ei_geometrymanager.h"
#include "ei_draw_tools.h"

#include "ei_frame_widget.h"
#include "ei_button_widget.h"
#include "ei_toplevel_widget.h"

#include "ei_event.h"
#include "ei_mouse.h"
#include "ei_bindinglist.h"



/****************************************************************************/
/**************************** VARIABLES GLOBALES ****************************/
/****************************************************************************/



// L'UNIQUE SURFACE AFFICHEE A L'ECRAN
static ei_surface_t display_surface;

// LA SURFACE PRINCIPALE DE L'OFFSCRREN
ei_surface_t offscreen_surface;

// L'UNIQUE WIDGET RACINE (DE CLASSE FRAME)
static ei_widget_t* root_widget;

// LISTE DES RECTANGLES A METTRE A JOUR
static ei_linked_rect_t* rects_to_update = NULL;

// BOOLEEN POUR LA SORTIE DE LA BOUCLE PRINCIPALE
// -> on reste dans la boucle tant que (run_app==EI_TRUE)
static ei_bool_t run_app = EI_TRUE;



/****************************************************************************/
/************************ CREATION OF AN APPLICATION ************************/
/****************************************************************************/



// Creates an application
void ei_app_create(ei_size_t* main_window_size, ei_bool_t fullscreen)
{
    // Initialization of the hardware
    hw_init();

    // Registration of all classes of widget
    ei_frame_register_class();
    ei_button_register_class();
    ei_toplevel_register_class();

    // Registration of all geometry managers
    ei_register_placer_manager();

    // Creation of the root window (in a system window or in the entire screen) 
    display_surface = hw_create_window(main_window_size, fullscreen);

    // Creation of the main offscreen surface
    offscreen_surface = hw_surface_create(display_surface, main_window_size, 
						EI_TRUE);

    // Creation of the root widget
    root_widget = ei_widget_create("frame",NULL);
}



/****************************************************************************/
/******************** GESTION DE LA LISTE DE MISE A JOUR ********************/
/****************************************************************************/



// Insertion d'un rectangle en queue de la liste globale
static void insertion_tail(ei_linked_rect_t* new_rect)
{
    // 1er cas : liste vide
    if (rects_to_update == NULL) {
        rects_to_update = new_rect;
        // 2nd cas : liste contenant au moins un élément
    } else {
        ei_linked_rect_t* courant = rects_to_update;
        // On se place en derniere position
        while (courant->next != NULL) {
            courant = courant->next;
        }
        // On chaîne le nouvel élément
        courant->next = new_rect;
    }
}



// Vidage de la liste de mise à jour
static void clear_out_list()
{
    if (rects_to_update != NULL) {
        ei_linked_rect_t* courant = rects_to_update;
        ei_linked_rect_t* suivant = courant->next;
        while (suivant != NULL) {
            free(courant);
            courant = suivant;
            suivant = suivant->next;
        }
        free(courant);
        rects_to_update = NULL;
    }
}



// Adds a rectangle to the list of rectangles that must be updated on screen
void ei_app_invalidate_rect(ei_rect_t* rect)
{
    ei_linked_rect_t* new_rect = malloc(sizeof(ei_linked_rect_t));
    new_rect->rect = *rect;
    new_rect->next = NULL;
    insertion_tail(new_rect);
}



/****************************************************************************/
/*************************** FREE AN APPLICATION ****************************/
/****************************************************************************/



extern ei_widgetclass_t* global_register_list;
extern ei_geometrymanager_t* global_geometry_register;


// Releases all the resources of the application
void ei_app_free()
{
    // Suppression de tous les bindings
    ei_unbind_everything();

    // Release of all widgets
    ei_widget_destroy(root_widget);
    root_widget = NULL;

    // Release of all classes of widget (global_register_list)
    if (global_register_list != NULL) {
        ei_widgetclass_t* courant = global_register_list;
        ei_widgetclass_t* suivant = courant->next;
        while (suivant != NULL) {
            free(courant);
            courant = suivant;
            suivant = suivant->next;
        }
        free(courant);
        global_register_list = NULL;
    }

    // Release of all geometry managers (global_geometry_register)
    if (global_geometry_register != NULL) {
        ei_geometrymanager_t* courant = global_geometry_register;
        ei_geometrymanager_t* suivant = courant->next;
        while (suivant != NULL) {
            free(courant);
            courant = suivant;
            suivant = suivant->next;
        }
        free(courant);
        global_geometry_register = NULL;
    }

    // Release of the updating list
    clear_out_list();

    // Release of the hardware
    // Rmq : "display_surface" est automatiquement libérée
    hw_surface_free(offscreen_surface);
    hw_quit();
}



/****************************************************************************/
/*************************** FONCTIONS DE DESSIN ****************************/
/****************************************************************************/



// Fonction récursive qui dessine le widget donné en paramètre ainsi que
// toute sa descendance (parcours en profondeur)
static void ei_draw_widget_family(ei_widget_t* widget, ei_rect_t* clips)
{
    // Vérification du widget passé en paramètre
    if (widget == NULL) {
        return;
    }

    // Classe du widget à dessiner
    ei_widgetclass_t* widget_class = widget->wclass;

	// Définition du paramètre "clipper"
	ei_rect_t clipper;
	if (widget->parent == NULL) {
	    clipper = hw_surface_get_rect(display_surface);
	} else if ((widget->parent)->content_rect == NULL) {
	    clipper = hw_surface_get_rect(display_surface);
	} else {
	    clipper = *(widget->parent)->content_rect;	
	}
	
	if (clips != NULL) {
		clipper = ei_rectangles_intersect(clipper, *clips);
    }
	
    // Dessin du widget
    widget_class->drawfunc(widget,display_surface,offscreen_surface,&clipper); 

    // Appels récursifs sur les fils du widget
    ei_widget_t* fils_courant = widget->children_head;
    while (fils_courant != NULL) {
        ei_draw_widget_family(fils_courant, clips);
        fils_courant = fils_courant->next_sibling;
    }
}



/****************************************************************************/
/************************ TRAITEMENT DES CALLBACKS **************************/
/****************************************************************************/



// Fonction qui exécute les callbacks lors d'un événement
void ei_execute_callbacks(ei_widget_t* widget, ei_event_t* event)
{
	ei_rect_t* monrectangle = widget->content_rect;

    ei_bindingcell_t* handle = bindinglist;
    ei_rect_t clipped_rect;

    // Mise à jour de la mémoire d'enfoncement de la souris
    if (event->type == ei_ev_mouse_buttondown) {
        set_mouse_down(EI_TRUE);
        set_mouse_down_coordinates(event->param.mouse.where);
        set_current_id(widget->pick_id);
    }
    if (event->type == ei_ev_mouse_buttonup) {
        set_mouse_down(EI_FALSE);
    }

    // Parcours de toute la bindinglist
    while (handle != NULL) {
        // On repère les bindings correspondant aux widgets concernés
        // ou au tag "all"
        if (	(handle->binding.widget = widget)
                ||	(!strncmp(handle->binding.tag,"all",3))
                ||	(!strncmp(handle->binding.tag,widget->wclass->name,20))) {
            // On regarde si c'est le bon event
            if (handle->binding.eventtype == event->type) {
                // Si c'est le cas, on exécute le callback
                // avec les paramètres spécifiés
                handle->binding.callback(widget, event,
                        handle->binding.user_param);
                // On date les rects en les empêchant
                // de dépasser du root_widget
                clipped_rect = ei_rectangles_intersect(*monrectangle,
                       root_widget->screen_location);
                if (event->type != ei_ev_mouse_move) {
                	ei_app_invalidate_rect(&clipped_rect);
                }
            }
        }
        handle = handle->next;
    }
}



/****************************************************************************/
/************************* MISE A JOUR DE LA SURFACE ************************/
/****************************************************************************/



// Mise à jour des surfaces décrites dans "rects_to_update"
static void ei_redraw_surfaces()
{
    ei_linked_rect_t* rect_cour = rects_to_update;
    while (rect_cour != NULL) {
    	ei_draw_widget_family(root_widget, &(rect_cour->rect));
        rect_cour = rect_cour->next;
    }
}



/****************************************************************************/
/**************************** BOUCLE PRINCIPALE *****************************/
/****************************************************************************/



void ei_app_run()
{

    ei_event_t event;
    ei_widget_t* widget;
    
    // ETAPE 1 : DESSIN DES WIDGETS

    // Lock of surfaces
    hw_surface_lock(display_surface);
    hw_surface_lock(offscreen_surface);

    // Nouvelle initialisation de root_widget (spécial à la racine)
	ei_root_frame_setdefaultsfunc(root_widget,display_surface);

    // Dessin de tous les widgets
    ei_draw_widget_family(root_widget, NULL);

    // Unlock et mise a jour
    hw_surface_unlock(display_surface);
    hw_surface_unlock(offscreen_surface);
    hw_surface_update_rects(display_surface, NULL);

    // Bindings relatifs aux boutons 
    // Binding du callback associé au bouton déclenché lors d'un "mouse_buttonup"
    ei_bind(ei_ev_mouse_buttonup, NULL, "button", ei_button_click, NULL);
    // Gestion basique de l'inversion du relief
    ei_bind(ei_ev_mouse_buttonup, NULL, "button", ei_raise_button, NULL);
    ei_bind(ei_ev_mouse_buttondown, NULL, "button", ei_sink_button, NULL);
    ei_bind(ei_ev_mouse_move, NULL, "button", ei_toggle_relief, NULL);
    ei_bind(ei_ev_mouse_move, NULL, "all", ei_reset_all_buttons, NULL);
    
    // Bindings relatifs aux toplevels
    ei_bind(ei_ev_mouse_move, NULL, "frame", ei_toplevel_drag, NULL);
    ei_bind(ei_ev_mouse_move, NULL, "frame", ei_toplevel_resize, NULL);
    ei_bind(ei_ev_keydown, NULL, "all", ei_toplevel_keys, NULL);
    ei_bind(ei_ev_mouse_buttondown, NULL, "all", ei_toplevel_focus, NULL);

    // ETAPE 2 : Main event loop 

    do {
		// 1.Attente d'un nouvel événement
        hw_event_wait_next(&event);

		// Vérouillage des surfaces de dessin
        hw_surface_lock(display_surface);
        hw_surface_lock(offscreen_surface);

        // 2. Identification du widget 
        widget = ei_widget_pick(&(event.param.mouse.where));

        // 3.Appel du ou des traitants correspondants 
        ei_execute_callbacks(widget, &event);

        // 4.Remise à jour de l'affichage
        ei_redraw_surfaces();
        
        // Dévérouillage des surfaces de dessin  
        hw_surface_unlock(display_surface);
        hw_surface_unlock(offscreen_surface);
        
        if (rects_to_update != NULL) {
        	hw_surface_update_rects(display_surface, rects_to_update);
        }
        clear_out_list();

    } while (run_app == EI_TRUE);
}



void ei_app_quit_request()
{
    run_app = EI_FALSE;
}



/****************************************************************************/
/*********************** ACCES AUX VARIABLES GLOBALES ***********************/
/****************************************************************************/



// Returns the root widget of the application
ei_widget_t* ei_app_root_widget()
{
    return(root_widget);
}


// Returns the surface of the root window
ei_surface_t ei_app_root_surface()
{
    return(display_surface);
}


