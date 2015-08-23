#include "config.h"

static int createFolder(const char *folderName) {
    if (folderName == NULL)
        return -1;

    struct stat s;
    int res = stat(folderName, &s);
    if (res == -1)
        res = mkdir(folderName, 0700);
    if (res != 0)
        return -1;

    return res;
}

static int createFile(const char *fileName) {
    if (fileName == NULL)
        return -1;

    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        file = fopen(fileName, "w");
        if (file == NULL)
            return -1;
    }

    fclose(file);

    return 0;
}

int CONFIG_write(Config *config) {
    if (config == NULL)
        return -1;

    FILE *file = fopen(config->configPath, "w+");
    if (file == NULL)
        return -1;

    int i;
    for (i = 0; i < config->objects; i++)
        fprintf(file, "%s=%s\n", config->co[i]->field, config->co[i]->value);

    fclose(file);

    return 0;
}

int CONFIG_read(Config *config) {
    if (config == NULL)
        return -1;

    FILE *file = fopen(config->configPath, "r");
    if (file == NULL)
        return -1;

    char line[128];
    char *find = NULL;
    int i = 0;
    while (fgets(line, sizeof line, file) != NULL) {
        find = strchr(line, '=');
        if (find != NULL) {
            config->co = realloc(config->co, (i + 1) * sizeof(ConfigObject));
            config->co[i] = malloc(sizeof(ConfigObject));

            size_t fieldSize = find-line;
            size_t valueSize = strlen(line)-fieldSize;
            config->co[i]->field = malloc(fieldSize + 1);
            config->co[i]->value = malloc(valueSize + 1);

            strncpy(config->co[i]->field, line, find-line);
            config->co[i]->field[fieldSize] = '\0';
            strncpy(config->co[i]->value, line + fieldSize + strlen("="), valueSize);
            config->co[i]->value[valueSize] = '\0';

            if (valueSize > 2 && config->co[i]->value[valueSize-2] == '\n')
                config->co[i]->value[valueSize-2] = '\0';

            i++;
        }
    }

    config->objects = i;

    fclose(file);

    return 0;
}

char *CONFIG_get_value(Config *config, const char *field) {
    if (config == NULL || field == NULL)
        return NULL;

    int i;
    for (i = 0; i < config->objects; i++) {
        if (strcmp(config->co[i]->field, field) == 0) {
            /* don't return an empty value */
            if (config->co[i]->value[1] != '\0')
                return config->co[i]->value;
            else
                return NULL;
        }
    }

    return NULL;
}

int CONFIG_set_object(Config *config, const char *field, const char *value) {
    if (config == NULL || field == NULL || value == NULL)
        return -1;

    /* change value if object already exists */
    int i;
    for (i = 0; i < config->objects; i++) {
        if (strcmp(field, config->co[i]->field) == 0) {
            free(config->co[i]->value);
            config->co[i]->value = malloc(strlen(value) + 1);
            strcpy(config->co[i]->value, value);

            return 0;
        }
    }

    i = config->objects;
    config->co = realloc(config->co, (i + 1) * sizeof(ConfigObject));
    config->co[i] = malloc(sizeof(ConfigObject));
    config->co[i]->field = malloc(strlen(field) + 1);
    strcpy(config->co[i]->field, field);

    config->co[i]->value = malloc(strlen(value) + 1);
    strcpy(config->co[i]->value, value);

    config->objects = i + 1;

    return 0;
}

Config *CONFIG_init(const char *configFolder, const char *configFile) {
    if (configFolder == NULL || configFile == NULL)
        return NULL;

    Config *config = malloc(sizeof(Config));
    config->co = malloc(sizeof(ConfigObject));
    config->objects = 0;

    config->configFolder = malloc(strlen(configFolder) + 1);
    strcpy(config->configFolder, configFolder);

    config->configFile = malloc(strlen(configFile) + 1);
    strcpy(config->configFile, configFile);

    config->configPath = malloc(strlen(configFolder) + strlen(configFile) + strlen("/") + 1);
    strcpy(config->configPath, configFolder);
    strcat(config->configPath, "/");
    strcat(config->configPath, configFile);

    int result = createFolder(config->configFolder);
    if (result != 0) {
        CONFIG_exit(config);
        return NULL;
    }

    result = createFile(config->configPath);
    if (result != 0) {
        CONFIG_exit(config);
        return NULL;
    }

    return config;
}

int CONFIG_exit(Config *config) {
    if (config == NULL)
        return -1;

    free(config->configFile);
    free(config->configFolder);
    free(config->configPath);

    int i;
    for (i = 0; i < config->objects; i++) {
        free(config->co[i]->field);
        free(config->co[i]->value);
        free(config->co[i]);
    }

    free(config->co);
    free(config);

    return 0;
}