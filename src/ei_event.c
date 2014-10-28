/**
 * @file ei_event.c
 * @brief Allows the binding and unbinding of callbacks to events.
 *
 * \author 
 * Equipe poulet
 * 02.06.14
 *
 */

#include <stdlib.h>
#include "ei_event.h"
#include "ei_bindinglist.h"

// Suppression d'un binding de la liste
void ei_delete_binding(ei_binding_t binding)
{
    ei_bindingcell_t* handle = bindinglist;
    ei_bindingcell_t* prec = NULL;
    ei_bool_t iseq;
    while (handle != NULL) {
        iseq = EI_TRUE;

        // Gros machin juste pour comparer deux bindings
        iseq = iseq && (handle->binding.eventtype == binding.eventtype);
        iseq = iseq && (handle->binding.widget == binding.widget);
        iseq = iseq && (handle->binding.tag == binding.tag);
        iseq = iseq && (handle->binding.callback == binding.callback);
        // cette ligne est facultative
        // iseq = iseq && (handle->binding.user_param == binding.user_param);

        if (iseq) {
            if (prec != NULL){
                prec->next = handle->next;
            } else {
                bindinglist = handle->next;
            }
            free(handle);
        } else {
            prec = handle;
            handle = handle->next;
        }
    }
}

// Insertion d'un binding en tête de la liste globale
void ei_bind  (ei_eventtype_t eventtype,
        ei_widget_t* widget,
        ei_tag_t tag,
        ei_callback_t callback,
        void*  user_param)
{
    ei_bindingcell_t* new_binding =
        (ei_bindingcell_t*)malloc(sizeof(ei_bindingcell_t));
    new_binding->binding.eventtype = eventtype;
    new_binding->binding.widget = widget;
    new_binding->binding.tag = tag;
    new_binding->binding.callback = callback;
    new_binding->binding.user_param = user_param;

    new_binding->next = bindinglist;
    bindinglist = new_binding;
}

void ei_unbind (ei_eventtype_t eventtype,
        ei_widget_t* widget,
        ei_tag_t tag,
        ei_callback_t callback,
        void*  user_param)
{
    ei_binding_t binding;
    binding.eventtype = eventtype;
    binding.widget = widget;
    binding.tag = tag;
    binding.callback = callback;
    binding.user_param = user_param;

    ei_delete_binding(binding);
}

