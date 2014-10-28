/**
 * @file	ei_widget.c
 * @brief	API for widgets management :
 *			- creation
 *			- configuration
 *			- hierarchy
 *			- redisplay
 * 
 * \author
 * Equipe poulet
 * 06.2014
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ei_widget.h"
#include "ei_toplevel_widget.h"
#include "ei_button_widget.h"
#include "ei_frame_widget.h"
#include "ei_geometrymanager.h"
#include "ei_application.h"
#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_mouse.h"


// Macro utilisée dans les fonctions de configuration des widgets
#define TEST_AND_SET(a,b) if (a != NULL) { b->a = *a; }


// Compteur général utilisé pour l'identification offscreen des widgets
uint32_t global_pick_counter = 0;


// Paramètres globals de la barre de titre d'un toplevel
extern uint32_t titlebar_height;
extern ei_color_t titlebar_color;



/******************************************************************************/
/*************** CONVERSION ENTIER->COULEUR (POUR L'OFFSCREEN) ****************/
/******************************************************************************/



ei_color_t int_to_color(uint32_t n)
{
    ei_color_t color;	
    color.blue	= n % 256;
    n=n/256;
    color.green	= n % 256;
    n=n/256;
    color.red	= n % 256;
    color.alpha	= 255;
    return(color);
}



/******************************************************************************/
/**************************** CREATION D'UN WIDGET ****************************/
/******************************************************************************/



ei_widget_t* ei_widget_create (ei_widgetclass_name_t class_name,
        ei_widget_t* parent)
{

    // Verification de l'existence d'une classe nommée "class_name"
    ei_widgetclass_t* class = ei_widgetclass_from_name(class_name);
    if (class == NULL) {
        return NULL;
    }
    // Maintenant la classe "class_name" est connue et enregistrée
    // dans la bibliothèque

    // Allocation mémoire du widget (la place en mémoire dépend de sa classe)
    void* widget_of_a_class = class->allocfunc();

    // Transtypage (pour utiliser les attributs généraux du widget)
    ei_widget_t* widget = (ei_widget_t*) widget_of_a_class;

    // Initialisation des attributs généraux du widget

    // 1.Classe du widget
    widget->wclass = class;

    // 2.Paramètres offscreen
    widget->pick_id		= global_pick_counter;
    global_pick_counter	= global_pick_counter + 1;
    ei_color_t ID_color	= int_to_color(widget->pick_id);
    widget->pick_color	= malloc(sizeof(ei_color_t));
    *(widget->pick_color) = ID_color;	

    // 3.Chaînage dans l'arbre des widgets

    // 3.1.Parent du widget
    // CAS 1 : parent != racine
    if (parent != NULL) {
        // CAS 1.1 : parent != toplevel
        if (strncmp(parent->wclass->name, "toplevel", 8)) {
            widget->parent = parent; // On prend le parent donné
            // CAS 1.2 : parent == toplevel
        } else {
            // Deux sous cas
            if (widget->pick_id < (parent->pick_id + 5) ) {
                widget->parent = parent; // On prend le parent donné
            } else {
                ei_widget_t* inside = parent->children_head->next_sibling;
                // Can't be NULL
                widget->parent = inside;
                // On prend le second fils (inside) du parent
            }
        }
        // CAS 2 : parent == racine
    } else {
        widget->parent = NULL;
    }

    widget->children_head = NULL;
    widget->children_tail = NULL;
    widget->next_sibling = NULL;

    // 3.2.Chainage du widget dans les fils de son parent
    // Cas 0 : parent == NULL (c.a.d widget == racine) -> rien à faire
    if (widget->parent != NULL) {
        // Cas 1 : "parent" n'a aucun fils
        if (widget->parent->children_head == NULL) {
            widget->parent->children_head = widget;
            widget->parent->children_tail = widget;
            // Cas 2 : "parent" a au moins un fils
        } else {
            ei_widget_t* old_tail = widget->parent->children_tail;
            old_tail->next_sibling = widget;
            widget->parent->children_tail = widget;
        }
    }

    // 4.Paramètres de gestion de la géométrie
    widget->geom_params = NULL;
    widget->requested_size = ei_size_zero();
    widget->screen_location.top_left = ei_point_zero();
    widget->screen_location.size = ei_size_zero();
    widget->content_rect = &(widget->screen_location);
   
    // Initialisation des attributs spécifiques à "class"
    class->setdefaultsfunc(widget);

    /********************************************************/
    /* CREATION DE LA DESCENDANCE AUTOMATIQUE D'UN TOPLEVEL */
    /********************************************************/

    // Un toplevel a toujours trois et seulement trois fils qui sont créés ici.
    // Les fils du toplevel créé par le programmeur sont en fait les fils de son
    // second fils (cf 3.1. choix du parent)
    if (!strncmp(class_name, "toplevel", 8)) {
        // TITLEBAR : widget de classe frame pour la barre de titre de la fenetre
        // C'est toujours le premier fils d'un toplevel
        ei_widget_t* titlebar = ei_widget_create("frame", widget);
        // CLOSE : widget de classe button pour la fermeture de la fenetre
        // C'est toujours le premier petit-fils d'un toplevel
        ei_widget_create("button", titlebar);
        // INSIDE : widget de class frame pour l'intérieur de la fenetre
        // C'est toujours le second fils d'un toplevel
        // C'est lui qui contient tous les fils du toplevel
        // créés par le programmeur
        ei_widget_create("frame", widget);
        // RESIZE : widget de classe button pour redimmensionner la fenetre
        // C'est toujours le troisieme fils d'un toplevel
        ei_widget_create("frame", widget);
    }

    return widget;
}



