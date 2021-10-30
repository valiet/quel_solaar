#include <math.h>
#include <stdio.h>
#include "forge.h"
#include "imagine.h"

void open_dir(char *path)
{
	char buffer[2048], full_path[2048];
	IDir *dir;
	dir = imagine_path_open(path);
	if(dir != NULL)
	{
		while(imagine_path_next(dir, buffer, 2048))
		{
			if(buffer[0] != '.')
			{
			//	printf("%s\n", buffer);
				sprintf(full_path, "%s/%s", path, buffer);
				if(imagine_path_is_dir(full_path))
					open_dir(full_path);
			}
		}
		imagine_path_close(dir);
	}
}


// 
// IMAGINE_DIR_ROOT_PATH
// 
int main()
{
//	while(TRUE)
//		open_dir(IMAGINE_DIR_HOME_PATH);
		open_dir("D:");
}