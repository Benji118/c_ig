/**
 * @file	ei_draw_tools.h
 * @brief	Tools to draw widget
 * 
 * \author
 * Equipe poulet
 * 06.2014
 *
 */

#ifndef EI_DRAW_TOOLS_H
#define EI_DRAW_TOOLS_H

#include "ei_types.h"
#include "ei_widget.h"

#define M_PI 3.14159265358979323846



// Requiert : 	- un pointeur sur un widget
//				- un entier correspondant à la largeur de la bordure en pixels
// Retourne :	une liste chaînée de points correspondant au cadre
//				rectangulaire du widget diminué de la bordure 
// Remarque :	désallocation de la liste nécessaire après utilisation
extern ei_linked_point_t* ei_frame_calculation(ei_widget_t* widget, int border_width);



// Désalloue la mémoire allouée pour une liste chaînée de points
extern void ei_free_linked_points_list(ei_linked_point_t* liste);



// Requiert : liste1 et liste2 deux listes simplement chaînée de points bien allouées
// Retourne : liste1 = liste1&liste2
extern void concat_lists(ei_linked_point_t** liste1, ei_linked_point_t** liste2);



// Retourne une couleur éclaircie de la couleur donnée
// A utiliser après du blanc pour ei_relief_raised
// A utiliser après du noir pour ei_relief_sunken
extern ei_color_t ei_clearer_color(ei_color_t color);



// Dessine le texte d'un widget
extern void ei_draw_widget_text(	ei_surface_t	surface,
									ei_widget_t*	widget,
									char*			text,
									ei_font_t		text_font,
									ei_color_t		text_color,
									ei_anchor_t		anchor		);



// Dessine l'image d'un widget
extern void ei_draw_widget_img(	ei_surface_t	surface,
								ei_widget_t*	widget,
								ei_surface_t	img,
								ei_rect_t*		img_rect,
								ei_anchor_t		anchor,
								int				button_border_width	);



// Retourne : une liste de points représentant un arc de cercle
// A utiliser dans le dessin des boutons
extern ei_linked_point_t* ei_draw_arc(ei_point_t centre, float R, float angledebut, float anglefin);



// Retourne : une liste de points représentant le coutour arrondi d'un bouton
extern ei_linked_point_t* ei_curvy_contour_button(ei_bool_t high, ei_bool_t low, ei_rect_t rect, float R, int border);



// Renvoie le rectangle formé par l'intersection de deux rectangles
extern ei_rect_t ei_rectangles_intersect(ei_rect_t a, ei_rect_t b);



#endif
