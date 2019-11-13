#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


typedef int T;

struct avl_node_t {
    int key;
    T val;
    struct avl_node_t * left, * right;
    int height;
};

struct avl_tree_t {
    struct avl_node_t * root;
    unsigned size;
};

struct avl_tree_t create_avl_tree(void) {
    struct avl_tree_t tree;
    tree.root = NULL;
    tree.size = 0;
    return tree;
}

void _destroy_avl_tree(struct avl_node_t * tree) {
    assert(tree != NULL);
    if (tree->left != NULL) _destroy_avl_tree(tree->left);
    if (tree->right != NULL) _destroy_avl_tree(tree->right);
    free(tree);
}

void clear_avl_tree(struct avl_tree_t * tree) {
    _destroy_avl_tree(tree->root);
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

struct avl_node_t * _avl_maintain(struct avl_node_t * tree) {
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

struct avl_node_t * _avl_insert(struct avl_node_t * tree, struct avl_node_t * node, unsigned * size) {
    if (node->key < tree->key) {
        if (tree->left != NULL) {
            tree->left = _avl_insert(tree->left, node, size);
        } else {
            tree->left = node;
            ++(*size);
        }
    } else if (tree->key < node->key) {
        if (tree->right != NULL) {
            tree->right = _avl_insert(tree->right, node, size);
        } else {
            tree->right = node;
            ++(*size);
        }
    } else {
        free(node);
    }
    return tree = _avl_maintain(tree);
}

/**
 * Returns the address of inserted node, or NULL if tree already contains the same key
 * When duplicate, values won't be updated
 */
struct avl_node_t * avl_insert(struct avl_tree_t * tree, int key, T value) {
    assert(tree != NULL);
    // create a new node
    struct avl_node_t * new_node = malloc(sizeof(struct avl_node_t));
    assert(new_node != NULL);
    new_node->key = key;
    new_node->val = value;
    new_node->left = new_node->right = NULL;
    new_node->height = 0;
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

/** Returns the address of a node with given key */
struct avl_node_t * avl_search(struct avl_tree_t * tree, int key) {
    assert(tree != NULL);
    struct avl_node_t * cursor = tree->root;
    while (cursor != NULL) {
        if (key < cursor->key) cursor = cursor->left;
        else if (cursor->key < key) cursor = cursor->right;
        else break;
    }
    return cursor;
}

struct avl_node_t * _avl_min(struct avl_node_t * tree) {
    assert(tree != NULL);
    while (tree->left != NULL)
        tree = tree->left;
    return tree;
}

/** Note that min node is not deleted using free() intentionally */
struct avl_node_t * _avl_exclude_min(struct avl_node_t * tree) {
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
struct avl_node_t * _avl_delete(struct avl_node_t * tree, int key) {
    // if (tree == NULL) return NULL;  // nothing deleted
    assert(tree != NULL);  // inexistent key causes an assertion error
    if (key < tree->key) {
        tree->left = _avl_delete(tree->left, key);
        tree = _avl_maintain(tree);
    } else if (tree->key < key) {
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
        free(current);
    }
    return tree;
}

/** inexistent key will cause an error */
void avl_delete(struct avl_tree_t * tree, int key) {
    assert(tree != NULL);
    tree->root = _avl_delete(tree->root, key);
    tree->size--;
}

void avl_print(struct avl_node_t * tree, int height, int branch) {
    if (tree == NULL) {
        return;
    }
    for (int i = 0; i < height; ++i) putchar(' ');
    printf("%skey=%d, value=%d, height=%d\n", branch == 0 ? "" : (branch == 1 ? "L: " : "R: "), tree->key, tree->val, tree->height);
    avl_print(tree->left, height + 1, 1);
    avl_print(tree->right, height + 1, 2);
}

int main() {
    srand(1);
    struct avl_tree_t tree = create_avl_tree();

    avl_insert(&tree, 3, 3);
    avl_insert(&tree, 9, 9);
    avl_insert(&tree, 4, 4);
    avl_insert(&tree, 0, 0);
    avl_insert(&tree, 5, 5);
    avl_insert(&tree, 2, 2);
    avl_insert(&tree, 7, 7);
    avl_insert(&tree, 8, 8);
    avl_insert(&tree, 1, 1);
    avl_insert(&tree, 6, 6);
    avl_print(tree.root, 0, 0); putchar('\n');

    // check insert duplicate
    // if (avl_insert(&tree, 7, 8) == NULL)
    //     puts("Failed to insert as expected\n");

    // printf("size=%d\n", tree.size);
    // avl_delete(&tree, 11);
    // printf("size=%d\n", tree.size);
    // avl_print(tree.root, 0); putchar('\n');
    // clear_avl_tree(&tree);

    avl_delete(&tree, 7);
    puts("deleted 7:");
    avl_print(tree.root, 0, 0); putchar('\n');

    avl_delete(&tree, 8);
    puts("deleted 8:");
    avl_print(tree.root, 0, 0); putchar('\n');

    // avl_delete(&tree, 3);
    // puts("deleted 3:");
    // avl_print(tree.root, 0, 0); putchar('\n');

    // avl_delete(&tree, 5);
    // puts("deleted 5:");
    // avl_print(tree.root, 0, 0); putchar('\n');

    // avl_print(tree.root, 0); putchar('\n');
    clear_avl_tree(&tree);

    // avl_insert(&tree, 3, 3);
    // avl_insert(&tree, 9, 9);
    // avl_insert(&tree, 4, 4);
    // avl_insert(&tree, 0, 0);

    // avl_print(tree.root, 0); putchar('\n');
    // clear_avl_tree(&tree);

    return 0;
}
