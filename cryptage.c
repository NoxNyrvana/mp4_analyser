#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define KEY_SIZE 16 // Size of the encryption key

void generate_key(unsigned char *key) {
    printf("Enter the encryption key (16 bytes, hexadecimal): ");
    char input_key[KEY_SIZE * 2 + 1];
    scanf("%s", input_key);
    for (int i = 0; i < KEY_SIZE; i++) {
        sscanf(&input_key[i * 2], "%hhx", &key[i]);
    }
}

void encrypt_video(const char *input_file, const char *output_file, unsigned char *key) {
    FILE *input_fp = fopen(input_file, "rb");
    if (input_fp == NULL) {
        perror("Error opening input file");
        exit(1);
    }

    FILE *output_fp = fopen(output_file, "wb");
    if (output_fp == NULL) {
        perror("Error opening output file");
        exit(1);
    }

    unsigned char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), input_fp)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            buffer[i] ^= key[i % KEY_SIZE]; 
        }
        fwrite(buffer, 1, bytes_read, output_fp);
    }

    fclose(input_fp);
    fclose(output_fp);
}


void decrypt_video(const char *input_file, const char *output_file, unsigned char *key) {
    FILE *input_fp = fopen(input_file, "rb");
    if (input_fp == NULL) {
        perror("Error opening input file");
        exit(1);
    }

    FILE *output_fp = fopen(output_file, "wb");
    if (output_fp == NULL) {
        perror("Error opening output file");
        exit(1);
    }

    unsigned char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), input_fp)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            buffer[i] ^= key[i % KEY_SIZE]; 
        }
        fwrite(buffer, 1, bytes_read, output_fp);
    }

    fclose(input_fp);
    fclose(output_fp);
}

int main(int argc, char *argv[]) {
    unsigned char key[KEY_SIZE];
    generate_key(key);
    
    char imput[100];
    strcpy(imput, argv[1]);
    encrypt_video(imput, "encrypted.mp4", key);
    printf("Enter the encryption key (16 bytes, hexadecimal): ");
    char input_key[KEY_SIZE * 2 + 1];
    scanf("%s", input_key);
    for (int i = 0; i < KEY_SIZE; i++) {
        sscanf(&input_key[i * 2], "%hhx", &key[i]);
    }

    decrypt_video("encrypted.mp4", "decrypted.mp4", key);

    return 0;
}