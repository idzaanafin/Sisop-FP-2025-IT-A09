#define FUSE_USE_VERSION 26
#define _FILE_OFFSET_BITS 64

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

static const char *source_dir = "/home/idzoyy/Sisop-FP-2025-IT-A09/source_dir";
static int is_admin = 0; // 0 = user (read-only), 1 = admin (full access)

static void fullpath(char fpath[PATH_MAX], const char *path) {
    sprintf(fpath, "%s%s", source_dir, path);
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int res = lstat(fpath, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;
    char fpath[PATH_MAX];
    fullpath(fpath, path);

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
    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi) {
    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;
    close(fd);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi) {
    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    int res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset,
                     struct fuse_file_info *fi) {
    if (!is_admin) return -EROFS;

    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int fd = open(fpath, O_WRONLY);
    if (fd == -1)
        return -errno;

    int res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    if (!is_admin) return -EROFS;

    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int fd = creat(fpath, mode);
    if (fd == -1)
        return -errno;

    close(fd);
    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode) {
    if (!is_admin) return -EROFS;

    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int res = mkdir(fpath, mode);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_unlink(const char *path) {
    if (!is_admin) return -EROFS;

    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int res = unlink(fpath);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_rmdir(const char *path) {
    if (!is_admin) return -EROFS;

    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int res = rmdir(fpath);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_rename(const char *from, const char *to) {
    if (!is_admin) return -EROFS;

    char ffrom[PATH_MAX];
    char fto[PATH_MAX];
    fullpath(ffrom, from);
    fullpath(fto, to);
    int res = rename(ffrom, fto);
    if (res == -1)
        return -errno;
    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .open    = xmp_open,
    .read    = xmp_read,
    .write   = xmp_write,
    .create  = xmp_create,
    .mkdir   = xmp_mkdir,
    .unlink  = xmp_unlink,
    .rmdir   = xmp_rmdir,
    .rename  = xmp_rename,
};

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--password=", 11) == 0) {
            const char *pass = argv[i] + 11;
            if (strcmp(pass, "sisop2025") == 0) {
                is_admin = 1;
                printf("[INFO] Akses admin diberikan.\n");
            } else {
                printf("[INFO] Password salah! Mode read-only aktif.\n");
            }

            // Hapus argumen password agar tidak dilempar ke FUSE
            for (int j = i; j < argc - 1; j++) {
                argv[j] = argv[j + 1];
            }
            argc--;
            break;
        }
    }

    return fuse_main(argc, argv, &xmp_oper, NULL);
}
