#ifndef AWD_jcache_H
#define AWD_jcache_H


#include <awd/libawd.h>
#include "3dsmaxsdk_preinclude.h"
#include "Max.h"
#include "maxskeleton.h"

typedef struct _jcache_elem {
    INode *obj;
	AWDMaxSkeletonJoint*joint;
    struct _jcache_elem *next;
} awd_jcache_item;

typedef struct _jcache {
    awd_jcache_item *first_item;
    awd_jcache_item *last_item;
} awd_jcache;


void awd_jcache_init(awd_jcache *jcache);
void awd_jcache_free(awd_jcache *jcache);
void awd_jcache_add(awd_jcache *jcache, INode *obj, AWDMaxSkeletonJoint *joint);
AWDMaxSkeletonJoint *awd_jcache_get(awd_jcache *jcache, INode *obj);


#endif
