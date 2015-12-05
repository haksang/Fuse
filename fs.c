#define FUSE_USE_VERSION 30

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

static inode *root_node;
static inode *sub_node;

static inode *find_node(inode* parent, const char* name){
	inode *node;
	for(node = parent->child; (node != NULL) && strcmp(node->name, name); node = node->next);
	return node;
}

static inode *find_path(const char* path)
{
	char *tmp;
	char *temp_path;
	inode *node;
	node = root_node;

	if(strcmp(path, "/") == 0)
		return node;

	temp_path = (char *)calloc(strlen(path),sizeof(char));
	strcpy(temp_path, path);
	tmp = strtok(temp_path, "/");

	if((node = find_node(node, tmp)) == NULL) {
		free(temp_path);
		return NULL;
	}
	for( ; (tmp = strtok(NULL, "/")) && node != NULL;
		 node = find_node(node, tmp));

	free(temp_path);
	return node;
}

static char *find_parent(const char* path) {
	int 	i = strlen(path);
	char *parent = (char*)calloc(i, sizeof(char));

	strcpy(parent, path);
	for(  ; parent[i] != '/' && i > 0 ; --i )
		parent[i] = '\0';

	return parent;
}

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
	inode *node;
	node = root_node;
	if (node == NULL)
		return -ENOENT;
	printf("hello_open end");
	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	(void) fi;
	printf("hello_read");
	inode *node;
	node = root_node;
	size_t length;

	if(node == NULL)
		return -ENOENT;

	length = node->st.st_size;
	if (offset < length) {
		if (offset + size > length)
			size = length - offset;
		memcpy(buf, node->data + offset, size);
	}
	else
		size = 0;
	printf("hello_read end");
  return size;
}

static int hello_write(const char *path, const char *buf, size_t size,
			off_t offset, struct fuse_file_info *fi)
{
	 printf("hello_write");
	 inode *node;
	 void *tmp;
	 (void) fi;
	 if(node == NULL)
	 	return -ENOENT;
	printf("hello_write end");
	return size;
}

static int hello_mkdir(const char *path, mode_t mode)
{
	printf("hello_mkdir");

	time_t	current_time = time(NULL);
	inode *node;
	node = root_node;
	node->st.st_nlink = 2;
	node->st.st_mode = mode | S_IFDIR;
	node->st.st_mtime = current_time;
	node->st.st_ctime = current_time;
	node->st.st_atime = current_time;

	printf("hello_mkdir end");
	return 0;
}

static int hello_rmdir(const char *path)
{
 	printf("hello_rmdir");
	inode *node;
	node = root_node;
	if(node==NULL)
		return -ENOENT;
	if (((node->st.st_mode & S_IFDIR) && ( node->child )))
		return -EISDIR;

	free(node);
	printf("hello_rmdir end");
	return 0;
}

static int hello_mknod(const char *path, mode_t mode)
{
 	printf("hello_mknod");
	time_t	current_time = time(NULL);
	inode* 	node;
	node = root_node;
	node->st.st_nlink = 1;
	node->st.st_mode = mode | S_IFREG;
	node->st.st_mtime = current_time;
	node->st.st_ctime = current_time;
	node->st.st_atime = current_time;

	printf("hello_mknod end");
	return 0;
}

static int hello_unlink(const char *path)
{
 	printf("hello_unlink");
	inode *node;
	node = root_node;
	if(node==NULL)
		return -ENOENT;
	free(node);
	printf("hello_unlink end");
	return 0;
}
static int hello_truncate(const char* path, off_t s)
{
		printf("hello_truncate");
		printf("hello_truncate end");
		return 0;
}
static int hello_utimens(const char *path, const struct timespec ts[2])
{
	printf("hello_utimens");
	inode *node;
	node = root_node;
	time_t current_time = time(NULL);
	if(node == NULL)
		return -ENOENT;
	node->st.st_mtime = current_time;
	node->st.st_atime = current_time;
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
