#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct inode {
		struct stat STAT;
		char*	name;
		void*	data;

		struct	inode	*parent;
		struct	inode	*child;
		struct	inode	*next;
		struct	inode	*before;
} inode;

static inode *root_node;

static void *hello_init(struct fuse_conn_info *conn)
{
	puts("Hello_init!!");
}
static int hello_getattr(const char *path, struct stat *stbuf)
{
	puts("Hello_Getattr");
	inode *node;
	node = root_node;

	if ( node == NULL) {
		puts("file not exist");
		return -ENOENT;
	}
	else {
		puts("file exist");
		memset(stbuf, 0, sizeof(struct stat));
		stbuf->st_mode = node->STAT.st_mode; /* protection */
		stbuf->st_nlink = node->STAT.st_nlink; /* number of hard links */
		stbuf->st_size = node->STAT.st_size; /* totla size, in bytes */
		stbuf->st_atime = node->STAT.st_atime; /* time of last access */
		stbuf->st_ctime = node->STAT.st_ctime; /* time of last status change */
		stbuf->st_mtime = node->STAT.st_mtime; /* time of last modificaiton */
	}
	return 0;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	puts("Hello_readdir");
  return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	puts("hello_open");
	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	puts("hello_read");
  return size;
}

static int hello_write(const char *path, const char *buf, size_t size,
			off_t offset, struct fuse_file_info *fi)
{
	 puts("hello_write");
	 return size;
}

static int hello_mkdir(const char *path, mode_t mode)
{
 	puts("hello_mkdir");
}

static int hello_rmdir(const char *path)
{
 	puts("hello_rmdir");
}

static int hello_mknod(const char *path, mode_t mode)
{
 	puts("hello_mknod");
}

static int hello_unlink(const char *path)
{
 	puts("hello_unlink");
}
static struct fuse_operations hello_oper = {
		.init		= hello_init,
		.getattr	= hello_getattr,
		.readdir	= hello_readdir,
		.open		= hello_open,
		.read		= hello_read,
		.write		= hello_write,
		.mkdir		= hello_mkdir,
		.rmdir		= hello_rmdir,
		.mknod		= hello_mknod,
		.unlink		= hello_unlink
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &hello_oper, NULL);
}
