#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct _inode {
		char name[14];
		unsigned char data[128];
		struct stat st;

		struct	inode	*parent;
		struct	inode	*child;
		struct	inode	*next;
		struct	inode	*before;
} inode;

static inode* root_node;

static void *hello_init(struct fuse_conn_info *conn)
{
	printf("Hello_init");

	(void) conn;

	time_t current_time = time(NULL);

	root_node = (inode *) calloc(1, sizeof(inode));
	root_node->parent = NULL;
	root_node->child = NULL;
	root_node->next = NULL;
	root_node->before = NULL;

	strcpy(root_node->name, "/");
	root_node->st.st_mode = (S_IFDIR | 0755);
	root_node->st.st_nlink = 2;
	root_node->st.st_mtime = current_time;
	root_node->st.st_ctime = current_time;
	root_node->st.st_atime = current_time;

	printf("Hello_init end");

	return NULL;
}
static int hello_getattr(const char *path, struct stat *stbuf)
{
	printf("Hello_Getattr");
	inode *node;
	node = root_node;

	if ( node == NULL) {
		printf("file not exist");
		return -ENOENT;
	}
	else {
		printf("file exist");
		memset(stbuf, 0, sizeof(struct stat));
		stbuf->st_mode = node->st.st_mode; /* protection */
		stbuf->st_nlink = node->st.st_nlink; /* number of hard links */
		stbuf->st_size = node->st.st_size; /* totla size, in bytes */
		stbuf->st_atime = node->st.st_atime; /* time of last access */
		stbuf->st_ctime = node->st.st_ctime; /* time of last status change */
		stbuf->st_mtime = node->st.st_mtime; /* time of last modificaiton */
	}
	printf("Hello_Getattr end");
	return 0;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	printf("Hello_readdir");
	(void) offset;
	(void) fi;
	inode *node;
	node = root_node;

		if ( node == NULL ) {
				return -ENOENT;
		}

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf,node->name, NULL, 0);

	printf("Hello_readdir end");
  return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	printf("hello_open");
	printf("hello_open end");
	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	printf("hello_read");
	printf("hello_read end");
  return size;
}

static int hello_write(const char *path, const char *buf, size_t size,
			off_t offset, struct fuse_file_info *fi)
{
	 printf("hello_write");
	 printf("hello_write end");
	 return size;
}

static int hello_mkdir(const char *path, mode_t mode)
{
 	printf("hello_mkdir");
	printf("hello_mkdir end");
}

static int hello_rmdir(const char *path)
{
 	printf("hello_rmdir");
	printf("hello_rmdir end");
}

static int hello_mknod(const char *path, mode_t mode)
{
 	printf("hello_mknod");
	printf("hello_mknod end");
}

static int hello_unlink(const char *path)
{
 	printf("hello_unlink");
	printf("hello_unlink end");
}
static int hello_truncate(const char* path, off_t s)
{
		printf("hello_truncate");
		printf("hello_truncate end");
}
static int hello_utimens(const char *path, const struct timespec ts[2])
{
	printf("hello_utimens");
	printf("hello_utimens end");
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
		.unlink		= hello_unlink,
		.truncate = hello_truncate,
		.utimens  = hello_utimens
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &hello_oper, NULL);
}
