typedef long unsigned int list_data_t;  // to change the type of the objects stored by the list, just change this typedef

typedef struct list_elem {
    struct list_elem *next;  // Zeiger auf das naechste Element
    list_data_t *data;       // Zeiger auf ein Datenobject
} list_elem_t;

typedef struct list {
    struct list_elem *first;  // erstes Element in der Liste
    struct list_elem *last;   // letztes Element in der Liste
} list_t;

/* function prototypes */
list_t *list_init();
struct list_elem *list_insert(list_t *list, list_data_t *data);
struct list_elem *list_append(list_t *list, list_data_t *data);
int list_remove(list_t *list, struct list_elem *elem);
void list_finit(list_t *list);
void list_print(list_t *list, void (*print_elem)(list_data_t *));
struct list_elem *list_find(list_t *list, list_data_t *data, int (*cmp_elem)(const list_data_t *, const list_data_t *));
