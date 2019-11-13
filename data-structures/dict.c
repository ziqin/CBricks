#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"

struct dict_t create_dict(void) {
    struct dict_t tree;
    tree.root = NULL;
    tree.size = 0;
    return tree;
}

inline static struct avl_node_t * _create_avl_node(const char * key, T value) {
    struct avl_node_t * new_node = malloc(sizeof(struct avl_node_t));
    assert(new_node != NULL);
    new_node->key = strdup(key);
    new_node->val = value;
    new_node->left = new_node->right = NULL;
    new_node->height = 0;
    return new_node;
}

inline static void _destroy_avl_node(struct avl_node_t * node) {
    free(node->key);
    free(node);
}

void _destroy_dict(struct avl_node_t * tree) {
    assert(tree != NULL);
    if (tree->left != NULL) _destroy_dict(tree->left);
    if (tree->right != NULL) _destroy_dict(tree->right);
    _destroy_avl_node(tree);
}

void clear_dict(dict_t * tree) {
    _destroy_dict(tree->root);
    tree->root = NULL;
    tree->size = 0;
}

#define _AVL_HEIGHT(tree) ((tree) ? (tree)->height : -1)

inline static void _avl_update_height(struct avl_node_t * tree) { // tree != NULL
    int left_height = _AVL_HEIGHT(tree->left);
    int right_height = _AVL_HEIGHT(tree->right);
    tree->height = (left_height > right_height ? left_height : right_height) + 1;
}

inline static struct avl_node_t * _avl_left_rotate(struct avl_node_t * tree) {
    struct avl_node_t * rotated = tree->right;
    tree->right = rotated->left;
    _avl_update_height(tree);
    rotated->left = tree;
    _avl_update_height(rotated);
    return rotated;
}

inline static struct avl_node_t * _avl_right_rotate(struct avl_node_t * tree) {
    struct avl_node_t * rotated = tree->left;
    tree->left = rotated->right;
    _avl_update_height(tree);
    rotated->right = tree;
    _avl_update_height(rotated);
    return rotated;
}

static struct avl_node_t * _avl_maintain(struct avl_node_t * tree) {
    _avl_update_height(tree);
    struct avl_node_t * left = tree->left, * right = tree->right;
    if (_AVL_HEIGHT(left) > _AVL_HEIGHT(right) + 1) {
        if (_AVL_HEIGHT(left->left) >= _AVL_HEIGHT(left->right)) {
            return _avl_right_rotate(tree);
        } else {
            tree->left = _avl_left_rotate(tree->left);
            return _avl_right_rotate(tree);
        }
    } else if (_AVL_HEIGHT(left) + 1 < _AVL_HEIGHT(right)) {
        if (_AVL_HEIGHT(right->right) >= _AVL_HEIGHT(right->left)) {
            return _avl_left_rotate(tree);
        } else {
            tree->right = _avl_right_rotate(tree->right);
            return _avl_left_rotate(tree);
        }
    } else {
        return tree;
    }
}

static struct avl_node_t * _avl_insert(struct avl_node_t * tree, struct avl_node_t * node, unsigned * size) {
    int cmp = strcmp(node->key, tree->key);
    if (cmp < 0) {
        if (tree->left != NULL) {
            tree->left = _avl_insert(tree->left, node, size);
        } else {
            tree->left = node;
            ++(*size);
        }
    } else if (cmp > 0) {
        if (tree->right != NULL) {
            tree->right = _avl_insert(tree->right, node, size);
        } else {
            tree->right = node;
            ++(*size);
        }
    } else {
        _destroy_avl_node(node);
    }
    return tree = _avl_maintain(tree);
}

static struct avl_node_t * _avl_min(struct avl_node_t * tree) {
    assert(tree != NULL);
    while (tree->left != NULL)
        tree = tree->left;
    return tree;
}

/** Note that min node is not deleted using free() intentionally */
static struct avl_node_t * _avl_exclude_min(struct avl_node_t * tree) {
    assert(tree != NULL);
    if (tree->left == NULL) { // current node excluded but not deleted
        tree = tree->right;
    } else {
        // min in left subtree
        tree->left = _avl_exclude_min(tree->left);
        tree = _avl_maintain(tree);
    }
    return tree;
}

// Reference: Algorithms (4th ed.) by R. Sedgewick & K. Wayne
static struct avl_node_t * _avl_delete(struct avl_node_t * tree, const char * key) {
    // if (tree == NULL) return NULL;  // nothing deleted
    assert(tree != NULL);  // inexistent key causes an assertion error
    int cmp = strcmp(key, tree->key);
    if (cmp < 0) {
        tree->left = _avl_delete(tree->left, key);
        tree = _avl_maintain(tree);
    } else if (cmp > 0) {
        tree->right = _avl_delete(tree->right, key);
        tree = _avl_maintain(tree);
    } else { // delete current node
        struct avl_node_t * current = tree;
        if (current->right == NULL) {
            tree = current->left;
        } else if (current->left == NULL) {
            tree = current->right;
        } else { // we know that right subtree is not empty
            tree = _avl_min(current->right);  // find successor
            tree->right = _avl_exclude_min(current->right); // successor deleted
            tree->left = current->left;
            tree = _avl_maintain(tree);
        }
        _destroy_avl_node(current);
    }
    return tree;
}

/**
 * Returns the address of inserted node, or NULL if tree already contains the same key
 * When duplicate, values won't be updated
 */
struct avl_node_t * dict_add(dict_t * tree, const char * key, T value) {
    assert(tree != NULL);
    // create a new node
    struct avl_node_t * new_node = _create_avl_node(key, value);
    // insert
    if (tree->root == NULL) {
        tree->size = 1;
        tree->root = new_node;
    } else {
        unsigned old_size = tree->size;
        tree->root = _avl_insert(tree->root, new_node, &(tree->size));
        if (tree->size == old_size) new_node = NULL;
    }
    return new_node;
}

/** Returns the address of a node with given key, or NULL if not found */
struct avl_node_t * dict_get(dict_t tree, const char * key) {
    struct avl_node_t * cursor = tree.root;
    while (cursor != NULL) {
        int cmp = strcmp(key, cursor->key);
        if (cmp < 0) cursor = cursor->left;
        else if (cmp > 0) cursor = cursor->right;
        else break;
    }
    return cursor;
}

void dict_set(dict_t * tree, const char * key, T value) {
    struct avl_node_t * cursor = dict_get(*tree, key);
    if (cursor) {
        cursor->val = value;
    } else {
        dict_add(tree, key, value);
    }
}

/** inexistent key will cause an error */
void dict_delete(dict_t * tree, const char * key) {
    assert(tree != NULL);
    tree->root = _avl_delete(tree->root, key);
    tree->size--;
}
