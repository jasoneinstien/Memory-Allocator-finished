#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

static ht_item* ht_new_item(const char*k , const char*v){
    ht_item* i = malloc(sizeof(ht_item));
    i->key = strdup(k);
    i->value = strdup(v); 
    return i; 
}

ht_hash_table* ht_new(){
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));

    ht -> size = 53; 
    ht -> count = 0;
    ht -> item = calloc((size_t) ht -> size , sizeof(ht_item*));
    
    return ht; 
}

static void ht_del_item(ht_item* i){
    free(i->key);
    free(i->value);
    free(i); 
}

void ht_del_hash_table(ht_tash_table* ht){
    for(int i =0 ; i < ht->size ; i++){
        ht_item* item = ht->items[i];
        if(item != NULL){
            ht_del_item(item);
        }
    }
    free (ht -> items);
    free (ht);
}

//hash algorithm 
//a refer to a prime number larger than ASCII scale --> 151 m , reduce the size of interger by taking it's reminder

static int ht_hash(const char*s , const int a , const int m){
    long hash = 0;
    const int len_s = strlen(s);
    for(int i= 0; i < len_s ; i++){
        hash += (long)pow(a , len_s - (i +1 ) *s[i]);
        hash = hash%m;
    }
    return (int)hash; 
}

static int ht_get_hash(const char *s , const int num_buckets , const int attempt){
    const int hash_a = ht_hash(s, HT_PRIME_1 , number_buckets);
    const int hash_b = ht_hash(s ,HT_PRIME_2 , num_buckets);
    return (hash_a + (attempt *(hash_b +1))) % number_buckets;
}

int main(){
    ht_hash_table *ht = ht_new();
    ht_del_hash_table(ht);
    return 0;
}
