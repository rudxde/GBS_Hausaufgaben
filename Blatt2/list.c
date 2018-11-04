#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/*
 * Initialisiert eine neue Liste und liefert einen Zeiger auf die Basisstruktur zurück.
 * Liefert einen Zeiger auf die Liste zurück oder NULL, falls ein Fehler aufgetreten ist.
 */
list_t *list_init()
{
    list_t *newList = malloc(sizeof(list_t));
    if (newList == NULL)
    {
        return NULL;
    }
    newList->first = NULL;
    newList->last = NULL;
    return newList;
}

/*
 * Erzeugt ein neues Listenelement und fügt es am Beginn der Liste ein.
 * Liefert einen Zeiger auf das neue Listenelement zurück oder NULL, falls ein Fehler aufgetreten ist. 
 **/
struct list_elem *list_insert(list_t *list, char *data)
{
    if (list == NULL)
    {
        return NULL;
    }
    struct list_elem *newListElement = malloc(sizeof(struct list_elem));
    if (newListElement == NULL)
    {
        return NULL;
    }
    newListElement->data = data;
    newListElement->next = list->first;
    list->first = newListElement;
    if (list->last == NULL)
    {
        list->last = newListElement;
    }
    return newListElement;
}

/*
 * Erzeugt ein neues Listenelement und fügt es am Ende der Liste ein.
 * Liefert einen Zeiger auf das neue Listenelement zurück oder NULL, falls ein Fehler aufgetreten ist.
 **/
struct list_elem *list_append(list_t *list, char *data)
{
    if (list == NULL)
    {
        return NULL;
    }
    struct list_elem *newListElement = malloc(sizeof(struct list_elem));
    if (newListElement == NULL)
    {
        return NULL;
    }
    newListElement->data = data;
    newListElement->next = NULL;
    if (list->last != NULL)
    {
        list->last->next = newListElement;
    }
    list->last = newListElement;
    if (list->first == NULL)
    {
        list->first = newListElement;
    }
    return newListElement;
}

/*
 * Durchsucht die Liste list von vorne und entfernt das Element list_elem, sofern es gefunden wird.
 * Die Datenstruktur von elem wird ebenfalls freigegeben, die eigentlichen Daten hingegen nicht,
 * denn es ist nicht bekannt, ob diese noch gebraucht werden oder überhaupt dynamisch alloziert wurden.
 * Liefert 0 zurück, wenn das Element gefunden wurde, andernfalls -1. 
 **/
int list_remove(list_t *list, struct list_elem *elem)
{
    if(list.first == elem){
        list.first = list.first.next;
        free(elem);
        return 0;
    }
    struct list_elem *actualListElement = list->first;
    while (actualListElement != NULL)
    {
        if (actualListElement->next == elem)
        {
            struct list_elem *elementToDelete = actualListElement->next;
            actualListElement->next = actualListElement->next->next;
            free(elementToDelete);
            return 0;
        }
        else
        {
            actualListElement = actualListElement->next;
        }
    }
    return -1;
}

/*
 * Entfernt alle Element aus der Liste und gibt die Basisdatenstruktur list frei. 
 **/
void list_finit(list_t *list)
{
    struct list_elem *actualListElement = list->first;
    struct list_elem *nextListElement = list->first;
    while (actualListElement != NULL)
    {
        nextListElement = actualListElement->next;
        free(actualListElement);
        actualListElement = nextListElement;
    }
    list->first = NULL;
    list->last = NULL;
}

/*
 * Geht die Elemente der Liste von vorne bis hinten durch und ruft für jedes Element die als zweiten Parameter
 * übergebene Funktion mit dem Argument data aus der Struktur struct list_elem auf. Für die Ausgabe werden die Elemente in
 * aufsteigender Reihefolge (beginnend bei 1) durchnumeriert. 
 **/
void list_print(list_t *list, void (*print_elem)(char *))
{
    struct list_elem *actualListElement = list->first;
    unsigned counter = 1;
    while (actualListElement != NULL)
    {
        printf("%u:", counter++);
        print_elem(actualListElement->data);
        printf("\n");
        actualListElement = actualListElement->next;
    }
}

/*
 * Sucht in der Liste ein Element, das den Daten des angegebenen Elements entsprechend einer anzugebenden Vergleichsfunktion entspricht.
 * Die Funktion soll die Liste von vorne nach hinten durchgehen und das erste Element zurückliefern, dessen Daten dem übergebenen data entsprechen.
 * In unserem Fall sind ist der data-Teil jedes Elements jeweils eine Zeichenkette (String),
 * so dass als Vergleichfunktion einfach strncmp() verwendet werden kann.
 * Die Funktion muss also auf jedes Element der Liste und den übergebenen Parameter data jeweils die Vergleichsfunktion cmp_elem anwenden.
 * Eine Übereinstimmung liegt vor, wenn die Vergleichsfunktion 0 zurückliefert.
 **/
struct list_elem *list_find(list_t *list, char *data, int (*cmp_elem)(const char *, const char *))
{
    struct list_elem *actualListElement = list->first;
    while (actualListElement != NULL)
    {
        char isSame = cmp_elem(data, actualListElement->data);
        if(isSame) {
            return actualListElement;
        }
        actualListElement = actualListElement->next;
    }
}
