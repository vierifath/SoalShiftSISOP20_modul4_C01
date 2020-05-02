#define FUSE_USE_VERSION 28
#include <assert.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
static const char *dirpath = "/home/vierifath/Documents";
char list[100] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";

int isFileExist(char *path) //jika 0 bukan file
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void logFile(char command[],char path[]){
    char waktu[1000];
    time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(waktu, "%04d%02d%02d-%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf("%s %s %s\n",waktu,command,path);
    FILE* fp = fopen("/home/vierifath/fs.log", "a");
    if((strcmp(command,"RMDIR")==0)||(strcmp(command,"UNLINK")==0)){
        fprintf(fp,"WARNING::%s::%s::%s\n",waktu,command,path);
    }
    else if ((strcmp(command,"MKDIR")==0)||(strcmp(command,"CREAT")==0) || (strcmp(command,"RENAME")==0)||(strcmp(command,"WRITE")==0))
    {
        fprintf(fp,"INFO::%s::%s::%s\n",waktu,command,path);        
    }
    
    fclose(fp);
}

int isDirectory(char *path)
{
    struct stat sb;
    stat(path, &sb);
    return S_ISDIR(sb.st_mode);
}

char *get_filename_ext(char *filename)
{
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot + 1;
}

void *getCurrentDir(char *path)
{
    char *token;
    char *directory;
    size_t length;

    token = strrchr(path, '/');

    if (token == NULL)
    {
        printf("Error"); /* You decide here */
        exit(1);
    }

    length = strlen(token);
    directory = malloc(length);
    memcpy(directory, token + 1, length);

    return directory;
}

int checkVersion(char path[])
{
    char temp[1000];
    char ver_1[6] = "encv1_";
    char ver_2[6] = "encv2_";
    strcpy(temp, path);
    char *new_path = temp;
    while (strlen(new_path) > 0)
    {
        char *dir = getCurrentDir(new_path);
        printf("dir %s\n",dir);
        if (strncmp(dir, "encv1_",strlen("encv1_")) == 0)
        {
            return 1;
        }
        else if (strncmp(dir, "encv2_",strlen("encv2_")) == 0)
        {
            return 2;
        }
        new_path[strlen(new_path) - strlen(dir)-1] = '\0';
    }
    return 0;
}

void enc(char *encrypt)
{
    if (!strcmp(encrypt, ".") || !strcmp(encrypt, ".."))
        return;
    int enkripsi = strlen(encrypt);
    for (int i = 0; i < enkripsi; i++)
    {
        for (int j = 0; j < 87; j++)
        {
            if (encrypt[i] == list[j])
            {
                encrypt[i] = list[(j + 10) % 87];
                break;
            }
        }
    }
}

void dec(char *decrypt)
{
    if (!strcmp(decrypt, ".") || !strcmp(decrypt, ".."))
        return;
    int dekripsi = strlen(decrypt);
    for (int i = 0; i < dekripsi; i++)
    {
        for (int j = 0; j < 87; j++)
        {
            if (decrypt[i] == list[j])
            {
                decrypt[i] = list[(j + 77) % 87];
                break;
            }
        }
    }
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res,versi;
    char fpath[1000],fullpath[1000];
    char name[1000],name1[1000];
    sprintf(name, "%s", path);
    versi = checkVersion(name);
    printf("name %s\n",name);
    sprintf(name1, "%s", path);
    sprintf(fullpath,"%s%s",dirpath,name);
    if (isDirectory(fullpath))
    {
        printf("MANA WOI %s",fullpath);
        res = lstat(fullpath,stbuf);
    }else
    {
        if (versi==1)
        {
            char *temp;
            temp = strrchr(name1, '/');
            name[strlen(name) - strlen(temp)] = '\0';
            printf("ini awal %s %s\n",temp,name);
            if(!isFileExist(fullpath)){
                char *ext = get_filename_ext(temp);
                if (strcmp(ext, "") != 0)
                {
                    temp[strlen(temp) - strlen(ext) - 1] = '\0';
                    dec(temp);
                    sprintf(temp, "%s.%s", temp, ext);
                }
                else
                {
                    dec(temp);
                }
            }
            sprintf(name, "%s%s",name, temp);
            sprintf(fpath, "%s%s", dirpath, name);
            printf("namenya %s getattr custom %s\n", name, fpath);
            res = lstat(fpath, stbuf);
        }else if (versi==2)
        {
            char new_path[1000];
            strcpy(new_path,fullpath);
            sprintf(new_path,"%s.000",new_path);
            res=lstat(new_path,stbuf);
            int count=0;
            struct stat sf;
            int total_file = 0;
            while (1)
            {
                printf("FULL PATH GAN %s\n",new_path);
                if (stat(new_path,&sf)<0)
                {
                    break;
                }
                count++;
                memset(new_path,'\0',sizeof(new_path));
                strcpy(new_path,fullpath);
                sprintf(new_path,"%s.%03d",new_path,count);
                total_file+=sf.st_size;
                printf("TOTAL FILE %d",total_file);
            }
            stbuf->st_size = total_file;
            printf("HASIL RES %d\n",res);
            return 0;
        }else
        {
            res = lstat(fullpath,stbuf);
        }
    }
    
    if (res != 0)
    {
        return -ENOENT;
    }
    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name, "%s", path);
    sprintf(fpath, "%s%s", dirpath, name);
    printf("mkdir %s\n", fpath);
    res = mkdir(fpath, mode);
    if (res == -1)
        return -errno;
    logFile("MKDIR",name);
    return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
    int res,versi;
    char fpath[1000],fullpath[1000];
    char name[1000],name1[1000];
    sprintf(name, "%s", path);
    versi = checkVersion(name);
    sprintf(name1, "%s", path);
    sprintf(fullpath,"%s%s",dirpath,name);
    if (isDirectory(fullpath))
    {
        
    }else
    {
        if (versi==1)
        {
            char *temp;
            temp = strrchr(name1, '/');
            name[strlen(name) - strlen(temp)] = '\0';
            char *ext = get_filename_ext(temp);
            if (strcmp(ext, "") != 0)
            {
                temp[strlen(temp) - strlen(ext) - 1] = '\0';
                dec(temp);
                sprintf(temp, "%s.%s", temp, ext);
            }
            else
            {
                dec(temp);
            }
            sprintf(name, "%s%s",name, temp);
            sprintf(fpath, "%s%s", dirpath, name);

        }else if (versi==2)
        {
            //versi2
        }
        sprintf(fpath, "%s%s", dirpath, name);
        res = truncate(fpath, size);
        if (res == -1)
            return -errno;
    }
	return 0;
}


