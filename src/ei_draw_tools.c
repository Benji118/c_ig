/**
 * @file	ei_draw_tools.c
 * @brief	Tools to draw widgets
 * 
 * \author
 * Equipe poulet
 * 06.2014
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#include "ei_draw_tools.h"
#include "hw_interface.h"
#include "ei_draw.h"
#include "ei_utils.h"
#include "ei_application.h"



/*****************************************************************************/
/*****************************************************************************/



// Requiert : 	- un pointeur sur un widget
//				- un entier correspondant à la largeur de la bordure en pixels
// Retourne :	une liste chaînée de points correspondant au cadre
//				rectangulaire du widget diminué de sa bordure
// Remarque :	désallocation de la liste nécessaire après utilisation
ei_linked_point_t* ei_frame_calculation(ei_widget_t* widget, int border_width)
{
    // Déclaration des 4 points
    ei_linked_point_t* topleft		= malloc(sizeof(ei_linked_point_t));
    ei_linked_point_t* topright		= malloc(sizeof(ei_linked_point_t));
    ei_linked_point_t* bottomright	= malloc(sizeof(ei_linked_point_t));
    ei_linked_point_t* bottomleft	= malloc(sizeof(ei_linked_point_t));
    ei_linked_point_t* topleft_again = malloc(sizeof(ei_linked_point_t));
    // Calcul des coordonnées de chaque point (bordure prise en compte)
    topleft->point.x		= widget->screen_location.top_left.x - border_width;
    topleft->point.y		= widget->screen_location.top_left.y - border_width;
    topright->point.x		= widget->screen_location.top_left.x 
       + widget->screen_location.size.width + border_width;
    topright->point.y		= topleft->point.y;
    bottomright->point.x	= topright->point.x;
    bottomright->point.y	= widget->screen_location.top_left.y
       + widget->screen_location.size.height + border_width;
    bottomleft->point.x		= topleft->point.x;
    bottomleft->point.y		= bottomright->point.y;
    topleft_again->point	= topleft->point;
    topleft_again->next		= NULL;
    // Chaînage des points
    topleft->next		= topright;
    topright->next		= bottomright;
    bottomright->next	= bottomleft;
    bottomleft->next	= topleft_again;
    topleft_again->next	= NULL;
    return(topleft);
}



/*****************************************************************************/
/*****************************************************************************/



// Désalloue la mémoire allouée pour une liste chaînée de points
void ei_free_linked_points_list(ei_linked_point_t* liste)
{
    if (liste == NULL) {
        return;
    }
    ei_linked_point_t* courant = liste;
    ei_linked_point_t* suivant = liste->next;
    while (suivant != NULL) {
        free(courant);
        courant = suivant;
        suivant = suivant->next;
    }
    free(courant);
}



/*****************************************************************************/
/*****************************************************************************/



// Requiert : liste1 et liste2 deux listes simplement chaînée de points
// bien allouées
// Retourne : liste1 = liste1&liste2
void concat_lists(ei_linked_point_t** liste1, ei_linked_point_t** liste2)
{
    // 1er cas : liste2 vide -> rien à faire
    // 2nd cas : liste2 non vide
    if (*liste2 != NULL) {
        if (*liste1 == NULL) {
            *liste1 = *liste2;
        } else {
            ei_linked_point_t* courant = *liste1;
            // On se place en derniere position de liste1
            while (courant->next != NULL) {
                courant = courant->next;
            }
            // On chaîne la seconde liste
            courant->next = *liste2;
        }
    }
}



/*****************************************************************************/
/*****************************************************************************/



// Retourne une couleur éclaircie de la couleur donnée
// A utiliser après du blanc pour ei_relief_raised
// A utiliser après du noir pour ei_relief_sunken
ei_color_t ei_clearer_color(ei_color_t color)
{
    ei_color_t clearer_color = color;
    clearer_color.alpha	= color.alpha-100;
    return(clearer_color);
}



/*****************************************************************************/
/*****************************************************************************/



