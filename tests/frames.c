#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget.h"
#include "ei_geometrymanager.h"
#include "ei_types.h"
#include "ei_utils.h"



/*
 * process_key --
 *
 * Callback called when any key is pressed by the user.
 * Simply looks for the "Escape" key to request the application to quit.
 */
ei_bool_t process_key(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
	if (event->param.key.key_sym == SDLK_ESCAPE) {
		ei_app_quit_request();
		return EI_TRUE;
	}
	return EI_FALSE;
}



int ei_main(int argc, char** argv)
{

	// Explications
	printf("FRAME DEMONSTRATION\n");
	printf("Voici des frames dans différentes configurations :\n");
	printf("- relief \"raised\", \"sunken\" ou aucun relief\n");
	printf("- texte ou image\n");
	printf("- inclusion de cadre dans d'autres (parenté)\n");
	printf("- un cadre en cachant un autre car créé après\n");
	printf("\nAppuyez sur échap pour quitter\n\n");
	
	
	/*****************************************************************/
	// Parametres de la fenetre racine
	ei_size_t		screen_size = {700, 700};
	ei_color_t		root_bgcol = {0x40, 0x40, 0x40, 0xFF};
	char*			root_text = "Racine ! (west anchor)";
	/*****************************************************************/
	// Paramètres communs
	int				frame_border_width = 6;
	ei_anchor_t		anc_west = ei_anc_west;
	ei_color_t		white = {0xFF, 0xFF, 0xFF, 0xFF};
	/*****************************************************************/
	// Parametres de la fenetre "frame1"
	ei_widget_t*	frame1;
	ei_size_t		frame1_size = {400,300};
	ei_color_t		frame1_color = {0xb4, 0x7f, 0x52, 0xFF};
	ei_relief_t		frame1_relief = ei_relief_raised;
	int 			frame1_x = 10;
	int 			frame1_y = 10;				 
	/*****************************************************************/
	// Paramètres de la fenetre "frame2"
 	ei_widget_t*	frame2;
	ei_size_t		frame2_size = {200,200};
	ei_color_t		frame2_color = {0xFF, 0x66, 0x00, 0xFF};
	ei_relief_t		frame2_relief = ei_relief_sunken;
	int 			frame2_x = 450;
	int 			frame2_y = 300;
	char*			frame2_text = "Ceci est un cadre...";
	/*****************************************************************/
	// Paramètres de la troisième fenetre "frame3"
	ei_widget_t*	frame3;
	ei_size_t		frame3_size = {600,100};
	int				frame3_x = 50;
	int				frame3_y = 450;
	ei_color_t		frame3_color = {0x33, 0x33, 0xD0, 0xFF};
	ei_relief_t 	frame3_relief = ei_relief_none;
	char*			frame3_text = "Ceci est un cadre sans relief dessiné après le orange";
	ei_color_t		frame3_text_color = {0x00, 0x00, 0x00, 0x00};
	/*****************************************************************/
	// Paramètres de la quatrième fenetre "frame4"
	ei_widget_t*	frame4;
	ei_size_t		frame4_size = {300,100};
	int				frame4_x = 200;
	int				frame4_y = 100;
	ei_color_t		frame4_color = {0xFF, 0x33, 0x33, 0xFF};
	ei_relief_t 	frame4_relief = ei_relief_raised;
	char*			frame4_text = "Bébé cadre";
	ei_color_t		frame4_text_color = {0x00, 0xFF, 0xFF, 0x00};
	/*****************************************************************/
	// Paramètres de la fenetre "frame5"
	ei_widget_t*	frame5;
	ei_size_t		frame5_size = {200,100};
	int				frame5_x = 200;
	int				frame5_y = 570;
	ei_color_t		frame5_color = {0xFF, 0x00, 0x66, 0xFF};
	ei_relief_t 	frame5_relief = ei_relief_raised;
	char*			frame5_text = "relief raised";
	ei_color_t		frame5_text_color = {0x66, 0x00, 0x99, 0xFF};
	/*****************************************************************/
	// Paramètres de la fenetre "frame6"
	ei_widget_t*	frame6;
	ei_size_t		frame6_size = {200,100};
	int				frame6_x = 450;
	int				frame6_y = 570;
	ei_color_t		frame6_color = {0xFF, 0x00, 0x66, 0xFF};
	ei_relief_t 	frame6_relief = ei_relief_sunken;
	char*			frame6_text = "relief sunken";
	ei_color_t		frame6_text_color = {0x66, 0x00, 0x99, 0xFF};
	/*****************************************************************/
	
	/* Create the application and change the color of the background. */
	ei_app_create(&screen_size, EI_FALSE);
	ei_widget_t* root = ei_app_root_widget();
	static char*	image_filename = "misc/klimt.jpg";
	ei_surface_t	image = hw_image_load(image_filename,ei_app_root_surface());
	ei_size_t		image_size = hw_surface_get_size(image);
	ei_rect_t		img_rect = ei_rect(root->screen_location.top_left,image_size);
	ei_rect_t*		img_rect_ptr = &img_rect;
	ei_frame_configure(root, NULL, &root_bgcol, NULL, NULL, &root_text, NULL, &white, &anc_west, NULL, NULL, NULL);
	
	/* Create, configure and place the frame1 on screen. */
 	frame1 = ei_widget_create("frame", ei_app_root_widget());
 	ei_frame_configure(frame1, &frame1_size, &frame1_color, &frame_border_width, &frame1_relief,
 						NULL, NULL, NULL, NULL, &image, &img_rect_ptr, NULL);
 	ei_place(frame1, NULL, &frame1_x, &frame1_y, NULL, NULL, NULL, NULL, NULL, NULL);
 
 	/* Create, configure and place the frame2 on screen. */
 	frame2 = ei_widget_create("frame", ei_app_root_widget());
 	ei_frame_configure(frame2, &frame2_size, &frame2_color, &frame_border_width, &frame2_relief,
 						&frame2_text, NULL, &white, NULL, NULL, NULL, NULL);
 	ei_place(frame2, NULL, &frame2_x, &frame2_y, NULL, NULL, NULL, NULL, NULL, NULL );
 	
 	/* Create, configure and place the frame3 on screen. */
 	frame3 = ei_widget_create("frame", ei_app_root_widget());
 	ei_frame_configure(frame3, &frame3_size, &frame3_color, &frame_border_width, &frame3_relief,
 						&frame3_text, NULL, &frame3_text_color, NULL, NULL, NULL, NULL);
 	ei_place(frame3, NULL, &frame3_x, &frame3_y, NULL, NULL, NULL, NULL, NULL, NULL );
 	
 	/* Create, configure and place the frame4 on screen. */
 	frame4 = ei_widget_create("frame", frame1);
 	ei_frame_configure(frame4, &frame4_size, &frame4_color, &frame_border_width, &frame4_relief,
 						&frame4_text, NULL, &frame4_text_color, NULL, NULL, NULL, NULL);
 	ei_place(frame4, NULL, &frame4_x, &frame4_y, NULL, NULL, NULL, NULL, NULL, NULL );
 	
 	/* Create, configure and place the frame4 on screen. */
 	frame5 = ei_widget_create("frame", ei_app_root_widget());
 	ei_frame_configure(frame5, &frame5_size, &frame5_color, &frame_border_width, &frame5_relief,
 						&frame5_text, NULL, &frame5_text_color, NULL, NULL, NULL, NULL);
 	ei_place(frame5, NULL, &frame5_x, &frame5_y, NULL, NULL, NULL, NULL, NULL, NULL );
 	
 	/* Create, configure and place the frame4 on screen. */
 	frame6 = ei_widget_create("frame", ei_app_root_widget());
 	ei_frame_configure(frame6, &frame6_size, &frame6_color, &frame_border_width, &frame6_relief,
 						&frame6_text, NULL, &frame6_text_color, NULL, NULL, NULL, NULL);
 	ei_place(frame6, NULL, &frame6_x, &frame6_y, NULL, NULL, NULL, NULL, NULL, NULL );

	/* Hook the keypress callback to the event. */
	ei_bind(ei_ev_keydown, NULL, "all", process_key, NULL);

	/* Run the application's main loop. */
	ei_app_run();

	/* We just exited from the main loop. Terminate the application (cleanup). */
	ei_unbind(ei_ev_keydown, NULL, "all", process_key, NULL);

	// We just exited from the main loop. Terminate the application (cleanup).
	ei_app_free();

	return (EXIT_SUCCESS);
}
