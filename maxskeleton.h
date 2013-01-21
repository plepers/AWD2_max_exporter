#ifndef _LIBAWD_MAXSKELETON_H
#define _LIBAWD_MAXSKELETON_H

#include <awd/block.h>
#include <awd/attr.h>




class AWDMaxSkeletonJoint : 
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        
        awd_float64 *bind_mtx;
        int num_children;

        AWDMaxSkeletonJoint *parent;

		FILE *			logfile;
        
    public:
        AWDMaxSkeletonJoint *next;

        //AWDMaxSkeletonJoint(const char *, awd_uint16, awd_float64 *);
		AWDMaxSkeletonJoint(const char *, awd_uint16, awd_float64 *, FILE *);
        ~AWDMaxSkeletonJoint();

        int write_joint(int, bool, bool);
        int calc_length(bool, bool);

		awd_uint32 id;
        awd_uint32 get_id();
        void set_parent(AWDMaxSkeletonJoint *);
        AWDMaxSkeletonJoint *get_parent();
        AWDMaxSkeletonJoint *add_child_joint(AWDMaxSkeletonJoint *);
};


class AWDMaxSkeleton : 
	public AWDSkeleton
{
    private:
        AWDMaxSkeletonJoint *mroot_joint;
		AWDMaxSkeletonJoint *mlast_joint;

		FILE *			logfile;

    protected:
        awd_uint32 calc_body_length(bool,bool);
        void write_body(int, bool,bool);

    public:
     //   AWDMaxSkeleton(const char *, awd_uint16);
		AWDMaxSkeleton(const char *, awd_uint16, FILE *);
        ~AWDMaxSkeleton();

        AWDMaxSkeletonJoint *add_joint(AWDMaxSkeletonJoint *);
        AWDMaxSkeletonJoint *get_head_joint();
};

#endif
