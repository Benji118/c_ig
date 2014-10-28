/**
 * @file	widgetclass.c
 * @brief	Definition and registration of widget classes.
 *
 * \author 
 * Equipe poulet
 * 06.2014
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ei_widgetclass.h"
#include "ei_frame_widget.h"
#include "ei_button_widget.h"
#include "ei_toplevel_widget.h"



/* LISTE DEFINIE EN GLOBAL QUI STOCKE TOUTES LES CLASSES DE WIDGETS CREES */

ei_widgetclass_t* global_register_list = NULL; // La liste est initialement vide



/* FONCTION D'INSERTION POUR LE TRAITEMENT DE LA LISTE GLOBALE */

// Insertion d'une classe en queue de la liste globale 
static void insertion_tail(ei_widgetclass_t* widgetclass)
{
    // 1er cas : liste vide
    if (global_register_list == NULL) {
        global_register_list = widgetclass;
        // 2nd cas : liste contenant au moins un élément
    } else {
        ei_widgetclass_t* courant = global_register_list;
        // On se place en derniere position
        while (courant->next != NULL) {
            courant = courant->next;
        }
        // On chaîne le nouvel élément
        courant->next = widgetclass;
    }
}


/* INSERTION DE NOUVELLES CLASSES DANS LA LISTE GLOBALE */

void ei_widgetclass_register(ei_widgetclass_t* widgetclass)
{
    insertion_tail(widgetclass);
}


/* RETROUVER UNE CLASSE DE WIDGET DANS LA LISTE GLOBALE */

ei_widgetclass_t* ei_widgetclass_from_name(ei_widgetclass_name_t name)
{
    // Pointeur courant initialisé au premier élément de la liste globale
    ei_widgetclass_t* courant = global_register_list;
    // Traitement de la classe de widget courante
    while (courant != NULL) {
        uint32_t comparaison = strncmp(courant->name, name, 20);
        if (comparaison == 0) {
            return courant;
        }
        courant = courant->next;
    }
    return NULL;
}


/* ENREGISTREMENT DE NOUVELLES CLASSES DE WIDGETS */

void ei_frame_register_class ()
{
    // Allocation mémoire
    ei_widgetclass_t* frame = malloc(sizeof(ei_widgetclass_t));
    // Attributs de la classe
    strncpy(frame->name, "frame", 20);
    frame->allocfunc = ei_frame_allocfunc;
    frame->releasefunc = ei_frame_releasefunc;
    frame->drawfunc = ei_frame_drawfunc;					
    frame->setdefaultsfunc = ei_frame_setdefaultsfunc;
    frame->geomnotifyfunc = ei_frame_geomnotifyfunc;
    frame->next = NULL;
    // Insertion dans la liste globale
    ei_widgetclass_register(frame);
}

void ei_button_register_class ()
{
    // Allocation mémoire
    ei_widgetclass_t* button = malloc(sizeof(ei_widgetclass_t));
    // Attributs de la classe
    strncpy(button->name, "button", 20);
    button->allocfunc = ei_button_allocfunc;
    button->releasefunc = ei_button_releasefunc;
    button->drawfunc = ei_button_drawfunc;
    button->setdefaultsfunc = ei_button_setdefaultsfunc;
    button->geomnotifyfunc = ei_button_geomnotifyfunc;
    button->next = NULL;
    // Insertion dans la liste globale
    ei_widgetclass_register(button);
}

void ei_toplevel_register_class ()
{
    // Allocation mémoire
    ei_widgetclass_t* toplevel = malloc(sizeof(ei_widgetclass_t));
    // Attributs de la classe
    strncpy(toplevel->name, "toplevel", 20);
    toplevel->allocfunc = ei_toplevel_allocfunc;
    toplevel->releasefunc = ei_toplevel_releasefunc;
    toplevel->drawfunc = ei_toplevel_drawfunc;
    toplevel->setdefaultsfunc = ei_toplevel_setdefaultsfunc;
    toplevel->geomnotifyfunc = ei_toplevel_geomnotifyfunc;
    toplevel->next = NULL;
    // Insertion dans la liste globale
    ei_widgetclass_register(toplevel);
}


