// C
//Program care primeste ca argumente in linia de comanda doua directoare d1 si d2 (d1 exista, d2 nu) si copiaza recursiv toata arborescenta cu originea in d1 intr-o arborescenta cu originea in d2 (intre cele doua arborescente difera doar numele directorului origine).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

void copy_dir(char *d1, char *d2)
{
	DIR *dir;
	struct dirent *entry;
	struct stat st;
	char *path1, *path2;
	int len;

	if ((dir = opendir(d1)) == NULL) {
		perror("opendir");
		exit(1);
	}

	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 ||
		    strcmp(entry->d_name, "..") == 0)
			continue;

		len = strlen(d1) + strlen(entry->d_name) + 2;
		path1 = malloc(len);
		if (path1 == NULL) {
			perror("malloc");
			exit(1);
		}
		snprintf(path1, len, "%s/%s", d1, entry->d_name);

		len = strlen(d2) + strlen(entry->d_name) + 2;
		path2 = malloc(len);
		if (path2 == NULL) {
			perror("malloc");
			exit(1);
		}
		snprintf(path2, len, "%s/%s", d2, entry->d_name);

		if (lstat(path1, &st) < 0) {
			perror("lstat");
			exit(1);
		}

		if (S_ISDIR(st.st_mode)) {
			if (mkdir(path2, st.st_mode) < 0) {
				perror("mkdir");
				exit(1);
			}
			copy_dir(path1, path2);
		} else if (S_ISREG(st.st_mode)) {
			FILE *f1, *f2;
			char buf[BUFSIZ];
			size_t n;

			if ((f1 = fopen(path1, "r")) == NULL) {
				perror("fopen");
				exit(1);
			}

			if ((f2 = fopen(path2, "w")) == NULL) {
				perror("fopen");
				exit(1);
			}

			while ((n = fread(buf, 1, BUFSIZ, f1)) > 0)
				if (fwrite(buf, 1, n, f2) != n) {
					perror("fwrite");
					exit(1);
				}

			if (fclose(f1) == EOF) {
				perror("fclose");
				exit(1);
			}

			if (fclose(f2) == EOF) {
				perror("fclose");
				exit(1);
			}
		} else {
			fprintf(stderr, "Not a regular file or directory: %s\n",
				path1);
			exit(1);
		}

		free(path1);
		free(path2);
	}

	if (closedir(dir) < 0) {
		perror("closedir");
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <dir1> <dir2>\n", argv[0]);
		exit(1);
	}

	if (mkdir(argv[2], 0755) < 0) {
		perror("mkdir");
		exit(1);
	}

	copy_dir(argv[1], argv[2]);

	return 0;
}