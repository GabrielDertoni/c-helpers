#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <colors.h>
#include <limits.h>
#include <skip_list.h>

#define nsprintf(dest, args...) ({ \
        char *__dest = (dest); \
        int res = sprintf(__dest, args); \
        __dest[res] = ' '; \
        res; \
})

int horizontal_sequence_len(skip_node_t *start) {
    int i;
    for (i = 0; start != NULL; start = skip_node_next(start), i++);
    return i;
}

int vertical_sequence_len(skip_node_t *start) {
    int i;
    for (i = 0; start != NULL; start = skip_node_below(start), i++);
    return i;
}

int sprint_box_str(char *value, char **buff, int line, int column) {
    char middle[strlen(value) + 5];
    sprintf(middle, "| %s |", value);

    int len = strlen(middle);

    char horiz[len + 1];
    memset(horiz, '-', len);
    horiz[0] = horiz[len-1] = '+';
    horiz[len] = '\0';

    nsprintf(buff[line++] + column, "%s", horiz);
    // printf("| %-*s |\n", len - 4, "");
    nsprintf(buff[line++] + column, "%s", middle);
    // printf("| %-*s |\n", len - 4, "");
    nsprintf(buff[line++] + column, "%s", horiz);
    return len;
}

int get_box_str_width(char *value) {
    char middle[strlen(value) + 5];
    sprintf(middle, "| %s |", value);
    return strlen(middle);
}

int sprint_box(int value, char **buff, int line, int column) {
    char middle[20];
    sprintf(middle, "| %d |", value);

    int len = strlen(middle);

    char horiz[len + 1];
    memset(horiz, '-', len);
    horiz[0] = horiz[len-1] = '+';
    horiz[len] = '\0';

    nsprintf(buff[line++] + column, "%s", horiz);
    // printf("| %-*s |\n", len - 4, "");
    nsprintf(buff[line++] + column, "%s", middle);
    // printf("| %-*s |\n", len - 4, "");
    nsprintf(buff[line++] + column, "%s", horiz);
    return len;
}

int get_box_width(int value) {
    char middle[20];
    sprintf(middle, "| %d |", value);
    return strlen(middle);
}

void print_skip_list(skip_list_t *l) {
    skip_node_t *bottom;
    for (bottom = skip_list_head(l);
         skip_node_below(bottom);
         bottom = skip_node_below(bottom));
    
    uint len = skip_list_size(l);
    int vert_len = vertical_sequence_len(skip_list_head(l));
    char **buff = (char **)calloc(vert_len * 32, sizeof(char *));
    for (int i = 0; i < vert_len * 32; i++) {
        buff[i] = (char *)malloc(len * 32 * sizeof(char));
        memset(buff[i], ' ', len * 32 * sizeof(char));
        buff[i][len * 32 - 1] = '\0';
    }

    int column = 0, line = 0;
    for (skip_node_t *head_ptr = skip_list_head(l); head_ptr; head_ptr = skip_node_below(head_ptr)) {
        column = 0;
        skip_node_t *bottom_ptr, *ptr;
        for (bottom_ptr = bottom, ptr = head_ptr; ptr && bottom_ptr; ptr = skip_node_next(ptr)) {
            // Spacing until next box.
            while (bottom_ptr && skip_list_compare(l, bottom_ptr, ptr) != 0) {
                bottom_ptr = skip_node_next(bottom_ptr);
                column += nsprintf(buff[line+1] + column, "--");

                if (bottom_ptr && skip_node_value(bottom_ptr) != skip_node_value(ptr)) {
                    int width = get_box_str_width(skip_node_value(bottom_ptr)) + 1;
                    for (int k = 0; k < width; k++)
                        column += nsprintf(buff[line+1] + column, "-");
                }
            }
            if (bottom_ptr != bottom)
                column += nsprintf(buff[line+1] + column, ">");

            // Draw box with number inside.
            int box_len = sprint_box_str(skip_node_value(ptr), buff, line, column);
            // Arrow down pointing at the node below.
            if (skip_node_below(ptr)) {
                nsprintf(buff[line+3] + column + box_len / 2, "|");
                nsprintf(buff[line+4] + column + box_len / 2, "|");
                nsprintf(buff[line+5] + column + box_len / 2, "V");
            }
            column += box_len;
        }

        // Puts last box on the right meaning the end of the skip list layer.
        if (bottom_ptr) {
            for (bottom_ptr = skip_node_next(bottom_ptr); bottom_ptr; bottom_ptr = skip_node_next(bottom_ptr)) {
                int width = get_box_str_width(skip_node_value(bottom_ptr));
                column += nsprintf(buff[line+1] + column, "---");
                for (int k = 0; k < width; k++)
                    column += nsprintf(buff[line+1] + column, "-");
            }
            column += nsprintf(buff[line+1] + column, "-->");
        }
        int box_len = sprint_box_str(ELEM_MAX, buff, line, column);
        if (skip_node_below(head_ptr)) {
            nsprintf(buff[line+3] + column + box_len / 2, "|");
            nsprintf(buff[line+4] + column + box_len / 2, "|");
            nsprintf(buff[line+5] + column + box_len / 2, "V");
        }

        column += box_len;
        line += 6;
    }

    for (int i = 0; i < line - 3; i++)
        printf("%s\n", buff[i]);

    for (int i = 0; i < vert_len * 32; i++) free(buff[i]);
    free(buff);
}

void visual_test_insert_remove() {
    skip_list_t *l = skip_list_create(5, compare);

    printf(MAGENTA "--> Insert 'Hello'" RESET "\n");
    skip_list_insert(l, strdup("Hello"));

    printf(MAGENTA "--> Insert 'World'" RESET "\n");
    skip_list_insert(l, strdup("World"));

    printf(MAGENTA "--> Insert 'how" RESET "\n");
    skip_list_insert(l, strdup("how"));

    printf(MAGENTA "--> Insert 'are'" RESET "\n");
    skip_list_insert(l, strdup("are"));
    
    printf(MAGENTA "--> Insert 'you?'" RESET "\n");
    skip_list_insert(l, strdup("you?"));

    print_skip_list(l);

    printf(MAGENTA "--> Remove 'world'" RESET "\n");
    skip_list_remove_free(l, "World", heap_free);

    print_skip_list(l);

    skip_list_destroy(l, heap_free);
}

int main() {
    skip_list_t *l = skip_list_create(5, compare);

    while (true) {
        char command[30];
        scanf(" %[^\r\n]s", command);
        if (!strcmp(command, "ADD")) {
            char *str = (char *)malloc(30 * sizeof(char));
            scanf(" %[^\r\n]s", str);
            printf(MAGENTA "--> Insert '%s'" RESET "\n", str);
            skip_list_insert(l, str);
        } else if (!strcmp(command, "REMOVE")) {
            char *str = (char *)malloc(30 * sizeof(char));
            scanf(" %[^\r\n]s", str);
            printf(MAGENTA "--> Remove '%s'" RESET "\n", str);
            skip_list_remove(l, str);
            free(str);
        } else if (!strcmp(command, "PRINT")) {
            if (skip_list_size(l) > 0)
                print_skip_list(l);
        } else {
            break;
        }
    }

    skip_list_destroy(l, heap_free);
    return EXIT_SUCCESS;
}
