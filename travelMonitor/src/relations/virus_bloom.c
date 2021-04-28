#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "virus_bloom.h"

struct virus_bloom_struct
{
    char *virusName;
    BloomFilter filter;
};

virus_bloom create_virus_bloom(char *virusName, size_t bytes){
    virus_bloom v = (virus_bloom)malloc(sizeof(struct virus_bloom_struct));

    v->virusName = (char *)malloc(sizeof(char)*(strlen(virusName)+1));
    strcpy(v->virusName, virusName);

    v->filter = BloomCreate(bytes);

    return v;
}

void *get_virus_bloomName(void *v){
    virus_bloom nv = v;
    return nv->virusName; 
}

BloomFilter get_bloom(void *v){
    virus_bloom nv = v;
    return nv->filter;    
}

void update_BloomFilter(void *v, char *bloom_filter){
    virus_bloom nv = v;
    update_array(nv->filter, bloom_filter);
}

int compare_virus_bloomName(void *key, void *v){
    char *k = key;
    virus_bloom nv = v;

    return strcmp(k, nv->virusName);    
}

void destroy_virus_bloom(void *v){
    virus_bloom nv = v;
    free(nv->virusName);
    BloomDestroy(nv->filter);
    free(nv);
}

