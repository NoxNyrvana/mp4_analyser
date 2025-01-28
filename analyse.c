#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <jansson.h>

struct Memory {
    char *response;
    size_t size;
};

static size_t WriteMemory(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = (char *)realloc(mem->response, mem->size + realsize + 1);
    if(ptr == NULL) {
        printf("pas assez de mémoire (le realloc a renvoyé NULL)");
        return 0;
    }
    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

void scan_file(const char *api_key, const char *file_path, char *analysis_id) {
    CURL *curl;
    CURLcode res;
    struct Memory chunk = {0};

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headerlist = NULL;
        char api_key_header[100];
        snprintf(api_key_header, sizeof(api_key_header), "x-apikey: %s", api_key);

        headerlist = curl_slist_append(headerlist, api_key_header);

        curl_mime *mime;
        curl_mimepart *part;

        mime = curl_mime_init(curl);
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "file");
        curl_mime_filedata(part, file_path);

        curl_easy_setopt(curl, CURLOPT_URL, "https://www.virustotal.com/api/v3/files");
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemory);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
            json_t *root;
            json_error_t error;
            root = json_loads(chunk.response, 0, &error);
            if (root) {
                json_t *data = json_object_get(root, "data");
                if (json_is_object(data)) {
                    json_t *id = json_object_get(data, "id");
                    if (json_is_string(id)) {
                        strcpy(analysis_id, json_string_value(id));
                    }
                }
                json_decref(root);
            }
        }

        curl_easy_cleanup(curl);
        curl_mime_free(mime);
        curl_slist_free_all(headerlist);
        free(chunk.response);
    }
    curl_global_cleanup();
}

void get_analysis_results(const char *api_key, const char *analysis_id) {
    CURL *curl;
    CURLcode res;
    struct Memory chunk = {0};

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headerlist = NULL;
        char api_key_header[100];
        snprintf(api_key_header, sizeof(api_key_header), "x-apikey: %s", api_key);

        headerlist = curl_slist_append(headerlist, api_key_header);

        char url[256];
        snprintf(url, sizeof(url), "https://www.virustotal.com/api/v3/analyses/%s", analysis_id);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemory);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
            json_t *root;
            json_error_t error;
            root = json_loads(chunk.response, 0, &error);
            if (root) {
                json_t *data = json_object_get(root, "data");
                if (json_is_object(data)) {
                    json_t *attributes = json_object_get(data, "attributes");
                    if (json_is_object(attributes)) {
                        json_t *date = json_object_get(attributes, "date");
                        if (json_is_integer(date)) {
                            printf("Date : %lld\n", json_integer_value(date));
                        }

                        json_t *results = json_object_get(attributes, "results");
                        if (json_is_object(results)) {
                            const char *key;
                            json_t *value;
                            json_object_foreach(results, key, value) {
                                json_t *category = json_object_get(value, "category");
                                json_t *engine_name = json_object_get(value, "engine_name");
                                json_t *engine_version = json_object_get(value, "engine_version");
                                json_t *engine_update = json_object_get(value, "engine_update");
                                json_t *method = json_object_get(value, "method");
                                json_t *result = json_object_get(value, "result");
                                json_t *size = json_object_get(value, "size");
                                printf("Moteur : %s\n", key);
                                printf("  Catégorie : %s\n", json_is_string(category) ? json_string_value(category) : "N/A");
                                printf("  Nom du moteur : %s\n", json_is_string(engine_name) ? json_string_value(engine_name) : "N/A");
                                printf("  Version du moteur : %s\n", json_is_string(engine_version) ? json_string_value(engine_version) : "N/A");
                                printf("  Mise à jour du moteur : %s\n", json_is_string(engine_update) ? json_string_value(engine_update) : "N/A");
                                printf("  Méthode : %s\n", json_is_string(method) ? json_string_value(method) : "N/A");
                                printf("  Résultat : %s\n", json_is_string(result) ? json_string_value(result) : "N/A");
                                if (json_is_integer(size)) {
                                    printf("  Taille : %lld\n", json_integer_value(size));
                                } else {
                                    printf("  Taille : N/A\n");
                                }
                            }
                        }

                        json_t *md5 = json_object_get(attributes, "md5");
                        if (json_is_string(md5)) {
                            printf("MD5 : %s\n", json_string_value(md5));
                        }

                        json_t *sha256 = json_object_get(attributes, "sha256");
                        if (json_is_string(sha256)) {
                            printf("SHA256 : %s\n", json_string_value(sha256));
                        }

                        json_t *stats = json_object_get(attributes, "stats");
                        if (json_is_object(stats)) {
                            const char *key;
                            json_t *value;
                            json_object_foreach(stats, key, value) {
                                printf("  %s : %lld\n", key, json_integer_value(value));
                            }
                        }

                        json_t *status = json_object_get(attributes, "status");
                        if (json_is_string(status)) {
                            printf("Statut : %s\n", json_string_value(status));
                        }
                    } else {
                        printf("Erreur : Champ 'attributes' non trouvé dans la réponse JSON.\n");
                    }
                } else {
                    printf("Erreur : Champ 'data' non trouvé dans la réponse JSON.\n");
                }
                json_decref(root);
            } else {
                printf("Erreur : Réponse JSON invalide. Erreur : %s\n", error.text);
            }
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headerlist);
        free(chunk.response);
    }
    curl_global_cleanup();
}

int main(int argc, char *argv[]) {
    const char *api_key = "bc4b3a01c79aad16b4433f2d945abc5a101588733d72a1286386bf7ff956f484";
    char analysis_id[100] = {0};

    const char *file_path = argv[1];

    scan_file(api_key, file_path, analysis_id);

    if (strlen(analysis_id) > 0) {
        get_analysis_results(api_key, analysis_id);
    } else {
        printf("Erreur : Impossible d'obtenir l'ID d'analyse.\n");
    }

    return 0;
}