static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    (void)fi;

    int res;
    char fpath[1000],full_path[1000];
    char name[1000],name1[1000];
    strcpy(name1,path);
    if (strcmp(path, "/") == 0)
    {
        sprintf(fpath, "%s", dirpath);
    }
    else
    {
        sprintf(name, "%s", path);
        sprintf(full_path, "%s%s", dirpath, name);
        printf("Path nge create: %s\n",full_path);
        
        if (!isFileExist(full_path))
        {
            sprintf(fpath, "%s%s", dirpath, name);
        }
    }
    printf("create %s\n", fpath);
    res = creat(fpath, mode);
    if (res == -1)
        return -errno;

    close(res);
    logFile("CREAT",name1);
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000], name[1000];
    int versi=0;
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", dirpath);
    }
    else
    {
        sprintf(name, "%s", path);
        versi = checkVersion(name);
        sprintf(fpath, "%s%s", dirpath, name);
    }
    int res = 0;
    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;
    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;
    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        char *temp, newPath[1000],full_path[1000];
        strcpy(newPath, de->d_name);
        sprintf(full_path,"%s/%s",fpath,newPath);
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
        {
            filler(buf, de->d_name, &st, 0);
            continue;
        }
        temp = newPath;
        if (versi==1)
        {
            //dec(temp);
            if (isFileExist(full_path))
            {
                char *ext = get_filename_ext(temp);
                if (strcmp(ext, "") != 0)
                {
                    temp[strlen(temp) - strlen(ext) - 1] = '\0';
                    enc(temp);
                    sprintf(temp, "%s.%s", temp, ext);
                }
                else
                {
                    enc(temp);
                }
            }
        }
        else if (versi==2)
        {
            if (isFileExist(full_path))
            {
                char *ext = get_filename_ext(temp);
                int bil = atoi(ext);
                if ((bil > 0 && bil < 1000) || strcmp(ext, "000") == 0){
                    temp[strlen(temp) - strlen(ext) - 1] = '\0';
                    res=filler(buf,temp, &st,0);
                    continue;
                    printf("hasil bagi %s",temp);
                }
                else
                {
                    char new_path[1000];
                    strcpy(new_path,full_path);
                    FILE * file = fopen(new_path, "rb");
                    int count = 0;
                    char topath[1000];
                    sprintf(new_path, "%s.%03d", new_path, count);
                    void * buffer = malloc(1024);
                    while(1){
                        size_t filesize = fread(buffer, 1, 1024, file);
                        if(filesize == 0)break;
                        FILE * op = fopen(new_path, "w");
                        fwrite(buffer, 1, filesize, op);
                        fclose(op);
                        count++;
                        memset(new_path,'\0',sizeof(new_path));
                        strcpy(new_path,full_path);
                        sprintf(new_path, "%s.%03d", new_path, count);
                    }
                    free(buffer);
                    fclose(file);
                    remove(full_path);
                    temp=(char *) malloc(sizeof(char)* sizeof(temp));
                    temp=de->d_name;
                }
            }
            
        }
        printf("akhirnya %s %s\n", de->d_name, temp);
        res = (filler(buf, temp, &st, 0));
        if (res != 0)
            break;
    }
    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    int res = 0;
    int fd = 0;
    char fpath[1000];
    char name[1000],name1[1000];
    if (strcmp(path, "/") == 0)
    {
        sprintf(fpath, "%s", dirpath);
    }
    else
    {
        int versi;
        sprintf(name, "%s", path);
        versi=checkVersion(name);
        sprintf(name1, "%s", path);
        if (versi==1)
        {
            char *temp;
            temp = strrchr(name1, '/');
            name[strlen(name) - strlen(temp)] = '\0';
            char *ext = get_filename_ext(temp);
            if (strcmp(ext, "") != 0)
            {
                temp[strlen(temp) - strlen(ext) - 1] = '\0';
                dec(temp);
                sprintf(temp, "%s.%s", temp, ext);
            }
            else
            {
                dec(temp);
            }
            sprintf(name, "%s%s",name, temp);
        }else if (versi==2)
        {
            printf("SINI GAN\n");
            char full_path[1000];
            int count = 0;
            sprintf(full_path, "%s%s", dirpath, name);
            FILE *fp = fopen(full_path,"a+");
            while (1)
            {
                sprintf(fpath, "%s%s.%03d", dirpath, name, count);
                FILE *filegabung =fopen(fpath,"r");
                if (filegabung==NULL)
                {
                    fclose(fp);
                    break;
                }
                
                size_t n, m;
                unsigned char buff[100000];
                do {
                        n = fread(buff, 1, sizeof buff, filegabung);
                        if (n) m = fwrite(buff, 1, n, fp);
                        else   m = 0;
                } while ((n > 0) && (n == m));
                
                fclose(filegabung);
            }
            
            return res;
        }
        sprintf(fpath, "%s%s", dirpath, name);
    }

    (void)fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_rename(const char *from, const char *to)
{
	char fpath_from[1000],fpath_to[1000];

    int res,versi;    
    sprintf(fpath_from,"%s%s",dirpath,from);
    versi = checkVersion(fpath_from);
    printf("SINIDULU\n");
    
    if (isDirectory(fpath_from))
    {
        sprintf(fpath_to,"%s%s",dirpath,to);
        printf("MAU RENAME %s %s\n",fpath_from,fpath_to);
        res = rename(fpath_from, fpath_to);
        if (res == -1)
            return -errno;
    }else
    {
        char name[1000],name1[1000];
        char fpath[1000],fullpath[1000];
        sprintf(name, "%s", from);
        sprintf(fpath_to,"%s%s",dirpath,to);
        sprintf(name1, "%s", from);
        sprintf(fullpath,"%s",name);
        if (versi==1)
        {
            char *temp;
            temp = strrchr(name1, '/');
            name[strlen(name) - strlen(temp)] = '\0';
            char *ext = get_filename_ext(temp);
            if (strcmp(ext, "") != 0)
            {
                temp[strlen(temp) - strlen(ext) - 1] = '\0';
                dec(temp);
                sprintf(temp, "%s.%s", temp, ext);
            }
            else
            {
                dec(temp);
            }
            sprintf(name, "%s%s",name, temp);
            sprintf(fpath, "%s%s", dirpath, name);

        }else if (versi==2)
        {
            //versi2
        }
        sprintf(fpath, "%s%s", dirpath, name);
        printf("RENAME LAGI %s\n",fpath);
        res = rename(fpath, fpath_to);
        if (res == -1)
            return -errno;
    }
    char tujuan[1000];
    sprintf(tujuan,"%s::%s",from,to);
    logFile("RENAME",tujuan);
	return 0;
}