// Requiert :	- anchor	= la position d'ancrage désirée
//				- wtopleft	= le topleft du widget de dessin
//				- wsize		= la taille du widget de dessin
//				- size		= la taille de l'image ou du texte à dessiner
//				- topleft	= un pointeur sur le point d'ancrage à calculer
// Retourne :	topleft est calculé en fonction l'ancrage donné
static void ei_switch_anchor(ei_anchor_t anchor, ei_point_t* topleft,
        ei_size_t size, ei_point_t wtopleft, ei_size_t wsize)
{
    switch (anchor) {
        case ei_anc_none :
            break;
        case ei_anc_center : // Anchor in the center
            topleft->x = wtopleft.x + wsize.width/2 - size.width/2;
            topleft->y = wtopleft.y + wsize.height/2 - size.height/2;
            break;
        case ei_anc_north : // Anchor on the top side, centered horizontally
            topleft->x = wtopleft.x + wsize.width/2 - size.width/2;
            topleft->y = wtopleft.y;
            break;
        case ei_anc_northeast : // Anchor on the top-right corner
            topleft->x = wtopleft.x + wsize.width - size.width;
            topleft->y = wtopleft.y;
            break;
        case ei_anc_east : // Anchor on the right side, centered vertically
            topleft->x = wtopleft.x + wsize.width - size.width;
            topleft->y = wtopleft.y + wsize.height/2 - size.height/2;
            break;
        case ei_anc_southeast : // Anchor on the bottom-right corner
            topleft->x = wtopleft.x + wsize.width - size.width;
            topleft->y = wtopleft.y + wsize.height - size.height;
            break;
        case ei_anc_south : // Anchor on the bottom side, centered horizontally
            topleft->x = wtopleft.x + wsize.width/2 - size.width/2;
            topleft->y = wtopleft.y + wsize.height - size.height;
            break;
        case ei_anc_southwest : // Anchor on the bottom-left corner
            topleft->x = wtopleft.x;
            topleft->y = wtopleft.y + wsize.height - size.height;
            break;
        case ei_anc_west : // Anchor on the left side, centered vertically
            topleft->x = wtopleft.x;
            topleft->y = wtopleft.y + wsize.height/2 - size.height/2;
            break;
        case ei_anc_northwest : // Anchor on the top-left corner
            topleft->x = wtopleft.x;
            topleft->y = wtopleft.y;
            break;
    }
}



/*****************************************************************************/
/*****************************************************************************/



// Dessine le texte d'un widget
void ei_draw_widget_text(	ei_surface_t	surface,
							ei_widget_t*	widget,
							char*			text,
							ei_font_t		text_font,
							ei_color_t		text_color,
							ei_anchor_t		anchor		)
{
    // Calcul du point d'ancrage "where" du type ei_point_t*

    ei_point_t where = ei_point_zero();
    
    // Taille du texte
    ei_size_t tsize;
    hw_text_compute_size(text,text_font,&(tsize.width),&(tsize.height));
    
    // Paramètres du widget
    ei_size_t wsize = widget->screen_location.size;
    ei_point_t wtopleft = widget->screen_location.top_left;

    if ((tsize.width >= wsize.width + 10) 
		|| (tsize.height >= wsize.height + 12)) {
        // Texte plus grand que le frame
        fprintf(stderr, "Texte trop grand pour être affiché.\n");
        return;
    }
	
    ei_switch_anchor(anchor, &where, tsize, wtopleft, wsize);
    ei_rect_t text_clipper;
    
    if (widget->parent == NULL) {
    	text_clipper = widget->screen_location;
    } else {
    	text_clipper = ei_rectangles_intersect(widget->screen_location,
    					widget->parent->screen_location);
    }
    ei_draw_text(surface, &where, text, text_font, &text_color, &text_clipper);
}



/*****************************************************************************/
/*****************************************************************************/



