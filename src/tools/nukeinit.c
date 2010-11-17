#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

typedef struct initrd_node
{
    char *name;
    char type;
    char *data;
    size_t size;
    int position_name;
    int position_data;
    int position_info;
    struct initrd_node *next;
    struct initrd_node *root;
} initrd_node_t;

#define TYPE_FILE 1
#define TYPE_DIRECTORY 2

initrd_node_t *files = NULL;

void scan(DIR *root, struct initrd_node *initrd_root)
{
    struct dirent *file;

    while ((file = readdir(root)) != NULL)
    {
        if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, ".."))
        {
            continue;
        }
            
        if (file->d_type == DT_DIR)
        {
            DIR *sub = opendir(file->d_name);

            assert(sub);

            initrd_node_t *initrd_dir = calloc(1, sizeof(initrd_node_t));

            initrd_dir->name = malloc(strlen(file->d_name) + 1);
            strcpy(initrd_dir->name, file->d_name);
            initrd_dir->name[strlen(file->d_name)] = 0;
            initrd_dir->type = TYPE_DIRECTORY;
            initrd_dir->root = initrd_root;

            initrd_dir->next = files;
            files = initrd_dir;

            chdir(file->d_name);
            scan(sub, initrd_dir);
            closedir(sub);
            chdir("..");
        }
        else
        {
            FILE *entry = fopen(file->d_name, "rb");

            assert(entry);

            initrd_node_t *initrd_file = calloc(1, sizeof(initrd_node_t));

            initrd_file->name = malloc(strlen(file->d_name) + 1);
            strcpy(initrd_file->name, file->d_name);
            initrd_file->name[strlen(file->d_name)] = 0;
            initrd_file->type = TYPE_FILE;
            fseek(entry, 0, SEEK_END);
            initrd_file->size = ftell(entry);
            rewind(entry);
            initrd_file->data = malloc(initrd_file->size);
            fread(initrd_file->data, 1, initrd_file->size, entry);
            initrd_file->root = initrd_root;

            initrd_file->next = files;
            files = initrd_file;

            fclose(entry);
        }
    }
}

int main(char argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: nukeinit <path-to-root>\r\n");

        return 1;
    }

    DIR *root = opendir(argv[1]);

    if (root == NULL)
    {
        perror("Could not read root directory");

        return 1;
    }
    
    FILE *image = fopen("nukeinit.img", "wb");

    assert(image);

    chdir(argv[1]);

    scan(root, NULL);

    closedir(root);

    fwrite("NUKEINIT\0\0\0\0\0\0\0\0", 1, 16, image);

    initrd_node_t *node = files;

    while(node != NULL)
    {
        node->position_name = ftell(image);

        fwrite(node->name, 1, strlen(node->name) + 1, image);

        node = node->next;
    }

    node = files;

    while(node != NULL)
    {
        if(node->type == TYPE_FILE)
        {
            node->position_data = ftell(image);

            fwrite(node->data, 1, node->size, image);
        }

        node = node->next;
    }

    node = files;

    int count = 0;
    int start = 0;
    int root_null = 0;
    int size = 0;

    while(node != NULL)
    {
        node->position_info = ftell(image);

        if(node->type == TYPE_DIRECTORY)
        {
            fseek(image, 12, SEEK_CUR);
        }
        else if(node->type == TYPE_FILE)
        {
            fseek(image, 16, SEEK_CUR);
        }

        count++;

        node = node->next;
    }

    start = files->position_info;

    fseek(image, 8, SEEK_SET);
    fwrite(&count, 1, 4, image);
    fwrite(&start, 1, 4, image);

    while(files != NULL)
    {
        initrd_node_t *node = files;

        size += node->size;

        printf("%1u: ", node->type);
        
        if (node->root != NULL)
        {
            printf("/%s", node->root->name);
        }

        printf("/%s @ %#x (%u bytes @ %#x)\r\n", node->name, node->position_info, node->size, node->position_data);

        fseek(image, node->position_info, SEEK_SET);

        if(node->type == TYPE_DIRECTORY)
        {
            fwrite(&node->type, 1, 4, image);
            fwrite(&node->position_name, 1, 4, image);

            if(node->root != NULL)
            {
                fwrite(&node->root->position_info, 1, 4, image);
            }
            else
            {
                fwrite(&root_null, 1, 4, image);
            }
        }
        else if(node->type == TYPE_FILE)
        {
            fwrite(&node->type, 1, 4, image);
            fwrite(&node->position_name, 1, 4, image);
            
            if(node->root != NULL)
            {
                fwrite(&node->root->position_info, 1, 4, image);
            }
            else
            {
                fwrite(&root_null, 1, 4, image);
            }

            fwrite(&node->position_data, 1, 4, image);
        }

        files = files->next;

        if(node->type == TYPE_FILE)
        {
            free(node->data);
        }

        free(node->name);
        free(node);
    }

    fclose(image);

    printf("%u files (%u bytes total)\r\n", count, size);

    return 0;
}
