#ifndef AWD_ncache_H
#define AWD_ncache_H


#include <awd/libawd.h>
#include "3dsmaxsdk_preinclude.h"
#include "Max.h"

typedef struct _cache_elem {
    InterfaceServer *obj;
    AWDBlock *block;
    struct _cache_elem *next;
} awd_ncache_item;

typedef struct _ncache {
    awd_ncache_item *first_item;
    awd_ncache_item *last_item;
} awd_ncache;


void awd_ncache_init(awd_ncache *ncache);
void awd_ncache_free(awd_ncache *ncache);
void awd_ncache_add(awd_ncache *ncache, InterfaceServer *obj, AWDBlock *block);
AWDBlock *awd_ncache_get(awd_ncache *ncache, InterfaceServer *obj);


#endif