// Dessine l'image d'un widget
void ei_draw_widget_img(	ei_surface_t	surface,
							ei_widget_t*	widget,
							ei_surface_t	img,
							ei_rect_t*		img_rect,
							ei_anchor_t		anchor,
							int				button_border_width	)
{	
    // Paramètres du widget
    ei_point_t wtopleft	= ei_point_add( widget->screen_location.top_left,
            ei_point(2*button_border_width, 2*button_border_width) );
    ei_size_t wsize = ei_size_sub( widget->screen_location.size,
            ei_size(4*button_border_width, 4*button_border_width) );

    // Variables à calculer
    ei_rect_t dst_widget_rect;
    ei_rect_t src_image_rect;

    /***********************************************/
    /* ETAPE 1 : CALCUL DE LA TAILLE DE DST ET SRC */
    /***********************************************/	

    ei_size_t img_size;
    if (img_rect != NULL) {
        img_size = img_rect->size;
    } else {
        img_size = hw_surface_get_size(img);
    }

    // Calcul de la largeur de la zone de copie
    if (img_size.width > wsize.width) {
        src_image_rect.size.width = wsize.width;
        dst_widget_rect.size.width = wsize.width;
    } else {
        src_image_rect.size.width = img_size.width;
        dst_widget_rect.size.width = img_size.width;
    }
    // Calcul de la hauteur de la zone de copie
    if (img_size.height > wsize.height) {
        src_image_rect.size.height = wsize.height;
        dst_widget_rect.size.height = wsize.height;
    } else {
        src_image_rect.size.height = img_size.height;
        dst_widget_rect.size.height = img_size.height;
    }

    /*************************************/
    /* ETAPE 2 : CALCUL DES DEUX TOPLEFT */
    /*************************************/	

    // Calcul du topleft de la destination
    ei_switch_anchor(anchor, &(dst_widget_rect.top_left),
            dst_widget_rect.size, wtopleft, wsize);

    // Calcul du topleft de la source
    if (img_rect != NULL) {
        src_image_rect.top_left = img_rect->top_left;
    //    fprintf(stderr, "x %i\n", img_rect->top_left.x);
    //    fprintf(stderr, "y %i\n", img_rect->top_left.y);
    } else {
        // Par défaut je prend le coin en haut à gauche de l'image
        ei_rect_t rect_total = hw_surface_get_rect(img);
        src_image_rect.top_left = rect_total.top_left;
    }

    /*************************************************************/
    /* ETAPE 2bis : RECALCUL SI SORTIE DU CADRE PRINCIPAL (ROOT) */
    /*************************************************************/	

    ei_surface_t display_surface = ei_app_root_surface();
    ei_rect_t display_rect = hw_surface_get_rect(display_surface);

    int cut;

    // Cas 1 : sortie à gauche
    cut = display_rect.top_left.x - dst_widget_rect.top_left.x;
    if	( cut > 0 ) {
        // Changement de DST
        dst_widget_rect.top_left.x = display_rect.top_left.x;
        dst_widget_rect.size.width = dst_widget_rect.size.width - cut;
        // Changement de SRC
        src_image_rect.top_left.x = src_image_rect.top_left.x + cut;
        src_image_rect.size = dst_widget_rect.size;
    }

    // Cas 2 : sortie à droite
    cut = dst_widget_rect.top_left.x + dst_widget_rect.size.width
        - display_rect.top_left.x - display_rect.size.width;
    if	( cut > 0 ) {
        // Changement de DST
        dst_widget_rect.size.width = dst_widget_rect.size.width - cut;
        // Changement de SRC
        src_image_rect.size = dst_widget_rect.size;
    }


    // Cas 3 : sortie en haut
    cut = display_rect.top_left.y - dst_widget_rect.top_left.y;
    if	( cut > 0 ) {
        // Changement de DST
        dst_widget_rect.top_left.y = display_rect.top_left.y;
        dst_widget_rect.size.height = dst_widget_rect.size.height - cut;
        // Changement de SRC
        src_image_rect.top_left.y = src_image_rect.top_left.y + cut;
        src_image_rect.size = dst_widget_rect.size;
    }

    // Cas 4 : sortie en bas
    cut = dst_widget_rect.top_left.y + dst_widget_rect.size.height
        - display_rect.top_left.y - display_rect.size.height;
    if	( cut > 0 ) {
        // Changement de DST
        dst_widget_rect.size.height = dst_widget_rect.size.height - cut;
        // Changement de SRC
        src_image_rect.size = dst_widget_rect.size;
    }

    /*****************************************/
    /* ETAPE 3 : COPIE DE L'IMAGE A AFFICHEE */
    /*****************************************/	

    hw_surface_lock(img);
    ei_copy_surface(surface, &dst_widget_rect, img, &src_image_rect, EI_FALSE);		
    hw_surface_unlock(img);

}



/*****************************************************************************/
/*****************************************************************************/



