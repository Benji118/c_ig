/**
 *  @file	ei_bindinglist.h
 *  @brief	Hosts the declaration of binding types and global binding list.
 *
 *  \author
 *  Poulet
 *  06.2014
 *
 */
#ifndef EI_BINDINGLIST_H
#define EI_BINDINGLIST_H

#include "ei_types.h"
#include "ei_widget.h"


typedef struct ei_binding_t {
    ei_eventtype_t eventtype;
    ei_widget_t* widget;
    ei_tag_t tag;
    ei_callback_t callback;
    void* user_param;
} ei_binding_t;

typedef struct ei_bindingcell_t ei_bindingcell_t;

struct ei_bindingcell_t {
    ei_binding_t binding;
    ei_bindingcell_t* next;
};

// Liste globale de bindings
ei_bindingcell_t* bindinglist;

static inline void ei_unbind_everything()
{
    ei_bindingcell_t* handle = bindinglist;
    while (handle != NULL) {
        bindinglist = handle->next;
        free(handle);
        handle = bindinglist;
    }
}

#endif
