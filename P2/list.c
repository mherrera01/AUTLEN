#include "list.h"

typedef struct _NodeList{ /* Private EdD, necessary to imlistement list */
    void *info;
    struct _NodeList *next;
} NodeList; /* Private NodeList type */

struct _List{
    NodeList *last; /* The LEC points to the last node and the last to the first */
    destroy_element_function_type destroy_element_function;
    copy_element_function_type copy_element_function;
    print_element_function_type print_element_function;
    cmp_element_function_type cmp_element_function;
};

NodeList* nodelist_ini(){
    NodeList *pnode = (NodeList*) malloc(sizeof(NodeList));
    
    if (pnode == NULL) 
        return NULL;    
    
    pnode->info = NULL;
    pnode->next = NULL;
    
    return pnode;
}

/* Initializes the list by saving memory and initialize all its elements.*/
List* list_ini (destroy_element_function_type f1, copy_element_function_type f2, print_element_function_type f3, cmp_element_function_type f4){
    List * list = (List*) malloc(sizeof(List));

    if (list == NULL) return NULL;

    list->last = NULL;
    list->destroy_element_function = f1;
    list->copy_element_function = f2;
    list->print_element_function = f3;
    list->cmp_element_function = f4;

    return list;
}

/* Frees the list, freeing all its elements. */
void list_destroy (List* list){
    NodeList* nlpointer = list->last, *paux = NULL;
    int i, times;
    if(!list) return;
    times = list_size(list);
    for(i=0; i<times; i++){
        paux = nlpointer->next;
        list->destroy_element_function(nlpointer->info); 
        free(nlpointer);
        nlpointer = paux;
    }
    
    free(list); 
    list = NULL;
}

/* Inserts at the beginning of the list making a copy of the information received. */
int list_insertFirst (List* list, const void *pelem){
    NodeList *pnode;
    if (list == NULL || pelem == NULL){
        return 1;
    }

    pnode = nodelist_ini();
    pnode->info = list->copy_element_function(pelem);
    switch(list_isEmpty(list)){
        case 1:
            list->last = pnode;
            pnode->next = pnode;
            return 0;
        case 0:
            pnode->next = list->last->next;
            list->last->next = pnode;
            return 0;
        default:
            return 1;
    }
}

/* Inserts at the end of the list making a copy of the information received. */
int list_insertLast (List* list, const void *pelem){
    NodeList *pnode;
    if(list == NULL || pelem == NULL){
        return 1;
    }

    pnode = nodelist_ini();
    pnode->info = list->copy_element_function(pelem);
    switch(list_isEmpty(list)){
        case 1:
            list->last = pnode;
            pnode->next = pnode;
            return 0;
        case 0:
            pnode->next = list->last->next;
            list->last->next = pnode;
            list->last = pnode;

            return 0;
        default:
    /*If the switch statement or the list_isEmpty function goes wrong*/
            return 1;
    }
    return 0;
}

/* Inserts in order in the list making a copy of the element. */
int list_insertInOrder (List *list, const void *pelem){
    NodeList *pnode, *aux;
    
    if (list == NULL || pelem == NULL)
        return 1;
    
    pnode = nodelist_ini();
    
    if (pnode == NULL)
        return 1;

    pnode->info = list->copy_element_function(pelem);
    
    if (list_isEmpty(list)){
        pnode->next = pnode;
        list->last = pnode;
        return 0;
    }
    aux=list->last->next;
    
    if (list->cmp_element_function(pelem, aux->info) < 0){
        list->destroy_element_function(pnode->info);
        free(pnode);
        return list_insertFirst (list, pelem);
    }
    while ((aux != list->last) && (list->cmp_element_function(pelem, aux->next->info) > 0)) 
        aux = aux->next;

    if (aux == list->last){
        list->last = pnode;
    }

    pnode->next = aux->next;
    aux->next = pnode;
    
    return 0;
}

/* Extracts from the beginning of the list, returning directly the pointer to the info field of the extracted node, node that is
finally released. BEWARE: after saving the address of the info field to be returned and before releasing the node, it sets the
info field of the node to NULL, so that it does not continue pointing to the info to be returned and, therefore, does not
release it when releasing the node */
void * list_extractFirst (List* list){
    NodeList *paux;
    void* pelem;
    
    if (!list || list_isEmpty(list))
        return NULL;
            
    pelem = list->last->next->info;
    list->last->next->info = NULL;

    if (list->last->next == list->last) {
	    list->destroy_element_function(list->last);
	    list->last = NULL;
    } 
    else {
        paux = list->last->next;
        list->last->next = paux->next;
        list->destroy_element_function(paux);
    }
    return pelem;
}

