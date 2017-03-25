typedef enum {
	SIZE= 0x01,
	PERMISSIONS = 0x02,
	OWNER = 0x04
} Options;

typedef struct {
	char *name;
	char *path;
	char *link_target;
	char *owner;
	char *group;
	char *permissions;
	unsigned char is_link;
	unsigned char is_dir;
	char *size;

} LsEntry;

typedef struct {
	int count;
	LsEntry **entries;
} Directory;

Directory* collect_entries(const char *path, Options flags);

Directory *directory_new();
void directory_free(Directory *directory);
