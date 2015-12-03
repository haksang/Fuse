
#define FUSE_USE_VERSION 26
#define PATH_MAX 200

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

char *mount_path;

static void makepath(char fpath[PATH_MAX], const char *path)
{
  strcpy(fpath, mount_path);
  strncat(fpath, path, PATH_MAX);
}

static void* hello_init(struct fuse_conn_info *conn)
{
	(void) conn;
  char buf[255];
  getcwd(buf,255);
  int i = strlen(buf);
  mount_path = (char*)calloc(i, sizeof(char));
  strcat(mount_path,buf);
  puts(mount_path);

	return NULL;
}

static int hello_getattr(const char *path, struct stat *stbuf)
{
  puts("hello_getattr");
  int res;

  char fpath[PATH_MAX];
  makepath(fpath,path);
  printf("getattr : fpath is %s\n",fpath);

	res = lstat(fpath, stbuf);

	if (res != 0)
		return -errno;
  puts("hello_getattr end");
	return 0;
}

static int hello_access(const char *path, int mask)
{
  puts("hello_access");
  char fpath[PATH_MAX];
  makepath(fpath,path);

  printf("access : fpath is %s\n",fpath);

  int res;
	res = access(fpath, mask);
	if (res < 0)
		return -errno;

  puts("hello_access end");
	return 0;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  puts("hello_readdir");
  char fpath[PATH_MAX];
  makepath(fpath,path);
  printf("readdir : fpath is %s\n",fpath);
	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;

  dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
  puts("hello_readdir end");
	return 0;
}

static int hello_mknod(const char *path, mode_t mode, dev_t rdev)
{
  puts("hello_mknod");
  char fpath[PATH_MAX];
  makepath(fpath,path);

	int res;
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fpath, mode);
	else
		res = mknod(fpath, mode, rdev);
	if (res == -1)
		return -errno;
  puts("hello_mknod end");
	return 0;
}

static int hello_mkdir(const char *path, mode_t mode)
{
  puts("hello_mkdir");
  char fpath[PATH_MAX];
  makepath(fpath,path);

	int res;
	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;
  puts("hello_mkdir end");
	return 0;
}

static int hello_unlink(const char *path)
{
  puts("hello_unlink");
  char fpath[PATH_MAX];
  makepath(fpath,path);

	int res;
	res = unlink(fpath);
	if (res == -1)
		return -errno;
  puts("hello_unlink end");
	return 0;
}

static int hello_rmdir(const char *path)
{
  puts("hello_rmdir");
  char fpath[PATH_MAX];
  makepath(fpath,path);

	int res;
	res = rmdir(fpath);
	if (res == -1)
		return -errno;
  puts("hello_rmdir end");
	return 0;
}
static int hello_chmod(const char *path, mode_t mode)
{
  puts("hello_chmod");
  char fpath[PATH_MAX];
  makepath(fpath,path);

	int res;
	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;
  puts("hello_chmod end");
	return 0;
}

static int hello_chown(const char *path, uid_t uid, gid_t gid)
{
  puts("hello_chown");
  char fpath[PATH_MAX];
  makepath(fpath,path);

	int res;
	res = lchown(fpath, uid, gid);
	if (res == -1)
		return -errno;
  puts("hello_chown end");
	return 0;
}

static int hello_truncate(const char *path, off_t size)
{
  puts("hello_truncate");
  char fpath[PATH_MAX];
  makepath(fpath,path);

	int res;
	res = truncate(fpath, size);
	if (res == -1)
		return -errno;
  puts("hello_truncate end");
	return 0;
}


static int hello_open(const char *path, struct fuse_file_info *fi)
{
  puts("hello_open");
  char fpath[PATH_MAX];
  makepath(fpath,path);

	int res;
	res = open(fpath, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
  puts("hello_open end");
	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  puts("hello_read");
  char fpath[PATH_MAX];
  makepath(fpath,path);

	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
  puts("hello_read end");
	return res;
}

static int hello_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
  puts("hello_write");
  char fpath[PATH_MAX];
  makepath(fpath,path);

	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
  puts("hello_write end");
	return res;
}

static struct fuse_operations hello_oper = {
  .init 		= hello_init,
	.getattr	= hello_getattr,
	.access		= hello_access,
	.readdir	= hello_readdir,
	.mknod		= hello_mknod,
	.mkdir		= hello_mkdir,
	.unlink		= hello_unlink,
	.rmdir		= hello_rmdir,
	.chmod		= hello_chmod,
	.chown		= hello_chown,
	.truncate	= hello_truncate,
	.open		= hello_open,
	.read		= hello_read,
	.write		= hello_write,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &hello_oper, NULL);
}