/******************************************************************************/
/************************** DESTRUCTION D'UN WIDGET ***************************/
/******************************************************************************/



// Procédure récursive
void ei_widget_destroy (ei_widget_t* widget)
{

    // Vérification du widget passé en paramètre
    if (widget == NULL) {
        return;
    }

    // ETAPE 1 : Destruction de la descendance

    if (widget->children_head != NULL) {
        ei_widget_t* fils_courant = widget->children_head;
        ei_widget_t* fils_suivant;
        while (fils_courant != widget->children_tail) {
            fils_suivant = fils_courant->next_sibling;
            ei_widget_destroy(fils_courant); // Appel récursif
            fils_courant = fils_suivant;
        }
        ei_widget_destroy(fils_courant); // Appel récursif
        widget->children_head = NULL;
        widget->children_tail = NULL;
    }

    // ETAPE 2 : Déchaînage du widget de l'arbre hiérarchique

    ei_widget_t* parent = widget->parent;
    if (parent != NULL) { // Déchaînage sauf si widget == root_widget
        // CAS 1 : Le widget est le premier fils de son parent
        if (parent->children_head == widget) {
            // CAS 1.1 : Le widget est l'unique fils de son parent
            if (parent->children_tail == widget) {
                parent->children_head = NULL;
                parent->children_tail = NULL;
                // CAS 1.2 : Son parent a au moins deux fils
            } else {
                parent->children_head = widget->next_sibling;
            }
            // CAS 2 : Le widget n'est pas le premier fils de son parent
        } else {
            ei_widget_t* courant = parent->children_head;
            ei_widget_t* suivant = courant->next_sibling;
            while (suivant != widget) {
                courant = suivant;
                suivant = suivant->next_sibling;
            }
            // Maintenant : - courant pointe sur le fils précédent widget
            //				- suivant pointe sur widget
            // CAS 2.1 : Le widget est le dernier fils
            if (widget == parent->children_tail) {
                parent->children_tail = courant;
                courant->next_sibling = NULL;
                // CAS 2.2 : Le widget n'est pas le dernier fils
            } else {
                courant->next_sibling = suivant->next_sibling;
            }
        }
        widget->parent = NULL;
        widget->next_sibling = NULL;
    }

    // Maintenant le widget est isolé de l'arbre hiérarchique des widgets

    // ETAPE 3 : Libération du widget

    // Libération des attributs généraux du widget
    ei_geometrymanager_unmap(widget); // Libération du gestionnaire de géométrie
    free(widget->pick_color);

    // Libération des attributs spécifiques à la classe du widget
    ei_widgetclass_t* class = widget->wclass;
    class->releasefunc(widget);

    // Libération définitive du widget
    free(widget);
}



/******************************************************************************/
/********************************* WIDGET PICK ********************************/
/******************************************************************************/



extern ei_surface_t offscreen_surface;


// Retourne : pointeur sur le widget de l'écran correspondant au point "where"
ei_widget_t* ei_widget_pick(ei_point_t* where)
{
    // Pointeur sur le pixel en haut à gauche (origine) de l'offscreen
    uint8_t* pointeur = hw_surface_get_buffer(offscreen_surface);

    // Taille de l'offscreen
    ei_size_t offscreen_size = hw_surface_get_size(offscreen_surface);

    // Déplacement de "pointeur" dans la mémoire :
    // "pointeur" pointe sur le pixel correspondant à "where"
    pointeur = pointeur + ( (where->y)*offscreen_size.width + where->x ) * 4;

    // On réccupère l'ordre dans la mémoire des couleurs d'un pixel
    int red;
    int green;
    int blue;
    int alpha;
    hw_surface_get_channel_indices(offscreen_surface,&red,&green,&blue,&alpha);	

    // Puis on récupère chaque couleur du pixel pointé par "pointeur"
    uint8_t* pointeur_red = pointeur + red;
    uint8_t* pointeur_green = pointeur + green;
    uint8_t* pointeur_blue = pointeur + blue;

    // Calcul de l'entier correspondant à la couleur du pixel
    uint32_t n = (*pointeur_blue) + 256 * (*pointeur_green)
        + 256 * 256 * (*pointeur_red);

    // Maintenant on retrouve le widget correspondant à l'entier
    ei_widget_t* root_widget = ei_app_root_widget();
    ei_widget_t* solution;

    find_widget_from_pick_id(root_widget, n, &solution);

    return(solution);
}



