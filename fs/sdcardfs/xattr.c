/*
 * fs/sdcardfs/xattr.c
 *
 * Copyright (c) 2015 Lenovo Co. Ltd
 *   Authors: liaohs , jixj

 *
 * This program has been developed as a stackable file system based on
 * the WrapFS which written by 
 *
 * Copyright (c) 1998-2014 Erez Zadok
 * Copyright (c) 2009	   Shrikar Archak
 * Copyright (c) 2003-2014 Stony Brook University
 * Copyright (c) 2003-2014 The Research Foundation of SUNY
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/xattr.h>
#include <linux/dcache.h>
#include "sdcardfs.h"

static struct dentry *
sdcardfs_dentry_to_lower(struct dentry *dentry)
{
    struct dentry* ret;

    ret = ((struct sdcardfs_dentry_info *)dentry->d_fsdata)->lower_path.dentry;
    return ret;
}

int
sdcardfs_setxattr(struct dentry *dentry, const char *name, const void *value, size_t size, int flags)
{
	int rc = 0;
	struct dentry *lower_dentry;

	lower_dentry = sdcardfs_dentry_to_lower(dentry);
	if (!lower_dentry->d_inode->i_op->setxattr) {
		rc = -EOPNOTSUPP;
		goto out;
	}

	rc = vfs_setxattr(lower_dentry, name, value, size, flags);
out:
	return rc;
}

static ssize_t
sdcardfs_getxattr_lower(struct dentry *lower_dentry, const char *name, void *value, size_t size)
{
	int rc = 0;

	if (!lower_dentry->d_inode->i_op->getxattr) {
		rc = -EOPNOTSUPP;
		goto out;
	}
	mutex_lock(&lower_dentry->d_inode->i_mutex);
	rc = lower_dentry->d_inode->i_op->getxattr(lower_dentry, name, value,
						   size);
	mutex_unlock(&lower_dentry->d_inode->i_mutex);
out:
	return rc;
}

ssize_t
sdcardfs_getxattr(struct dentry *dentry, const char *name, void *value, size_t size)
{
    ssize_t ret;
	ret = sdcardfs_getxattr_lower(sdcardfs_dentry_to_lower(dentry), name,
				       value, size);
    return ret;
}

ssize_t
sdcardfs_listxattr(struct dentry *dentry, char *list, size_t size)
{
	int rc = 0;
	struct dentry *lower_dentry;

	lower_dentry = sdcardfs_dentry_to_lower(dentry);
	if (!lower_dentry->d_inode->i_op->listxattr) {
		rc = -EOPNOTSUPP;
		goto out;
	}
	mutex_lock(&lower_dentry->d_inode->i_mutex);
	rc = lower_dentry->d_inode->i_op->listxattr(lower_dentry, list, size);
	mutex_unlock(&lower_dentry->d_inode->i_mutex);
out:
	return rc;
}

int
sdcardfs_removexattr(struct dentry *dentry, const char *name)
{
	int rc = 0;
	struct dentry *lower_dentry;

	lower_dentry = sdcardfs_dentry_to_lower(dentry);
	if (!lower_dentry->d_inode->i_op->removexattr) {
		rc = -EOPNOTSUPP;
		goto out;
	}
	mutex_lock(&lower_dentry->d_inode->i_mutex);
	rc = lower_dentry->d_inode->i_op->removexattr(lower_dentry, name);
	mutex_unlock(&lower_dentry->d_inode->i_mutex);
out:
	return rc;
}
