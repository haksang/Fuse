#define FUSE_USE_VERSION 26
#define MAX_NAME_LEN 255

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include "log.h"
#include "params.h"

typedef struct _inode {
		char 		*name;
		void		*data;
		struct stat st;

		struct	_inode	*parent;
		struct	_inode	*child;
		struct	_inode	*next;
		struct	_inode	*before;
} inode;

static inode* root_node;
/* find node in path */
static inode* find_node(inode* parent, const char* name){
	inode*	node = parent->child;
	while(node != NULL && strcmp(node->name, name)) {
		node = node->next;
	}
	return node;
}

static inode* find_path(const char* path)
{
	char* tmp;
	char* temp_path;
	inode* node = root_node;

	if(strcmp(path, "/") == 0)
		return node;

	temp_path = (char*)calloc(strlen(path),sizeof(char));
	strcpy(temp_path, path);
	tmp = strtok(temp_path, "/");

	if((node = find_node(node, tmp)) == NULL) {
		free(temp_path);
		return NULL;
	}

	while((tmp = strtok(NULL, "/")) && node != NULL) {
		node = find_node(node, tmp);
	}

	free(temp_path);
	return node;
}

static char* find_parent(const char* path) {
	int 	i = strlen(path);
	char*	parent = (char*)calloc(i, sizeof(char));

	strcpy(parent, path);

	while(parent[i] != '/' && i > 0) {
		parent[i]= '\0';
		--i;
	}

	return parent;
}