static int xmp_unlink(const char *path)
{
	int res;
    
    char fpath[1000];
    char name[1000],name1[1000],name_path[1000];
    printf("Sebelum Unlink %s\n",path);
    if (strcmp(path, "/") == 0)
    {
        sprintf(fpath, "%s", dirpath);
    }
    else
    {
        sprintf(name, "%s", path);
        sprintf(fpath, "%s%s", dirpath, name);
        if(!isFileExist(fpath)){
            sprintf(name1, "%s", path);
            char *temp;
            temp = strrchr(name1, '/');
            name[strlen(name) - strlen(temp)] = '\0';
            char *ext = get_filename_ext(temp);
            if (strcmp(ext, "") != 0)
            {
                temp[strlen(temp) - strlen(ext) - 1] = '\0';
                dec(temp);
                sprintf(temp, "%s.%s", temp, ext);
            }
            else
            {
                dec(temp);
            }
            sprintf(name, "%s%s",name, temp);
            memset(fpath,'\0',sizeof(fpath));
            sprintf(fpath, "%s%s", dirpath, name);
        }
    }
    printf("Unlink %s\n",fpath);
	res = unlink(fpath);
	if (res == -1)
		return -errno;
    sprintf(name_path,"%s",path);
    logFile("UNLINK",name_path);
	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
    char fpath[1000],name_path[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    printf("rmdir %s\n",fpath);
	res = rmdir(fpath);
	if (res == -1)
		return -errno;
    sprintf(name_path,"%s",path);
    logFile("RMDIR",name_path);
	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	char fpath[1000], name[1000],name1[1000];
	strcpy(name, path);
	enc(name);

	if(strcmp(name,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else{
        int versi;
        sprintf(name, "%s", path);
        versi=checkVersion(name);
        sprintf(name1, "%s", path);
        if (versi==1)
        {
            char *temp;
            temp = strrchr(name1, '/');
            name[strlen(name) - strlen(temp)] = '\0';
            char *ext = get_filename_ext(temp);
            if (strcmp(ext, "") != 0)
            {
                temp[strlen(temp) - strlen(ext) - 1] = '\0';
                dec(temp);
                sprintf(temp, "%s.%s", temp, ext);
            }
            else
            {
                dec(temp);
            }
            sprintf(name, "%s%s",name, temp);
        }else if (versi==2)
        {
            

       
        }
        sprintf(fpath, "%s%s", dirpath, name);
    }

	int res;

	res = utimensat(0, fpath, ts, AT_SYMLINK_NOFOLLOW);
	
	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res,versi;
    char fpath[1000],fullpath[1000];
    char name[1000],name1[1000],pathlog[1000];
    sprintf(name, "%s", path);
    sprintf(pathlog, "%s", path);
    versi = checkVersion(name);
    printf("path to write : %s\n",path);
    sprintf(name1, "%s", path);
    sprintf(fullpath,"%s%s",dirpath,name);
    if (isDirectory(fullpath))
    {
        
    }else
    {
        if (versi==1)
        {
            if (!isFileExist(fullpath))
            {
                char *temp;
                temp = strrchr(name1, '/');
                name[strlen(name) - strlen(temp)] = '\0';
                printf("ini awal %s %s\n",temp,name);
                char *ext = get_filename_ext(temp);
                if (strcmp(ext, "") != 0)
                {
                    printf("1N ");
                    temp[strlen(temp) - strlen(ext) - 1] = '\0';
                    dec(temp);
                    sprintf(temp, "%s.%s", temp, ext);
                }
                else
                {
                    printf("2N ");
                    dec(temp);
                }
                sprintf(name, "%s%s",name, temp);
            }
            

        }else if (versi==2)
        {
            //versi2
        }
        sprintf(fpath, "%s%s", dirpath, name);
        printf("write %s\n",fpath);
        (void) fi;
        fd = open(fpath, O_WRONLY);
        if (fd == -1)
            return -errno;

        res = pwrite(fd, buf, size, offset);
        if (res == -1)
            res = -errno;
        
        close(fd);
    }
    logFile("WRITE",pathlog);
	return res;
}

void chopN(char *str, size_t n)
{
    assert(n != 0 && str != 0);
    size_t len = strlen(str);
    if (n > len)
        return;  // Or: n = len;
    memmove(str, str+n, len - n + 1);
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .mkdir = xmp_mkdir,
    .create = xmp_create,
    .rename = xmp_rename,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
    .truncate = xmp_truncate,
    .write = xmp_write,
    .utimens = xmp_utimens,
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}