/******************************************************************************/
/************************** CONFIGURATION DES WIDGETS *************************/
/******************************************************************************/



void ei_frame_configure(ei_widget_t*		widget,
        ei_size_t*			requested_size,
        const ei_color_t*	color,
        int*				border_width,
        ei_relief_t*		relief,
        char**				text,
        ei_font_t*			text_font,
        ei_color_t*			text_color,
        ei_anchor_t*		text_anchor,
        ei_surface_t*		img,
        ei_rect_t**			img_rect,
        ei_anchor_t*		img_anchor)
{
    // Vérification du widget passé en paramètre
    if (widget == NULL) {
        return;
    }
    // Transtypage en type "frame"
    ei_frame_widget_t* frame_widget = (ei_frame_widget_t*) widget;
    // Configuration des attributs pour les pointeurs non nuls
    TEST_AND_SET(requested_size,widget);
    TEST_AND_SET(color,frame_widget);
    TEST_AND_SET(border_width,frame_widget);
    TEST_AND_SET(relief,frame_widget);
    TEST_AND_SET(text,frame_widget);
    TEST_AND_SET(text_font,frame_widget);
    TEST_AND_SET(text_color,frame_widget);
    TEST_AND_SET(text_anchor,frame_widget);
    if (frame_widget->text != NULL) {
        frame_widget->img = NULL;
    } else {
        TEST_AND_SET(img,frame_widget);
    }
    if (img_rect != NULL) {
    	frame_widget->img_rect = malloc(sizeof(ei_rect_t));
    	*(frame_widget->img_rect) = **img_rect;
    }
    TEST_AND_SET(img_rect,frame_widget);
    TEST_AND_SET(img_anchor,frame_widget);
}


/******************************************************************************/


void ei_toplevel_configure (ei_widget_t*	widget,
        ei_size_t*		requested_size,
        ei_color_t*		color,
        int*			border_width,
        char**			title,
        ei_bool_t*		closable,
        ei_axis_set_t*	resizable,
        ei_size_t**		min_size)
{
    // Vérification du widget passé en paramètre
    if (widget == NULL) {
        return;
    }
    // Transtypage en type "toplevel"
    ei_toplevel_widget_t* toplevel_widget = (ei_toplevel_widget_t*) widget;
    // Configuration des attributs pour les pointeurs non nuls
    TEST_AND_SET(requested_size,widget);
    TEST_AND_SET(color,toplevel_widget);
    TEST_AND_SET(border_width,toplevel_widget);
    TEST_AND_SET(title,toplevel_widget);
    TEST_AND_SET(closable,toplevel_widget);
    TEST_AND_SET(resizable,toplevel_widget);
    TEST_AND_SET(min_size,toplevel_widget);
}


/******************************************************************************/


void ei_button_configure(	ei_widget_t*		widget,
        ei_size_t*			requested_size,
        const ei_color_t*	color,
        int*				border_width,
        int*				corner_radius,
        ei_relief_t*		relief,
        char**				text,
        ei_font_t*			text_font,
        ei_color_t*			text_color,
        ei_anchor_t*		text_anchor,
        ei_surface_t*		img,
        ei_rect_t**			img_rect,
        ei_anchor_t*		img_anchor,
        ei_callback_t*		callback,
        void**				user_param)
{
    // Vérification du widget passé en paramètre
    if (widget == NULL) {
        return;
    }
    // Transtypage en type "button"
    ei_button_widget_t* button_widget = (ei_button_widget_t*) widget;
    // Configuration des attributs pour les pointeurs non nuls
    TEST_AND_SET(requested_size,widget);
    TEST_AND_SET(color,button_widget);
    TEST_AND_SET(border_width,button_widget);
    TEST_AND_SET(corner_radius,button_widget);
    TEST_AND_SET(relief,button_widget);
    TEST_AND_SET(text,button_widget);
    TEST_AND_SET(text_font,button_widget);
    TEST_AND_SET(text_color,button_widget);
    TEST_AND_SET(text_anchor,button_widget);
    if (button_widget->text != NULL) {
        button_widget->img = NULL;
    } else {
        TEST_AND_SET(img,button_widget);
    }
    if (img_rect != NULL) {
    	button_widget->img_rect = malloc(sizeof(ei_rect_t));
    	*(button_widget->img_rect) = **img_rect;
    }
    TEST_AND_SET(img_anchor,button_widget);
    TEST_AND_SET(callback,button_widget);
    TEST_AND_SET(user_param,button_widget);
    TEST_AND_SET(text_font,button_widget);
    
}


