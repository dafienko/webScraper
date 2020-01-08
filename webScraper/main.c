#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "curl/curl.h"

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

struct curl_slist *list = NULL;

static const char urlPrefix[] = "https://prnt.sc/";
static const char urlImagePrefix[] = "https://image.prntscr.com/image/";

int main()
{
    int sNum = 2334;
    char suffix[] = "ps";
    sprintf(suffix, "%s%d%c", suffix, sNum, 0);
    printf(suffix);
    static const char* outName = "web.txt";
    FILE *outputFile;

    CURL *cHandle = curl_easy_init();

    if (cHandle == NULL) {
        printf("curl failed to init\n");
        curl_easy_cleanup(cHandle);
        return 0;
    }

    char* url = calloc(sizeof(urlPrefix) / sizeof(char), sizeof(char));
    memcpy(url, &urlPrefix, sizeof(urlPrefix));
    sprintf(url, "%s%s", url, suffix);

    curl_easy_setopt(cHandle, CURLOPT_WILDCARDMATCH, 1L);
    curl_easy_setopt(cHandle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(cHandle, CURLOPT_HEADER, 1L);
    curl_easy_setopt(cHandle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(cHandle, CURLOPT_BUFFERSIZE, 4200000L);

    curl_easy_setopt(cHandle, CURLOPT_URL, url);

    list = curl_slist_append(list, "authority: prnt.sc" );
    list = curl_slist_append(list, "upgrade-insecure-requests: 1" );
    list = curl_slist_append(list, "user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36" );
    list = curl_slist_append(list, "sec-fetch-user: ?1" );
    list = curl_slist_append(list, "accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
    list = curl_slist_append(list, "sec-fetch-site: none" );
    list = curl_slist_append(list, "sec-fetch-mode: navigate");
    //list = curl_slist_append(list, "accept-encoding: gzip, deflate, br");
    list = curl_slist_append(list, "accept-language: en-US,en;q=0.9,la;q=0.8");
    list = curl_slist_append(list, "cookie: __cfduid=d4f3d3ca37dd8af7f2853003c2f3d78171578413117; _ga=GA1.2.253215967.1578413118; _gid=GA1.2.1923712756.1578413118");
    curl_easy_setopt(cHandle, CURLOPT_HTTPHEADER, list);

    outputFile = fopen(outName, "wb");

    char foundData = 0;
    if (outputFile) {
        foundData = 1;
        curl_easy_setopt(cHandle, CURLOPT_WRITEDATA, outputFile);

        curl_easy_perform(cHandle);

        fclose(outputFile);
    } else {
        printf("output file not found\n");
    }

    printf("done------------------------------------------------------\n");

    outputFile = fopen(outName, "r");
    char imageFound = 0;
    char* imageName;
    if (outputFile && foundData == 1) {
        printf("looking for image key--------------------------------------\n");
        char c;
        const int IMAGE_PREFIX_LENGTH = sizeof(urlImagePrefix) / sizeof(char);
        int countCorrect = 0;
        char imageNameBuffer[32];
        char imageBufferSize = 0;
        char loggingData = 0;
        do {
            c = fgetc(outputFile);
            if (loggingData) {
                if (c == '\"') {
                    loggingData = 0;
                    imageNameBuffer[imageBufferSize] = 0;
                    imageName = &imageNameBuffer;
                    imageFound = 1;
                    break;
                } else {
                    imageNameBuffer[imageBufferSize] = c;
                    imageBufferSize++;
                }
            } else {
                if (c == urlImagePrefix[countCorrect]) {
                    countCorrect++;
                    if (countCorrect == IMAGE_PREFIX_LENGTH - 1) { // we aren't looking for the 0 character at the end of urlImagePrefix
                        loggingData = 1;
                    }
                } else {
                    countCorrect = 0;
                }
            }
        } while (c != EOF);
        printf("%s-------------------------------------\n", imageNameBuffer);
        fclose(outputFile);
    }
    printf("%s-------------------------------------\n", imageName);

    curl_slist_free_all(list);
    curl_easy_cleanup(cHandle);


    if (imageFound && foundData) {
        printf("got it-----------------------------------\n");
        outputFile = fopen("out.png", "wb");
        if (outputFile != NULL) {
            printf("init curl again-----------------------------------\n");
            cHandle = curl_easy_init();
            printf("done-----------------------------------\n");

            curl_easy_setopt(cHandle, CURLOPT_VERBOSE, 1L);
            printf("done1-----------------------------------\n");
            curl_easy_setopt(cHandle, CURLOPT_WRITEFUNCTION, write_data);
            printf("done2-----------------------------------\n");
            curl_easy_setopt(cHandle, CURLOPT_BUFFERSIZE, 4200000L);
            printf("done3-----------------------------------\n");
            curl_easy_setopt(cHandle, CURLOPT_WRITEDATA, outputFile);
            printf("done4-----------------------------------\n");

            sprintf(url, "%s%s", urlImagePrefix, imageName);
            printf("%s-----------------------------------\n", url);
            curl_easy_setopt(cHandle, CURLOPT_URL, url);
            printf("done5-----------------------------------\n");

            printf("getting image-----------------------------------\n");
            curl_easy_perform(cHandle);
            printf("done-----------------------------------\n");

            curl_easy_cleanup(cHandle);
            fclose(outputFile);
        }
    }

    return 0;
}
















