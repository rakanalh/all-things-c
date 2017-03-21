#include <argp.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

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

static void list_dirs(const char *path) {
	struct dirent *entry;
	DIR *dir = opendir(path);

	if(dir == NULL) {
		return;
	}

	while((entry = readdir(dir)) != NULL) {
		printf("%s\n", entry->d_name);
	}

	closedir(dir);
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

	list_dirs(cwd);


	exit(0);
}
