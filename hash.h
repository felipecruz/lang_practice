#include <stdint.h>
#include <string.h>

#define match(key1, key2, size) (0 == memcmp(key1, key2, size))

typedef struct hash_table hash;
typedef struct hash_element helem;

struct hash_element {
    uint8_t *key;
    void *value;
    size_t value_len;
    helem *next;
};

struct hash_table {
    helem **elements;
    size_t elements_length;
    size_t hash_len;
    size_t capacity;
};

hash *hash_init (size_t);

void hash_destroy (hash **hash);

void hash_put (hash *table, uint8_t *key, void *data, size_t length);

helem *hash_get (hash *table, uint8_t *key);

void hash_del (hash *table, uint8_t *key);
