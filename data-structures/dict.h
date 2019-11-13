#ifndef DICT_H
#define DICT_H

typedef int T;

struct avl_node_t {
    char * key;
    T val;
    struct avl_node_t * left, * right;
    int height;
};

typedef struct dict_t {
    struct avl_node_t * root;
    unsigned size;
} dict_t;

dict_t create_dict(void);
void clear_dict(dict_t * dict);
struct avl_node_t * dict_add(dict_t * dict, const char * key, T value);
struct avl_node_t * dict_get(dict_t dict, const char * key);
void dict_set(dict_t * dict, const char * key, T value);
void dict_delete(dict_t * dict, const char * key);

#endif
