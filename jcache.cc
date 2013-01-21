#include "jcache.h"
#include <awd/libawd.h>

void 
awd_jcache_init(awd_jcache *cache)
{
    cache->first_item = NULL;
    cache->last_item = NULL;
}


void
awd_jcache_free(awd_jcache *cache)
{
    awd_jcache_item *cur;
	awd_jcache_item *next;

    next = cache->first_item;
    while (next != NULL) {
		cur = next;
		next = next->next;

		// block normally already deleted by awdlib
		// delete cur->block;
		
		cur->obj = NULL;
		cur->joint = NULL;
		cur->next = NULL;
		delete cur;
    }

}


void
awd_jcache_add(awd_jcache *cache, INode *obj, AWDMaxSkeletonJoint *joint)
{
    awd_jcache_item *item;

    item = (awd_jcache_item *)malloc(sizeof(awd_jcache_item));
    item->obj = obj;
    item->joint = joint;
    item->next = NULL;

    if (cache->first_item == NULL) {
        cache->first_item = item;
    }
    else {
        cache->last_item->next = item;
    }

    cache->last_item = item;
}


AWDMaxSkeletonJoint *
awd_jcache_get(awd_jcache *cache, INode *obj)
{
    awd_jcache_item *cur;

    cur = cache->first_item;
    while (cur != NULL) {
        if (cur->obj == obj)
            return cur->joint;

        cur = cur->next;
    }

    return NULL;
}



