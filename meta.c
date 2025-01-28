#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_METADATA 128

typedef struct {
  char key[128];
  char value[256];
} Metadata;

typedef struct {
    uint32_t size;
    char type[4];
} Atom;

uint32_t read_uint32(const unsigned char *buffer) {
  return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

long get_file_size(FILE *file) {
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);
  return size;
}

void calculate_framerate_and_ratio(FILE *file, Metadata *metadata, size_t *meta_count) {
    unsigned char buffer[BUFFER_SIZE];
    int width = 0, height = 0;
    int tkhd_processed = 0;

    fseek(file, 0, SEEK_SET);

    while (fread(buffer, 1, BUFFER_SIZE, file) > 0) {
        for (size_t i = 0; i < BUFFER_SIZE - 4; i++) {
            // Détection de la boîte 'tkhd' pour obtenir la largeur et la hauteur
            if (memcmp(&buffer[i], "tkhd", 4) == 0 && !tkhd_processed) {
                fseek(file, (long)(ftell(file) - BUFFER_SIZE + i + 4), SEEK_SET);
                fread(buffer, 1, 100, file);
                width = read_uint32(buffer + 76) >> 16;
                height = read_uint32(buffer + 80) >> 16;
                double aspect_ratio = (double)width / height;

                // Stockage des métadonnées de largeur, hauteur et ratio d'aspect
                snprintf(metadata[*meta_count].key, sizeof(metadata[*meta_count].key), "width");
                snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "%d", width);
                (*meta_count)++;

                snprintf(metadata[*meta_count].key, sizeof(metadata[*meta_count].key), "height");
                snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "%d", height);
                (*meta_count)++;

                snprintf(metadata[*meta_count].key, sizeof(metadata[*meta_count].key), "aspect_ratio");
                if (aspect_ratio == 1) {
                    snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "1:1");
                } else if (aspect_ratio == 4.0 / 3.0) {
                    snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "4:3");
                } else if (aspect_ratio == 16.0 / 9.0) {
                    snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "16:9");
                } else if (aspect_ratio == 16.0 / 10.0) {
                    snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "16:10");
                } else if (aspect_ratio == 21.0 / 9.0) {
                    snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "21:9");
                } else {
                    snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "Unknown aspect ratio");
                }
                (*meta_count)++;

                tkhd_processed = 1;
            }
        }
    }
}

void extract_authors_and_directors(FILE *file, Metadata *metadata, size_t *meta_count) {
    unsigned char buffer[BUFFER_SIZE];
    fseek(file, 0, SEEK_SET);
    while (fread(buffer, 1, BUFFER_SIZE, file) > 0) {
        for (size_t i = 0; i < BUFFER_SIZE - 4; i++) {
            if (memcmp(&buffer[i], "udta", 4) == 0) {
                long udta_offset = ftell(file) - BUFFER_SIZE + i;
                fseek(file, udta_offset, SEEK_SET);
                fread(buffer, 1, BUFFER_SIZE, file);
                for (size_t j = 0; j < BUFFER_SIZE - 4; j++) {
                    if (memcmp(&buffer[j], "auth", 4) == 0) {
                        char author[256] = {0};
                        strncpy(author, (char *)&buffer[j + 4], sizeof(author) - 1);
                        snprintf(metadata[*meta_count].key, sizeof(metadata[*meta_count].key), "author");
                        snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "%s", author);
                        (*meta_count)++;
                    } else if (memcmp(&buffer[j], "dire", 4) == 0) {
                        char director[256] = {0};
                        strncpy(director, (char *)&buffer[j + 4], sizeof(director) - 1);
                        snprintf(metadata[*meta_count].key, sizeof(metadata[*meta_count].key), "director");
                        snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "%s", director);
                        (*meta_count)++;
                    }
                }
                return;
            }
        }
    }
}

