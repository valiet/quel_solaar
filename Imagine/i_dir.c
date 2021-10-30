#include <stdio.h>
typedef unsigned int uint;

#ifdef _WIN32
#include <string.h> 
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>

typedef struct{
#ifdef UNICODE
	short			*d_name;
#else
	char			*d_name;
#endif
}dirent;
 
typedef struct{
	WIN32_FIND_DATA data; 
	HANDLE			handle;
	dirent			ent;
	uint			drive_letter;
}DIR;


DIR *opendir(char *path) 
{ 
	DIR *dir;
	if(path != NULL && path[0] != 0)
	{
		unsigned int i;
		WIN32_FIND_DATA data; 
		HANDLE handle; 
		char d[1025];
		short uni[1025];
		sprintf(d,"%s//*.*", path); 
#ifdef UNICODE
		for(i = 0; i < 1024 && d[i] !=0 ; i++)
			uni[i] = (char)d[i];
		uni[i] = 0;
		if((handle = FindFirstFile(uni, &data)) != INVALID_HANDLE_VALUE)
#else
		if((handle = FindFirstFile(d, &data)) != INVALID_HANDLE_VALUE)
#endif
		{ 
			dir = malloc(sizeof *dir);
			dir->handle = handle;
			dir->data = data;
			dir->ent.d_name = NULL;
			dir->drive_letter = -1;
	//		printf("FOUND\n");
			return dir;
		}
	//	printf("NOT FOUND -%s- (%d %d)\n", d, GetLastError(), ERROR_FILE_NOT_FOUND);
		return NULL;
	}else
	{
		dir = malloc(sizeof *dir);
		dir->handle = NULL;
		dir->ent.d_name = malloc((sizeof *dir->ent.d_name) * 4);
		dir->ent.d_name[0] = 64;
		dir->ent.d_name[1] = ':';
		dir->ent.d_name[2] = '/';
		dir->ent.d_name[3] = 0;
		dir->drive_letter = GetLogicalDrives() * 2;
	//	printf("Creating opendir\n");
		return dir;
	}
}

void closedir(DIR *dir) 
{
	if(dir->drive_letter == -1)
		FindClose(dir->handle);
	free(dir);
}

dirent *readdir(DIR *dir) 
{
	if(dir->drive_letter == -1)
	{
		if(dir->ent.d_name != NULL)
			if(FindNextFile(dir->handle, &dir->data) != TRUE)
				return NULL;
		dir->ent.d_name = dir->data.cFileName;
		return &dir->ent;
	}else
	{
		uint letter, drive, i;
		drive = dir->drive_letter;
		letter = dir->ent.d_name[0] - 64;
		letter++;
		for(i = 0; i < letter; i++)
			drive /= 2;

		for(i = letter; i < 32; i++)
		{
			if(drive % 2 == 1)
			{
				dir->ent.d_name[0] = 64 + i;
				return &dir->ent;
			}
			drive /= 2;
		}
		return NULL;
	}
}


void *imagine_path_watch(char *path, boolean subfoleders)
{
#ifdef UNICODE
	short uni[1025];
	uint i;
	for(i = 0; path[i] != 0 && i < 1024; i++)
		uni[i] = path[i];
	uni[i] = 0;
	return FindFirstChangeNotification(uni, subfoleders, FILE_NOTIFY_CHANGE_FILE_NAME);
#else
	return FindFirstChangeNotification(path, subfoleders, FILE_NOTIFY_CHANGE_FILE_NAME);
#endif
}

	#define DIR_ROOT_PATH ""
	#define DIR_HOME_PATH "."
#else

    #include <sys/types.h>
    #include <sys/dir.h>
	#include <dirent.h>
	#define DIR_ROOT_PATH "\"
	#define DIR_HOME_PATH "."
#endif

void imagine_drive_test()
{
	DIR *d;
	dirent *ent;
	return;
	d = opendir(DIR_HOME_PATH);
	printf("d = %p -%s-\n", d, DIR_HOME_PATH);
	if(d != NULL)
	{
		ent = readdir(d);
		if(ent != NULL)
		{
			while(TRUE)
			{
				printf("-%s%s-\n", DIR_HOME_PATH, d->ent.d_name);
				ent = readdir(d);
				if(ent == NULL)
					break;
			}
		}
		closedir(d);
	}
}


boolean imagine_path_search(char *file, boolean partial, char *path, boolean folders, uint number, char *out_buffer, uint out_buffer_size)
{
	uint i, j, found = 0;
	DIR *d;
	dirent *ent;
	d = opendir(path);
	if(d != NULL)
	{
		ent = readdir(d);
		if(ent != NULL)
		{		
			out_buffer_size--;
			while(TRUE)
			{
				if(partial)
				{
					for(i = 0; d->ent.d_name[i] != 0; i++)
					{
						if(d->ent.d_name[i] == file[0])
						{
							for(j = 0; d->ent.d_name[i + j] != 0 && file[j] != 0 && (d->ent.d_name[i + j] == file[j]); j++);
							if(file[j] == 0)
							{
								if(found == number)
								{
									for(i = 0; d->ent.d_name[i] != 0 && i < out_buffer_size; i++)
										out_buffer[i] = d->ent.d_name[i];
									out_buffer[i] = 0;
									closedir(d);
									return TRUE;
								}
								found++;
							}
						}
					}
				}else
				{
					for(i = 0; d->ent.d_name[i] != 0 && d->ent.d_name[i] == file[i]; j++);
					if(d->ent.d_name[i] == file[i])
					{
						if(found == number)
						{
							for(i = 0; d->ent.d_name[i] != 0 && i < out_buffer_size; i++)
								out_buffer[i] = d->ent.d_name[i];
							out_buffer[i] = 0;
							closedir(d);
							return TRUE;
						}
						found++;
					}
				}

				ent = readdir(d);
				if(ent == NULL)
					break;
			}
		}
		closedir(d);
	}
	return FALSE;
}


void *imagine_path_open(char *path)
{
	return opendir(path);
}

boolean imagine_path_next(DIR *d, char *file_name_buffer, uint buffer_size)
{
	uint i;
	dirent *ent;
	if(d != NULL)
	{
		ent = readdir(d);
		if(ent != NULL)
		{
			buffer_size--;
			for(i = 0; d->ent.d_name[i] != 0 && i < buffer_size; i++)
				file_name_buffer[i] = d->ent.d_name[i];
			file_name_buffer[i] = 0;
			return TRUE;
		}
	}
	return FALSE;
}

boolean imagine_path_is_dir(char *path)
{
	DIR *d;
	d = opendir(path);
	if(d != NULL)
	{
		closedir(d);
		return TRUE;
	}
	return FALSE;
}

void imagine_path_close(DIR *d)
{
	closedir(d);
}

void imagine_path_test()
{
	char file[128];
	uint i;
	void *dir;

	dir = imagine_path_open("./");
	printf("dir Poninter %p\n", dir);
	if(dir != NULL)
	{
		for(i = 0; imagine_path_next(dir, file, 128); i++)
			printf("FILE:%s\n", file);
		imagine_path_close(dir);
	}
	exit(0);
}

/*
FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001   
#define FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002   
#define FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004   
#define FILE_NOTIFY_CHANGE_SIZE         0x00000008   
#define FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010   
#define FILE_NOTIFY_CHANGE_LAST_ACCESS  0x00000020   
#define FILE_NOTIFY_CHANGE_CREATION     0x00000040   
#define FILE_NOTIFY_CHANGE_SECURITY     0x00000100   */