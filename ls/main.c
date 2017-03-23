#include <argp.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * CLI arguments
 */
static struct argp_option options[] = {
	{"list", 'l', 0, 0, "Show a detailed list"},
};

struct arguments {
	short list;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;
	switch(key) {
	case 'l':
		arguments->list = 1;
		break;
	case ARGP_KEY_END:
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, 0, ""};

/*
 * ls command types
 */
typedef struct {
	char *name;
	unsigned char is_dir;
	long size;

} LsEntry;

typedef struct {
	int count;
	LsEntry **entries;
} Directory;

char* make_path(const char* path, const char *file) {
	char *filename;

	asprintf(&filename, "%s/%s", path, file);
	printf("filename: %s\n", filename);
	return filename;
}

Directory* collect_entries(const char *path) {
	struct dirent *entry;
	DIR *dir = opendir(path);

	Directory *currentDirectory = (Directory*) malloc(sizeof(Directory));
	currentDirectory->entries = (LsEntry**) malloc(sizeof(LsEntry*));
	currentDirectory->count = 1;

	if(dir == NULL) {
                return NULL;
	}

	while((entry = readdir(dir)) != NULL) {
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		char *name;
		asprintf(&name, "%s", entry->d_name);
		char *filename = make_path(path, entry->d_name);
		struct stat *buf = malloc(sizeof(struct stat));
		stat(filename, buf);

		LsEntry *currEntry = (LsEntry*) malloc(sizeof(currEntry));
		currEntry->name = name;
		currEntry->is_dir = (entry->d_type == DT_DIR ? 1 : 0);
		currEntry->size = buf->st_size;
		currentDirectory->entries[currentDirectory->count - 1] = currEntry;

		free(buf);

		currentDirectory->count++;
		currentDirectory->entries = realloc(currentDirectory->entries, currentDirectory->count * sizeof(LsEntry));
	}

	closedir(dir);
	return currentDirectory;
}


int main(int argc, char **argv) {
	struct arguments arguments;
	arguments.list = 0;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));

	if(cwd == NULL) {
		printf("Could not fetch currect working directory\n");
	}

	printf("Current working directory is %s\n", cwd);

	Directory *result = collect_entries(cwd);
	printf("Count %d\n", result->count - 1);

	for(int i = 0; i < result->count - 1; i++) {
		LsEntry *entry = result->entries[i];
		if(entry == NULL) {
			printf("entry %d is null", i);
		}
		printf("%s -", entry->name);
		printf("%c -", entry->is_dir);
		printf("%ld", entry->size);
	}

	free(result->entries);
	free(result);

	exit(0);
}
