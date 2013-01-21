#include "ncache.h"
#include <awd/libawd.h>

void 
awd_ncache_init(awd_ncache *cache)
{
    cache->first_item = NULL;
    cache->last_item = NULL;
}


void
awd_ncache_free(awd_ncache *cache)
{
    awd_ncache_item *cur;
	awd_ncache_item *next;

    next = cache->first_item;
    while (next != NULL) {
		cur = next;
		next = next->next;

		// block normally already deleted by awdlib
		// delete cur->block;
		
		cur->obj = NULL;
		cur->block = NULL;
		cur->next = NULL;
		delete cur;
    }

}


void
awd_ncache_add(awd_ncache *cache, InterfaceServer *obj, AWDBlock *block)
{
    awd_ncache_item *item;

    item = (awd_ncache_item *)malloc(sizeof(awd_ncache_item));
    item->obj = obj;
    item->block = block;
    item->next = NULL;

    if (cache->first_item == NULL) {
        cache->first_item = item;
    }
    else {
        cache->last_item->next = item;
    }

    cache->last_item = item;
}


AWDBlock *
awd_ncache_get(awd_ncache *cache, InterfaceServer *obj)
{
    awd_ncache_item *cur;

    cur = cache->first_item;
    while (cur != NULL) {
        if (cur->obj == obj)
            return cur->block;

        cur = cur->next;
    }

    return NULL;
}



