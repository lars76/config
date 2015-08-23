#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

typedef struct {
    char *field;
    char *value;
} ConfigObject;

typedef struct {
    char *configFile;
    char *configFolder;
    char *configPath;
    ConfigObject **co;
    int objects;
} Config;

int CONFIG_write(Config *config);
int CONFIG_read(Config *config);
char *CONFIG_get_value(Config *config, const char *field);
int CONFIG_set_object(Config *config, const char *field, const char *value);
Config *CONFIG_init(const char *configFolder, const char *configFile);
int CONFIG_exit(Config *config);