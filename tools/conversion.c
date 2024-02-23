#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define HEADER_SIZE sizeof(uint32_t)
// convert all .sms files in the current directory to the 'converted' directory
// by adding a 4-byte header to the beginning of each file containing the file size
int main() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(".");
    if (dir == NULL) {
        perror("Error opening directory");
        return 1;
    }

    // Create 'converted' directory if it doesn't exist
    struct stat st = {0};
    if (stat("./converted", &st) == -1) {
        mkdir("./converted", 0700);
    }

    // Read each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".sms") != NULL) {
            FILE *source_file, *destination_file;
            char destination_filename[100];
            uint32_t file_size;

            // Open the source file in binary read mode
            source_file = fopen(entry->d_name, "rb");
            if (source_file == NULL) {
                printf("Error opening source file: %s\n", entry->d_name);
                continue;
            }

            // Create destination filename in the 'converted' directory
            sprintf(destination_filename, "./converted/%s", entry->d_name);

            // Open the destination file in binary write mode
            destination_file = fopen(destination_filename, "wb");
            if (destination_file == NULL) {
                printf("Error opening destination file: %s\n", destination_filename);
                fclose(source_file);
                continue;
            }

            // Seek to the end of the source file to get the file size
            fseek(source_file, 0, SEEK_END);
            file_size = ftell(source_file);
            rewind(source_file);

            // Write the file size as header to the destination file
            fwrite(&file_size, HEADER_SIZE, 1, destination_file);

            // Buffer to hold data while copying
            char buffer[1024];
            size_t bytes_read;

            // Copy contents of source file to destination file
            while ((bytes_read = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
                fwrite(buffer, 1, bytes_read, destination_file);
            }

            // Close the files
            fclose(source_file);
            fclose(destination_file);

            printf("File copied successfully: %s\n", entry->d_name);
        }
    }

    closedir(dir);

    return 0;
}