/* Extracts from the end of the list, directly returning the pointer to the info field of the extracted node, node that is finally
released. BEWARE: after saving the address of the info field to be returned and before releasing the node, it sets the info
field of the node to NULL, so that it does not continue pointing to the info to be returned and, therefore, does not release it
when releasing the node */
void * list_extractLast (List* list){
    NodeList *paux;
    void* pelem;
    
    if (!list || list_isEmpty(list))
        return NULL;

    pelem = list->last->info;
    list->last->info = NULL;
    
    if (list->last->next == list->last) {
        list->destroy_element_function(list->last);
        list->last = NULL;
        return pelem;
    } 

    for (paux=list->last; paux->next!=list->last; paux=paux->next);

    paux->next = list->last->next;
    list->destroy_element_function(list->last);
    list->last = paux;
    return pelem;
}

/* Extracts the element in the position given from the list, directly returning the pointer to the info field of the extracted node,
node that is finally released. BEWARE: after saving the address of the info field to be returned and before releasing the node, it sets the info
field of the node to NULL, so that it does not continue pointing to the info to be returned and, therefore, does not release it
when releasing the node */
void * list_extractElement (List* list, int index){
    NodeList *aux = NULL, *paux = NULL;
    int i=-1;
    void *pelem = NULL;

    if (!list) return NULL;
    if (list_isEmpty(list) || index > list_size(list)) return NULL;

    /* Gets the element of the list */
    aux = list->last;
    do {
        i++;
        aux = aux->next;
    } while ((aux != list->last) && (i<index));

    if (i==index){
        pelem = aux->info;
        aux->info = NULL;

        /* Checks if there is only one element in the list */
        if (list->last->next == list->last) {
            list->destroy_element_function(list->last);
            list->last = NULL;
            return pelem;
        }

        /* Gets the previous element to the one given */
        for (paux=list->last; paux->next!=aux; paux=paux->next);

        paux->next = aux->next;

        if (aux == list->last){
            list->last = paux;
        }
        list->destroy_element_function(aux);

        return pelem;
    }

    return NULL;
}

/* Checks if a list is empty or not. */
int list_isEmpty (const List* list){
    if (!list) return 1;
    return(list->last == NULL);
}

/* Returns the information stored in the i-th node of the list. In case of 1, returns NULL. */
void* list_get (const List* list, int index){
    NodeList *aux;
    int i=-1;
    if (!list) return NULL;

    if (list_isEmpty(list) || index > list_size(list)) return NULL;
    aux = list->last;
    do {
        i++;
        aux = aux->next;
    } while ((aux != list->last) && (i<index));
    if (i==index) return aux->info;
    else return NULL;
}

/* Returns the number of elements in a list. */
int list_size (const List* list){
    NodeList *aux = list->last;
    int i = 0;
    if (aux == NULL)
        return -1;
    do {
        i++;
        aux = aux->next;
    } while (aux != list->last);
    return i;
}

/* Prints a list returning the number of written characters. */
int list_print (FILE *fd, const List* list){
    NodeList *paux;
    int count = 0;
    if (!list) return -1;
    if (list_isEmpty(list) == 1) return -1;

    count += fprintf (fd, "List with %d elements:\n", list_size(list));

    paux = list->last;
    do {
        paux = paux->next;
        count += list->print_element_function(fd, paux->info) + fprintf(fd, "\n");
    } while (paux != list->last);

    return count;

}

/* Returns 1 if the element is contained in the list */
int list_contains (List* list, void* element){
    int i;

    if (list == NULL || element == NULL) return 0;

    for(i = 0; i < list_size(list); i++){
        if((list->cmp_element_function(element, list_get(list, i))) == 0) return 1;
    }

    return 0;
}

/* Returns the position in the list of the element given, -1 otherwise */
int list_getIndex (List* list, void* element){
    int i;

    if (list == NULL || element == NULL) return -1;

    /* Vemos si la lista contiene el elemento */
    if (!list_contains(list, element)) return -1;

    /* Obtenemos en qué posición de la lista está el elemento */
    for (i=0; i< list_size(list); i++){
        if((list->cmp_element_function(element, list_get(list, i))) == 0) return i;
    }

    return -1;
}
