# SoalShiftSISOP20_modul4_C01

### KELOMPOK        : C1
ANGGOTA         :

* Vieri Fath Ayuba     (05111840000153)
* Himawan Wijaya       (05111640000133)




### Soal Shift Modul 4

## SOAL 1

![3](https://user-images.githubusercontent.com/61290164/80866623-e3fbf480-8cb9-11ea-9df7-9633470a362d.PNG)

### Source Code

Untuk mengecek folder encv1_ atau encv2_
```

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
```

Fungsi mendapatkan ekstensi dari file
```
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
```


Fungsi enkripsi : yaitu dengan mennggeser karakter sebanyak 10
```
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
```


Fungsi Dekripsi : yaitu dengan menggeser karakter sebanyak 77 kali
```
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
```


Fuse Operation

```
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
```
## Screenshot Output

![documents](https://user-images.githubusercontent.com/61290164/80866631-ebbb9900-8cb9-11ea-859c-2f6a2f0bb163.PNG)
![filesys](https://user-images.githubusercontent.com/61290164/80866635-eeb68980-8cb9-11ea-9cbd-5b893665719b.PNG)

![1](https://user-images.githubusercontent.com/61290164/80866616-df374080-8cb9-11ea-8084-11d5bfac5e11.PNG)
![2](https://user-images.githubusercontent.com/61290164/80866621-e2cac780-8cb9-11ea-8c01-ba890436fcbf.PNG)

## SOAL 2

![4](https://user-images.githubusercontent.com/61290164/80866625-e65e4e80-8cb9-11ea-9a8a-5d269a49ea2d.PNG)


### Belum selesai

Untuk mengecek folder encv1_ atau encv2_
```

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
```


## SOAL 4

![5](https://user-images.githubusercontent.com/61290164/80866628-e8c0a880-8cb9-11ea-9b12-6fa42c5d67bc.PNG)

### Source Code

Membuat suatu LOGFILE yang berada di /home/vierifath/. Log file berisi info local time serta log akan dibagi menjadi beberapa level yaitu INFO dan WARNING. Untuk log level WARNING, merupakan pencatatan log untuk syscall rmdir dan unlink. 


```
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
```
Format untuk logging sesuai dengan perintah soal

```
fprintf(fp,"WARNING::%s::%s::%s\n",waktu,command,path);
fprintf(fp,"INFO::%s::%s::%s\n",waktu,command,path);   
```

## Screenshot Output

![filelog](https://user-images.githubusercontent.com/61290164/80866632-ececc600-8cb9-11ea-9ecc-e29c81af48aa.PNG)

