/**
 * @file	ei_main.c
 * @brief	Declares the "ei_main" function : the main function of programs built with the
 *			libei.
 *
 * \author 
 * Equipe poulet
 * 06.2014
 *
 **/

#include "ei_main.h"

#include "ei_geometrymanager.h"
#include "ei_event.h"
#include "ei_application.h"


/**
 * \brief	The main function of the program.
 * @param	argc, argv	The parameters that were passed the the "main" function.
 * @return	An error code : 0 means ok, 1 means error.
 */


int ei_main (int argc, char* argv[])
{

    // 0.Phase d'initialisation :
    // 		- créer les intéracteurs et définir leurs attributs
    //		- enregistrer les traitants d'évènements utilisateur
    //		- placer à l'écran chaque interacteur via un gestionnaire de géométrie

    //ei_app_create(ei_size_t* main_window_size, ei_bool_t fullscreen);


    // tant que pas de demande d'arrêt du programme faire
    //		dessiner à l'écran les mises à jour nécessaires
    //		attendre un évènement
    //		analyser l'évènement pour trouver le traitant associé
    //		appeller le traitant associé
    // fin tant que

    // 1.Attente d'un nouvel évènement :
    // 2.Recherche de l'intéracteur concerné
    // 3.Appel du ou des traitants correspondants
    // 4.Mise à jour de l'affichage

    // ei_app_run();


    // 5.Phase de terminaison :
    //		= liberer toutes les ressources utilisées

    // ei_app_free();

}