void calculate_duration(FILE *file, Metadata *metadata, size_t *meta_count) {
    unsigned char buffer[BUFFER_SIZE];

    fseek(file, 0, SEEK_SET);
    while (fread(buffer, 1, BUFFER_SIZE, file) > 0) {
        for (size_t i = 0; i < BUFFER_SIZE - 4; i++) {
            if (memcmp(&buffer[i], "mvhd", 4) == 0) {
                fseek(file, (long)(ftell(file) - BUFFER_SIZE + i + 4), SEEK_SET);
                fread(buffer, 1, 100, file);
                uint32_t timescale = read_uint32(buffer + 12);
                uint32_t duration_ticks = read_uint32(buffer + 16);
                double duration_seconds = (double)duration_ticks / timescale;

                snprintf(metadata[*meta_count].key, sizeof(metadata[*meta_count].key), "duration");
                snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "%.2f seconds", duration_seconds);
                (*meta_count)++;
                return;
            }
        }
    }
    snprintf(metadata[*meta_count].key, sizeof(metadata[*meta_count].key), "duration");
    snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "Unknown duration");
    (*meta_count)++;
}

void extract_creation_date(FILE *file, Metadata *metadata, size_t *meta_count) {
    unsigned char buffer[BUFFER_SIZE];
    fseek(file, 0, SEEK_SET);  // Aller au début du fichier

    while (fread(buffer, 1, BUFFER_SIZE, file) > 0) {
        for (size_t i = 0; i < BUFFER_SIZE - 4; i++) {
            // Recherche du marqueur "mvhd"
            if (memcmp(&buffer[i], "mvhd", 4) == 0) {
                long mvhd_offset = ftell(file) - BUFFER_SIZE + i;
                fseek(file, mvhd_offset + 4, SEEK_SET);  // Aller à la position de la boîte 'mvhd'
                if (fread(buffer, 1, 100, file) <= 0) {
                    // Si la lecture de la section "mvhd" échoue, retour
                    return;
                }

                uint8_t version = buffer[0];
                uint32_t creation_time;

                if (version == 0) {
                    // Version 0: les temps sont des entiers de 32 bits
                    creation_time = read_uint32(buffer + 4);
                } else if (version == 1) {
                    // Version 1: les temps sont des entiers de 64 bits
                    creation_time = (uint32_t)read_uint32(buffer + 12);  // Utiliser les 32 bits inférieurs
                }

                // Convertir le temps MP4 en temps Unix
                time_t raw_time = creation_time - 2082844800U;
                struct tm *timeinfo = gmtime(&raw_time);
                char creation_date[256];
                strftime(creation_date, sizeof(creation_date), "%Y-%m-%d %H:%M:%S", timeinfo);

                // Stocker le résultat dans les métadonnées
                snprintf(metadata[*meta_count].key, sizeof(metadata[*meta_count].key), "creation_date");
                snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "%s", creation_date);
                (*meta_count)++;  // Incrémenter le compteur de métadonnées

                return;  // Quitter après avoir trouvé la date
            }
        }
    }
    snprintf(metadata[*meta_count].key, sizeof(metadata[*meta_count].key), "creation_date");
    snprintf(metadata[*meta_count].value, sizeof(metadata[*meta_count].value), "1970-01-01 00:00:00");
    (*meta_count)++;
}

int extract_mp4_metadata(FILE *file, Metadata *metadata, size_t *meta_count) {
    unsigned char buffer[BUFFER_SIZE];
    size_t count = 0;

    long file_size = get_file_size(file);
    snprintf(metadata[count].key, sizeof(metadata[count].key), "file_size");
    snprintf(metadata[count].value, sizeof(metadata[count].value), "%ld bytes", file_size);
    count++;

    extract_authors_and_directors(file, metadata, &count);
    calculate_duration(file, metadata, &count);
    calculate_framerate_and_ratio(file, metadata, &count);
    extract_creation_date(file, metadata, &count);
    *meta_count = count;
    return 0;
}

int extract_metadata(const char *filename, Metadata *metadata, size_t *meta_count) {
    const char *extension = strrchr(filename, '.');
    if (!extension || extension == filename) {
        return -1;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    int result = extract_mp4_metadata(file, metadata, meta_count);
    fclose(file);
    return result;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    Metadata metadata[MAX_METADATA];
    size_t meta_count = 0;

    int result = extract_metadata(filename, metadata, &meta_count);
    if (result == 0) {
        printf("Filename: %s\n", filename);
        const char *extension = strrchr(filename, '.');
        if (extension) {
            printf("Extension: %s\n", extension + 1);
        }
        for (size_t i = 0; i < meta_count; i++) {
            printf("%s: %s\n", metadata[i].key, metadata[i].value);
        }
    } else {
        printf("Failed to extract metadata.\n");
    }

    return 0;
}