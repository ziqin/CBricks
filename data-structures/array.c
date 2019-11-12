#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// typedef int T;

#define USE_ARRAY(T)                                                            \
                                                                                \
struct T##_array_t {                                                            \
    T * addr;                                                                   \
    unsigned length;                                                            \
    unsigned capacity;                                                          \
};                                                                              \
                                                                                \
static struct T##_array_t create_array_##T(int length, int capacity) {          \
    assert(capacity > 0);                                                       \
    struct T##_array_t arr;                                                     \
    arr.addr = calloc(capacity, sizeof(T));                                     \
    if (arr.addr == NULL) {                                                     \
        arr.length = arr.capacity = 0;                                          \
    } else {                                                                    \
        arr.length = length;                                                    \
        arr.capacity = capacity;                                                \
    }                                                                           \
    return arr;                                                                 \
}                                                                               \
                                                                                \
static void delete_array_##T(struct T##_array_t * arr) {                        \
    assert(arr != NULL);                                                        \
    if (arr->addr != NULL) {                                                    \
        free(arr->addr);                                                        \
    }                                                                           \
    arr->length = arr->capacity = 0;                                            \
}                                                                               \
                                                                                \
/* amortized time complexity: O(1) */                                           \
static void append_##T(struct T##_array_t * arr, T val) {                       \
    assert(arr != NULL);                                                        \
    if (arr->length >= arr->capacity) {                                         \
        T * new_addr = realloc(arr->addr, sizeof(T) * (arr->capacity * 2));     \
        assert(new_addr != NULL);                                               \
        arr->addr = new_addr;                                                   \
        arr->capacity = arr->capacity * 2;                                      \
    }                                                                           \
    arr->addr[arr->length++] = val;                                             \
}                                                                               \
                                                                                \
static T pop_##T(struct T##_array_t * arr) {                                    \
    assert(arr != NULL && arr->length > 0);                                     \
    T ans = arr->addr[--(arr->length)];                                         \
    if (arr->length < arr->capacity / 2) {                                      \
        arr->capacity /= 2;                                                     \
        arr->addr = realloc(arr->addr, sizeof(T) * arr->capacity);              \
        assert(arr->addr != NULL);                                              \
    }                                                                           \
    return ans;                                                                 \
}                                                                               \
                                                                                \
typedef struct T##_array_t T##_array_t

#define array_t(T) T##_array_t
#define create_array(T, length, capacity) create_array_##T((length), (capacity))
#define delete_array(T, arr) delete_array_##T((arr))
#define append(T, arr, val) append_##T((arr), (val))
#define pop(T, arr) pop_##T((arr))
#define at(arr, index) ((arr).addr[index])

USE_ARRAY(int);

int main() {
    array_t(int) arr = create_array(int, 3, 3);
    for (int i = 0; i < arr.length; ++i) {
        at(arr, i) = i * 10;
        printf("set arr[%d] = %d\n", i, at(arr, i));
    }
    append(int, &arr, 30);
    append(int, &arr, 40);
    append(int, &arr, 50);
    printf("arr.length = %u, arr.capacity = %u\n", arr.length, arr.capacity);
    while (arr.length > 0) {
        printf("pop %d\n", pop(int, &arr));
    }
    printf("arr.length = %u, arr.capacity = %u\n", arr.length, arr.capacity);
    for (int i = 0; i < 10; ++i) {
        append(int, &arr, -i);
        for (int i = 0; i < arr.capacity; ++i) {
            printf(" %d", at(arr, i));
        }
        printf(" | length=%d\n", arr.length);
    }
    for (int i = 0; i < arr.length; ++i) {
        printf("arr[%d] = %d\n", i, at(arr, i));
    }
    while (arr.length > 5) {
        printf("pop %d\n", pop(int, &arr));
    }
    printf("arr.length = %u, arr.capacity = %u\n", arr.length, arr.capacity);
    delete_array(int, &arr);
    return 0;
}