// Retourne : une liste de points représentant un arc de cercle
// A utiliser dans le dessin des boutons
ei_linked_point_t* ei_draw_arc(ei_point_t centre, float R, float angledebut, float anglefin)
{
    ei_linked_point_t* liste = NULL;

    float angle = angledebut;

    while (angle < anglefin)
    {
        ei_linked_point_t* new_point = malloc(sizeof(ei_linked_point_t));
        new_point->point = ei_point( (int)(R*cos(angle)) + centre.x, 
                                     (int)(R*sin(angle)) + centre.y);
        new_point->next = NULL;
        concat_lists(&liste, &new_point);
        // Pas de rotation = 0.1
        angle = angle + 0.1;
    }

    ei_linked_point_t* last_point = malloc(sizeof(ei_linked_point_t));
    last_point->point = ei_point( (int)(R*cos(anglefin)) + centre.x, 
                                  (int)(R*sin(anglefin)) + centre.y);
    last_point->next = NULL;

    concat_lists(&liste, &last_point);

    return liste;
}



/*****************************************************************************/
/*****************************************************************************/



// Requiert :	- high, booléen qui vaut true ssi on souhaite le contour 
//              		de la partie supérieure
//				- low, booléen qui vaut true ssi on souhaite le contour
//						de la partie inférieure
//				- rect, rectangle contenant le bouton sur lequel dessiner
//						le contour
//				- R, rayon des arcs de cercles des coins du bouton
//				- border, la largeur de la bordure (relief) en pixels
// Retourne :	Une liste chaînée de points correspondant au contour
// 				arrondi d'un bouton
ei_linked_point_t* ei_curvy_contour_button(ei_bool_t high, ei_bool_t low,
        ei_rect_t rect, float R, int border)
{
    // Pointeur "curve_button" sur la liste à retourner
    ei_linked_point_t* curve_button = NULL;

    // Calcul des 4 points intérieurs clés pour calculer les 4 bords
    ei_point_t centertopleft	=
        ei_point( rect.top_left.x + R, rect.top_left.y + R );
    ei_point_t centertopright	=
        ei_point( rect.top_left.x + rect.size.width - R, rect.top_left.y + R );
    ei_point_t centerbottomleft =
        ei_point( rect.top_left.x + R, rect.top_left.y + rect.size.height - R );
    ei_point_t centerbottomright=
        ei_point(rect.top_left.x + rect.size.width - R -1.0, rect.top_left.y
                + rect.size.height - R);

    // Calcul des 2 listes représentant les 2 coins non coupés
    // (topleft et bottomright)
    ei_linked_point_t* curvetopleft		=
        ei_draw_arc(centertopleft, R-(float)border, M_PI, 1.5*M_PI);
    ei_linked_point_t* curvebottomright	=
        ei_draw_arc(centerbottomright, R-(float)border, 0.0,  0.5*M_PI);

    // Définition de h = demi-hauteur
    int h = rect.size.height/2;

    // Calcul des deux points de marquage de la ligne centrale
    ei_point_t markleft  = ei_point(rect.top_left.x + h, rect.top_left.y + h);
    ei_point_t markright =
        ei_point(rect.top_left.x + rect.size.width - h, rect.top_left.y + h);

    // Calcul des 4 listes représentant les 2 coins coupés
    // (bottomleft et topright)
    ei_linked_point_t* curvebottomleft_low	= ei_draw_arc(centerbottomleft,
            R-(float)border, 0.5 *M_PI, 0.75*M_PI);
    ei_linked_point_t* curvetopright_low	= ei_draw_arc(centertopright,
            R-(float)border, 1.75*M_PI, 2*M_PI);
    ei_linked_point_t* curvebottomleft_high	= ei_draw_arc(centerbottomleft,
            R-(float)border, 0.75*M_PI, M_PI);
    ei_linked_point_t* curvetopright_high	= ei_draw_arc(centertopright,
            R-(float)border, 1.5 *M_PI, 1.75*M_PI);

    // Chaînage des points et listes de points suivants les différents cas
    // pour "high" et "low" 

    // CAS 1 : on veut tout le contour
    if ( (high == EI_TRUE) && (low == EI_TRUE) ) {

        // Partie supérieure
        concat_lists(&curve_button, &curvebottomleft_high);
        concat_lists(&curve_button, &curvetopleft);
        concat_lists(&curve_button, &curvetopright_high);
        // Partie inférieure
        concat_lists(&curve_button, &curvetopright_low);
        concat_lists(&curve_button, &curvebottomright);
        concat_lists(&curve_button, &curvebottomleft_low);
        

    // CAS 2 : on ne veut que le contour supérieur
    } else if ( (high == EI_TRUE) && (low == EI_FALSE) ) {
    
        concat_lists(&curve_button, &curvebottomleft_high);
        concat_lists(&curve_button, &curvetopleft);
        concat_lists(&curve_button, &curvetopright_high);        
        // Ligne de démarquation
        ei_linked_point_t* marquage_droit = malloc(sizeof(ei_linked_point_t));
        marquage_droit->point = markright;
        marquage_droit->next = NULL;
        concat_lists(&curve_button, &marquage_droit);
        ei_linked_point_t* marquage_gauche = malloc(sizeof(ei_linked_point_t));
        marquage_gauche->point = markleft;
        marquage_gauche->next = NULL;
        concat_lists(&curve_button, &marquage_gauche);
        // Libération des listes non utilisées
        ei_free_linked_points_list(curvebottomright);
        ei_free_linked_points_list(curvebottomleft_low);
        ei_free_linked_points_list(curvetopright_low);

    // CAS 3 : on ne veut que le contour inférieur
    } else if ( (high == EI_FALSE) && (low == EI_TRUE) ) {
    
        concat_lists(&curve_button, &curvetopright_low);
        concat_lists(&curve_button, &curvebottomright);
        concat_lists(&curve_button, &curvebottomleft_low);
        // Ligne de démarquation
        ei_linked_point_t* marquage_gauche = malloc(sizeof(ei_linked_point_t));
        marquage_gauche->point = markleft;
        marquage_gauche->next = NULL;
        concat_lists(&curve_button, &marquage_gauche);
        ei_linked_point_t* marquage_droit = malloc(sizeof(ei_linked_point_t));
        marquage_droit->point = markright;
        marquage_droit->next = NULL;
        concat_lists(&curve_button, &marquage_droit);
        // Libération des listes non utilisées
        ei_free_linked_points_list(curvetopleft);
        ei_free_linked_points_list(curvebottomleft_high);
        ei_free_linked_points_list(curvetopright_high);
    }

    return(curve_button);
}



