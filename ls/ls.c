#define _GNU_SOURCE
#include <dirent.h>
#include <grp.h>
#include <limits.h>
#include <math.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ls.h"

/*
 * ls command types
 */


Directory* directory_new() {
	Directory *current_dir = (Directory*) malloc(sizeof(Directory));
	// malloc one entry ahead
	current_dir->entries = (LsEntry**) malloc(sizeof(LsEntry));
	current_dir->count = 1;
	return current_dir;
}

void directory_free(Directory *directory) {
	for(int i = 0; i < directory->count - 1; i++) {
		if(directory->entries[i] == NULL) continue;
		free(directory->entries[i]);
	}
	free(directory->entries);
	free(directory);
}

LsEntry* entry_new() {
	LsEntry *entry = (LsEntry*) malloc(sizeof(LsEntry));
	entry->name = NULL;
	entry->path = NULL;
	entry->link_target = NULL;
	entry->owner = NULL;
	entry->group = NULL;
	entry->permissions = NULL;
	entry->is_link = 0;
	entry->is_dir = 0;
	entry->size = 0;

	return entry;
}

/*
 * make_path concatenates path and filename
 */
static char* make_path(const char* path, const char *file) {
	char *filename;
	asprintf(&filename, "%s/%s", path, file);
	return filename;
}

static char* humanize_size(float size) {
	char sizes[] = {'B', 'K', 'M', 'G'};
	char human_size = sizes[0];

	int i = 1;
	while(size >= 1024) {
		human_size = sizes[i];
		i++;
		size /= 1024;
	}

	char* str_size;
	asprintf(&str_size, "%.1f %c", size, human_size);
	return str_size;
}

static char* get_file_permissions(struct stat buf) {
	char *permissions = (char *) malloc(11 * sizeof(char));
	permissions[0] = S_ISDIR(buf.st_mode)  ? 'd' : '-';
	permissions[1] = buf.st_mode & S_IRUSR ? 'r' : '-';
	permissions[2] = buf.st_mode & S_IWUSR ? 'w' : '-';
	permissions[3] = buf.st_mode & S_IXUSR ? 'x' : '-';
	permissions[4] = buf.st_mode & S_IRGRP ? 'r' : '-';
	permissions[5] = buf.st_mode & S_IWGRP ? 'w' : '-';
	permissions[6] = buf.st_mode & S_IXGRP ? 'x' : '-';
	permissions[7] = buf.st_mode & S_IROTH ? 'r' : '-';
	permissions[8] = buf.st_mode & S_IWOTH ? 'w' : '-';
	permissions[9] = buf.st_mode & S_IXOTH ? 'x' : '-';
	permissions[10] = '\0';
	return permissions;
}

static void populate_entry_details(LsEntry *entry, Options options) {
	// Construct full file name
	char *filename = make_path(entry->path, entry->name);

	// Fetch file size info
	struct stat buf;

	lstat(filename, &buf);

	if(options & SIZE) {
		entry->size = humanize_size(buf.st_size);
	}

	if(S_ISLNK(buf.st_mode)) {
		entry->is_link = 1;

		ssize_t bufsize = buf.st_size + 1;
		char *link_name = malloc(bufsize);
		ssize_t r = readlink(filename, link_name, bufsize);
		if(r == -1) {
			perror("Could not fetch symlink");
			exit(EXIT_FAILURE);
		}
		link_name[r] = '\0';
		entry->link_target = link_name;

	}

	if(options & OWNER) {
		struct group *grp;
		struct passwd *pwd;
		grp = getgrgid(buf.st_gid);
		pwd = getpwuid(buf.st_uid);

		entry->owner = pwd->pw_name;
		entry->group = grp->gr_name;
	}

	if(options & PERMISSIONS) {
		entry->permissions = get_file_permissions(buf);
	}

	free(filename);
}

Directory* collect_entries(const char *path, Options options) {
	struct dirent *dent;
	DIR *dir = opendir(path);

	if(dir == NULL) {
                return NULL;
	}

	Directory *current_dir = directory_new();
	while((dent = readdir(dir)) != NULL) {
		if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0) {
			continue;
		}

		// Copy name
		char *name, *entry_path;
		asprintf(&name, "%s", dent->d_name);
		asprintf(&entry_path, "%s", path);

		// Create entry
		LsEntry *entry = entry_new();
		entry->name = name;
		entry->path = entry_path;
		entry->is_dir = (dent->d_type == DT_DIR ? 1 : 0);

		if(options) {
			populate_entry_details(entry, options);
		}

		// Populate the previously malloc'ed location
		current_dir->entries[current_dir->count - 1] = entry;

		current_dir->count++;
		// malloc one entry ahead
		current_dir->entries = realloc(current_dir->entries, current_dir->count * sizeof(LsEntry));
	}

	closedir(dir);
	return current_dir;
}
