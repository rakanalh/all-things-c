#include <argp.h>
#include <limits.h>
#include <stdlib.h>
#include "ls.h"

/*
 * CLI arguments
 */
static struct argp_option options[] = {
	{"size", 's', 0, 0, "Show size"},
	{"owner", 'o', 0, 0, "Show owner"},
	{"permissions", 'p', 0, 0, "Show permissions"}
};

struct arguments {
	unsigned char size;
	unsigned char owner;
	unsigned char permissions;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;
	switch(key) {
	case 's':
		arguments->size = 1;
		break;
	case 'o':
		arguments->owner = 1;
		break;
	case 'p':
		arguments->permissions = 1;
		break;
	case ARGP_KEY_END:
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, 0, ""};

int main(int argc, char **argv) {
	struct arguments arguments;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));

	if(cwd == NULL) {
		printf("Could not fetch currect working directory\n");
	}

	Options options = 0;
	if(arguments.size) {
		options |= SIZE;
	}
	if(arguments.owner) {
		options |= OWNER;
	}
	if(arguments.permissions) {
		options |= PERMISSIONS;
	}

	Directory *directory = collect_entries(cwd, options);

	for(int i = 0; i < directory->count - 1; i++) {
		LsEntry *entry = directory->entries[i];
		if(entry == NULL) {
			printf("entry %d is null", i);
		}

		if(arguments.permissions) {
			printf("%-20s", entry->permissions);
		}

		if(arguments.owner) {
			char *owner;
			asprintf(&owner, "%s:%s", entry->owner, entry->group);
			printf("%-20s", owner);
		}

		if(arguments.size) {
			printf("%-10s", entry->size);
		}

		printf("%-10s", entry->name);
		if(entry->is_link) {
			printf(" -> %s", entry->link_target);
		}
		printf("\n");
	}

	directory_free(directory);

	exit(0);
}