/*****************************************************************************/
/*****************************************************************************/



// Renvoie le rectangle formé par l'intersection de deux rectangles
ei_rect_t ei_rectangles_intersect(ei_rect_t a, ei_rect_t b)
{
    ei_rect_t c;
    uint32_t i_x;
    uint32_t i_y;
    uint32_t cut_x;
    uint32_t cut_y;

    // Topleft_x
    if (a.top_left.x > b.top_left.x) {
    	i_x = 1; // Cas 1
    	c.top_left.x = a.top_left.x;
    	cut_x = b.size.width-(a.top_left.x-b.top_left.x);
    } else {
    	i_x = 2; // Cas 2
    	c.top_left.x = b.top_left.x;
    	cut_x = a.size.width-(b.top_left.x-a.top_left.x);
    }

	// Size width
	if (i_x == 1) { // Cas 1 
		if (cut_x < a.size.width) {
			c.size.width = cut_x;
		} else {
			c.size.width = a.size.width;
		}
	} else { // Cas 2
		if (cut_x < b.size.width) {
			c.size.width = cut_x;
		} else {
			c.size.width = b.size.width;
		}
	}

    // Topleft_y
    if (a.top_left.y > b.top_left.y) {
    	i_y = 1; // Cas 1
    	c.top_left.y = a.top_left.y;
    	cut_y = b.size.height-(a.top_left.y-b.top_left.y);
    } else {
    	i_y = 2; // Cas 2
    	c.top_left.y = b.top_left.y;
    	cut_y = a.size.height-(b.top_left.y-a.top_left.y);
    }

	// Size height
	if (i_y == 1) { // Cas 1 
		if (cut_y < a.size.height) {
			c.size.height = cut_y;
		} else {
			c.size.height = a.size.height;
		}
	} else { // Cas 2
		if (cut_y < b.size.height) {
			c.size.height = cut_y;
		} else {
			c.size.height = b.size.height;
		}
	}

    return c;
}


