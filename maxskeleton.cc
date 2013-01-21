#include <cstdio>
#include <cstring>

#include <awd/util.h>
#include "maxskeleton.h"

#include <awd/platform.h>


AWDMaxSkeletonJoint::AWDMaxSkeletonJoint(const char *name, awd_uint16 name_len, awd_float64 *bind_mtx, FILE *			logfile) :
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->next = NULL;
    this->bind_mtx = bind_mtx;
    this->num_children = 0;
	this->parent = NULL;

	this->logfile = logfile;

    if (this->bind_mtx == NULL) {
        this->bind_mtx = awdutil_id_mtx4x4(NULL);
    }
}


AWDMaxSkeletonJoint::~AWDMaxSkeletonJoint()
{
    AWDMaxSkeletonJoint *cur;


    if (this->bind_mtx) {
        free(this->bind_mtx);
        this->bind_mtx = NULL;
    }

    this->num_children = 0;
}


awd_uint32
AWDMaxSkeletonJoint::get_id()
{
    return this->id;
}


void
AWDMaxSkeletonJoint::set_parent(AWDMaxSkeletonJoint *joint)
{
    this->parent = joint;
}


AWDMaxSkeletonJoint *
AWDMaxSkeletonJoint::get_parent()
{
    return this->parent;
}



AWDMaxSkeletonJoint *
AWDMaxSkeletonJoint::add_child_joint(AWDMaxSkeletonJoint *joint)
{
    if (joint != NULL) {
        if (joint->get_parent() != NULL) {
            // TODO: Remove from old parent
        }

        joint->set_parent(this);
        this->num_children++;
    }

    return joint;
}



int
AWDMaxSkeletonJoint::calc_length(bool wide_geom, bool wide_mtx)
{
    int len;
    AWDMaxSkeletonJoint *child;
    
    // id + parent + name varstr + matrix
    len = sizeof(awd_uint32) + sizeof(awd_uint32) + 
        sizeof(awd_uint16) + this->get_name_length() + 
        MTX4_SIZE(wide_mtx);

    len += this->calc_attr_length(true,true, wide_geom, wide_mtx);

    return len;
}



int
AWDMaxSkeletonJoint::write_joint(int fd, bool wide_geom, bool wide_mtx)
{

	fprintf( logfile, "		AWDMaxSkeletonJoint::write_body \n" );
	fflush( logfile );

    int num_written;
    awd_uint32 child_id;
    AWDMaxSkeletonJoint *child;
    awd_uint32 par_id_be;
    awd_uint32 id_be;


    // Convert numbers to big-endian
    id_be = UI32(this->id);

	fprintf( logfile, "		AWDMaxSkeletonJoint::write_body id %i \n", id_be );
	fflush( logfile );

    if (this->parent) 
        par_id_be = UI32(this->parent->id);
    else 
		par_id_be = 0;

	
	fprintf( logfile, "		AWDMaxSkeletonJoint::write_body pid %i \n", par_id_be );
	fflush( logfile );
	

    // Write this joint
    write(fd, &id_be, sizeof(awd_uint32));
    write(fd, &par_id_be, sizeof(awd_uint32));
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    awdutil_write_mtx4x4(fd, this->bind_mtx, wide_mtx);

    //  TODO: Write attributes
    this->properties->write_attributes(fd, wide_geom, wide_mtx);
    this->user_attributes->write_attributes(fd, wide_geom, wide_mtx);

   
    return num_written;
}






AWDMaxSkeleton::AWDMaxSkeleton(const char *name, awd_uint16 name_len, FILE * logfile) :
    AWDSkeleton(name, name_len)
{
    this->mroot_joint = NULL;
	this->mlast_joint = NULL;
	this->logfile = logfile;
}


AWDMaxSkeleton::~AWDMaxSkeleton()
{
	AWDMaxSkeletonJoint * cur;
	cur = this->mroot_joint;
    while (cur) {
        AWDMaxSkeletonJoint *next = cur->next;
        cur->next = NULL;
        delete cur; // Will remove it's children too (recursively)
        cur = next;
    }

    if (this->mroot_joint) {
        delete this->mroot_joint;
        this->mroot_joint = NULL;
    }
	if (this->mlast_joint) {
        delete this->mlast_joint;
        this->mlast_joint = NULL;
    }
}


awd_uint32
AWDMaxSkeleton::calc_body_length(bool wide_geom, bool wide_mtx)
{
    awd_uint32 len;

    len = sizeof(awd_uint16) + this->get_name_length() + sizeof(awd_uint16);
    len += this->calc_attr_length(true,true, wide_geom, wide_mtx);


	AWDMaxSkeletonJoint* joint = this->mroot_joint;

	while( joint ) {
		len += joint->calc_length(wide_geom, wide_mtx);
		joint = joint->next;
	}
   
    return len;
}


void
AWDMaxSkeleton::write_body(int fd, bool wide_geom, bool wide_mtx)
{

	fprintf( logfile, "	AWDMaxSkeleton::write_body \n" );
	fflush( logfile );

    awd_uint16 num_joints_be;

	num_joints_be = UI16(this->mlast_joint->id);

    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    write(fd, &num_joints_be, sizeof(awd_uint16));

    // Write optional properties
    this->properties->write_attributes(fd, wide_geom, wide_mtx);
	
	AWDMaxSkeletonJoint* joint = this->mroot_joint;

	while( joint ) {

		fprintf( logfile, "		AWDMaxSkeleton::write_body enter join %i \n", joint->id );
		fflush( logfile );

		joint->write_joint(fd, wide_geom, wide_mtx);
		joint = joint->next;
	}

    // Write user attributes
    this->user_attributes->write_attributes(fd, wide_geom, wide_mtx);
}



AWDMaxSkeletonJoint *
AWDMaxSkeleton::get_head_joint()
{
    return this->mroot_joint;
}


AWDMaxSkeletonJoint *
AWDMaxSkeleton::add_joint(AWDMaxSkeletonJoint *joint)
{
	if( this->mroot_joint == NULL ) {
		joint->id = 1;
		this->mroot_joint = joint;
	} else {
		this->mlast_joint->next = joint;
		joint->id = this->mlast_joint->id + 1;
	}
	this->mlast_joint = joint;

    return joint;
}