/* fuse operation */
static void *hello_init(struct fuse_conn_info *conn)
{
	(void) conn;
	time_t current_time = time(NULL);

	printf("Hello_init");

	root_node = (inode *) calloc(1, sizeof(inode));
	root_node->parent = NULL;
	root_node->child = NULL;
	root_node->next = NULL;
	root_node->before = NULL;

	root_node->st.st_mode = (S_IFDIR | 0755);
	root_node->st.st_nlink = 2;
	root_node->st.st_mtime = current_time;
	root_node->st.st_ctime = current_time;
	root_node->st.st_atime = current_time;
	root_node->name = "";
	printf("Hello_init end");

	return NULL;
}
static int hello_getattr(const char *path, struct stat *stbuf)
{
	inode *node;

	printf("Hello_Getattr");

	node = find_path(path);
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
	(void) offset;
	(void) fi;
	inode *node;

	printf("Hello_readdir");

	node = find_path(path)->child;

	if ( node == NULL ) {
				return -ENOENT;
	}

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	while(node != NULL) {
		filler(buf, node->name, NULL, 0);
		node = node->next;
	}

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
	size_t len;
	(void) fi;
	inode* node;

	printf("hello_read");

	node = find_path(path);
	if(node == NULL)
		return -ENOENT;
	len = node->st.st_size;
	if (offset < len)
	{
		if (offset + size > len)
			size = len - offset;
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
	 inode* node;
	 void* tmp;
	 (void) fi;

	 printf("hello_write");

	 node = find_path(path);

	 if(node == NULL)
	 	return -ENOENT;

	 if (size + offset > node->st.st_size)
	 {
	 	tmp = malloc(node->st.st_size + size);
	 	if (tmp) {
	 		if (node->data) {
	 			memcpy(tmp, node->data, node->st.st_size);
	 			free(node->data);
	 		}
	 		node->data = tmp;
	 		node->st.st_size += size;
	 	}
	}
	memcpy(node->data + offset, buf, size);
	printf("hello_write end");
	return size;
}
static int hello_mkdir(const char *path, mode_t mode)
{
	time_t	current_time = time(NULL);
	inode*	parent;
	inode* 	node;
	inode*	last_node;

	char*	parent_path = find_parent(path);
	int len = strlen(parent_path);
	int i;

	printf("hello_mkdir");

	parent = find_path(parent_path);

	if (parent == NULL)
		return -ENOENT;
	if ( find_path(path) != NULL )
		return -EEXIST;
	node = (inode*)calloc(1, sizeof(inode));
	node->parent = parent;
	node->st.st_nlink = 2;
	node->st.st_mode = mode | S_IFDIR;
	node->st.st_mtime = current_time;
	node->st.st_ctime = current_time;
	node->st.st_atime = current_time;
	node->child = NULL;
	node->next = NULL;

	node->name = (char*)calloc(MAX_NAME_LEN, sizeof(char));


	for( i = len ; path[i] != '\0' ; i++ )
		node->name[i-len] = path[i];
	node->name[i-len] = '\0';

	if(parent->child == NULL){
		parent->child = node;
		node->before = NULL;
	}
	else {
		for(last_node = parent->child; last_node->next != NULL;
			last_node = last_node->next);
		last_node->next = node;
		node->before = last_node;
	}

	printf("hello_mkdir end");
	return 0;
}

static int hello_rmdir(const char *path)
{
	inode* node;
	node = find_path(path);

 	printf("hello_rmdir");

	if (node == root_node ||
		((node->st.st_mode & S_IFDIR) && ( node->child )) )
		return -EISDIR;
	if(node == node->parent->child){
		if(node->next)
			node->next->before = NULL;
		node->parent->child = node->next;
		if(node->data)
			free(node->data);
		free(node->name);
		free(node);
	}
	else {
		node->before->next = node->next;
		if(node->next)
			node->next->before = node->before;
		if(node->data)
			free(node->data);
		free(node->name);
		free(node);
	}
	printf("hello_rmdir end");
	return 0;
}

static int hello_mknod(const char *path, mode_t mode)
{
 		time_t	current_time = time(NULL);
		inode*	parent;
		inode* 	node;
		inode*	last_node;

		char*	parent_path = find_parent(path);
		int len = strlen(parent_path);
		int i;

		printf("hello_mknod");

		parent = find_path(parent_path);

		if (parent == NULL)
			return -ENOENT;
		if ( find_path(path) != NULL )
			return -EEXIST;

		node = (inode*)calloc(1, sizeof(inode));
		node->parent = parent;
		node->st.st_nlink = 1;
		node->st.st_mode = mode;
		node->st.st_mtime = current_time;
		node->st.st_ctime = current_time;
		node->st.st_atime = current_time;
		node->child = NULL;
		node->next = NULL;

		node->name = (char*)calloc(MAX_NAME_LEN, sizeof(char));

		for( i = len ; path[i] != '\0' ; i++ )
			node->name[i-len] = path[i];
		node->name[i-len] = '\0';

		if(parent->child == NULL){
			parent->child = node;
			node->before = NULL;
		}
		else {
			for(last_node = parent->child; last_node->next != NULL;
				last_node = last_node->next);
			last_node->next = node;
			node->before = last_node;
		}
	printf("hello_mknod end");
	return 0;
}

static int hello_unlink(const char *path)
{
	inode* node;
	node = find_path(path);

 	printf("hello_rmdir");

	if (node == root_node ||
		((node->st.st_mode & S_IFDIR) && ( node->child )) )
		return -EISDIR;

	if(node == node->parent->child){
		if(node->next)
			node->next->before = NULL;
		node->parent->child = node->next;
		if(node->data)
			free(node->data);
		free(node->name);
		free(node);
	}
	else {
		node->before->next = node->next;
		if(node->next)
			node->next->before = node->before;
		if(node->data)
			free(node->data);
		free(node->name);
		free(node);
	}
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
	inode* node = find_path(path);
	time_t current_time = time(NULL);

	printf("hello_utimens");

	if(node == NULL)
		return -ENOENT;
	node->st.st_mtime = current_time;
	node->st.st_atime = current_time;

	printf("hello_utimens end");
	return 0;
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
	umask(0);
	return fuse_main(argc, argv, &hello_oper, NULL);
}
