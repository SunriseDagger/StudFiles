#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

struct element {
	char name[NAME_MAX];
	int linkCount;
	short type;	// 0 - файл, 1 - папка
};

void sort(struct element *dirs, int n) {
	int i, j;
	struct element tmp;
	for (i = 0; i < n - 1; i++) {
		for (j = 0; j < n - i - 1; j++) {
			if (dirs[j].linkCount < dirs[j + 1].linkCount || dirs[j].type == 1) {
				tmp = dirs[j];
				dirs[j] = dirs[j + 1];
				dirs[j + 1] = tmp;
			}
		}
	}
}

int main (int argc, char *argv[]) {
	DIR *home;
	if (argc > 1)
		home = opendir (argv[1]);
	else
		home = opendir(".");
	struct dirent *curDir;
	struct element allFiles[100];
	struct stat curStat;
	int n = 0, i;

	while ((curDir = readdir(home)) != NULL) {
		lstat (curDir->d_name, &curStat);
		if (curStat.st_mode & S_IFDIR) {
			if (strcmp( curDir->d_name, ".") != 0 && strcmp(curDir->d_name, "..") != 0) {
				strcpy(allFiles[n].name, curDir->d_name);
				allFiles[n].linkCount = 1;
				allFiles[n].type = 1;
				n++;
			}
		} 
		else {
			strcpy (allFiles[n].name, curDir->d_name);
			allFiles[n].linkCount = curStat.st_nlink;
			allFiles[n].type = 0;
			n++;
		}
	}
	closedir(home);

	sort(allFiles, n);
	
	if (n == 0)
		printf ("Files not found");
	else {
		printf ("Link: %d\n", allFiles[0].linkCount);
		printf ("\t%s\n", allFiles[0].name);
		for (i = 1; i < n && allFiles[i].type != 1; i++) {
			if (allFiles[i].linkCount != allFiles[i-1].linkCount) {
				printf ("Link: %d\n", allFiles[i].linkCount);
				printf ("\t%s\n", allFiles[i].name);
			} else {
				printf ("\t%s\n", allFiles[i].name);
			}
		}
		if(i < n)
			printf ("\nFolders: \n");
		while (i < n) {
			printf("\t%s\n", allFiles[i].name);
			i++;
		}
	}
	printf("Press Enter key to continue...\n");
	getchar();
    return 0;
}
