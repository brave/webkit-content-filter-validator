#include <wpe/webkit.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#define __USE_XOPEN_EXTENDED true
#include <ftw.h>
#undef __USE_XOPEN_EXTENDED

#define STDIN_MALLOC_SIZE 4096

// Removes a file. Intended for use with `nftw`.
int rmFiles(const char* pathname, const struct stat* sbuf, int type, struct FTW* ftwb) {
    if (remove(pathname)) {
        return -1;
    }
    return 0;
}

// Custom data structure for `finishedCallback`.
typedef struct FilterSaveData {
    GMainLoop* mainLoop;
    WebKitUserContentFilter* filter;
    GError* error;
} FilterSaveData;

// Callback for `webkit_user_content_filter_store_save`.
void finishedCallback(WebKitUserContentFilterStore* store, GAsyncResult* res, FilterSaveData* data) {
    data->filter = webkit_user_content_filter_store_save_finish(store, res, &data->error);
    g_main_loop_quit(data->mainLoop);
}

// Attempts to compile `data` into a content filter store.
// Returns nonzero and populates `error_msg` if any failure is encountered.
// Don't forget to free(error_msg) if this returns nonzero.
int try_compile(void* data, size_t data_size, char** error_msg) {
    const gchar *storage_path = "./ContentFilterStore.tmp";
    WebKitUserContentFilterStore* store = webkit_user_content_filter_store_new(storage_path);

    FilterSaveData saveData = { NULL, };

    GBytes* filter_data = g_bytes_new_static(data, data_size);
    webkit_user_content_filter_store_save(store,
                                          "test-filter",
                                          filter_data,
                                          NULL,
                                          (GAsyncReadyCallback) finishedCallback,
                                          &saveData);

    saveData.mainLoop = g_main_loop_new(NULL, false);
    g_main_loop_run(saveData.mainLoop);

    int exit_code;
    if (saveData.filter) {
        exit_code = 0;
    } else {
        int len = strlen(saveData.error->message) + 1;
        *error_msg = malloc(len);
        strncpy(*error_msg, saveData.error->message, len);
        exit_code = 1;
    }

    g_clear_pointer(&saveData.error, g_error_free);
    g_clear_pointer(&saveData.filter, webkit_user_content_filter_unref);
    g_main_loop_unref(saveData.mainLoop);

    // we have to cleanup `storage_path` ourselves since WebKit provides no
    // means to do so.
    nftw(storage_path, rmFiles, 2, FTW_DEPTH | FTW_MOUNT | FTW_PHYS);

    return exit_code;
}

int main(int argc, char** argv) {
    int fd;

    void* data;
    size_t data_size = 0;

    if (argc == 1) {
        // Read from stdin
        fd = fileno(stdin);

        data = malloc(STDIN_MALLOC_SIZE);
        size_t read_size;
        while ((read_size = fread(data + data_size, 1, STDIN_MALLOC_SIZE, stdin))) {
            data_size += read_size;
            if (feof(stdin)) {
                break;
            }
            int fe;
            if ((fe = ferror(stdin))) {
                printf("failed to read stdin: %i", fe);
                exit(fe);
            }
            data = realloc(data, data_size + STDIN_MALLOC_SIZE);
        }
    } else if (argc == 2 && argv[1][0] != '-') {
        // Read from specified file
        char* file = argv[1];

        fd = open(file, O_RDONLY);
        if (errno) {
            printf("could not open file '%s': error %i\n", file, errno);
            return errno;
        }

        struct stat statbuf;
        fstat(fd, &statbuf);
        data_size = statbuf.st_size;

        data = mmap(NULL, data_size, PROT_READ, MAP_PRIVATE, fd, 0);
    } else {
        printf("usage: \n");
        printf(" %s               read from stdin\n", argv[0]);
        printf(" %s <FILE>        read from <FILE>\n", argv[0]);
        exit(1);
    }

    char* error_message;
    int exitCode = try_compile(data, data_size, &error_message);
    if (exitCode) {
        printf("%s\n", error_message);
        free(error_message);
    }

    if (fd == fileno(stdin)) {
        free(data);
    } else {
        munmap(data, data_size);
    }

    return exitCode;
}
