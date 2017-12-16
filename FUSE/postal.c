// AHMET TURK - TUGBA OZKAL
// 150120107  - 150120053

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#define FILE_SIZE 36304
#define NAME_LENGTH 30
#define INFO_LENGTH 200

typedef struct data{
    char code[NAME_LENGTH];
    char neighborhood[NAME_LENGTH];
    char city[NAME_LENGTH];
    char district[NAME_LENGTH];
    char latitude[NAME_LENGTH];
    char longitude[NAME_LENGTH];  
}Data;

typedef struct folder{
	char name[NAME_LENGTH];
	struct folder *next;
	struct folder *child;
	char info[INFO_LENGTH];
}Folder;

Folder *NAMES;
Folder *CODES;
char* cityName;

void readFromFile(char * filename){
	Data * k = malloc(sizeof(Data));
	NAMES = NULL;
    FILE * file;
    file = fopen(filename, "r+");
    fseek(file, 0, SEEK_SET);
    int i;
    for(i = 0; i < FILE_SIZE; i++){
        fscanf(file, "%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%s\n", k->code, k->neighborhood, k->city, k->district, k->latitude, k->longitude);
        strcpy(k->code, k->code + 1);
        k->longitude[strlen(k->longitude)-2] = '\0';
        int isAdded = 0;
        if (NAMES == NULL) {
			NAMES = malloc(sizeof(Folder));
			strcpy(NAMES->name, k->city);
			NAMES->child = malloc(sizeof(Folder));
			strcpy(NAMES->child->name, k->district);
			NAMES->child->child = malloc(sizeof(Folder));
			strcpy(NAMES->child->child->name, k->neighborhood);
			strcpy(NAMES->child->child->info, "code: ");
			strcat(NAMES->child->child->info, k->code);
			strcat(NAMES->child->child->info, "\nneighborhood: ");
			strcat(NAMES->child->child->info, k->neighborhood);
			strcat(NAMES->child->child->info, "\ncity: ");
			strcat(NAMES->child->child->info, k->city);
			strcat(NAMES->child->child->info, "\ndistrict: ");
			strcat(NAMES->child->child->info, k->district);
			strcat(NAMES->child->child->info, "\nlatitude: ");
			strcat(NAMES->child->child->info, k->latitude);
			strcat(NAMES->child->child->info, "\nlongitude: ");
			strcat(NAMES->child->child->info, k->longitude);
			strcat(NAMES->child->child->info, "\n");
		}
		else {
			Folder *traverseCity = NAMES;
			Folder *previousCity;
			while(!isAdded && traverseCity != NULL) {
				if (strcmp(traverseCity->name, k->city) == 0) {
					Folder *traverseDistrict = traverseCity->child;
					Folder *previousDistrict;
					while(!isAdded && traverseDistrict != NULL) {
						if (strcmp(traverseDistrict->name, k->district) == 0) {
							Folder *traverseHood = traverseDistrict->child;
							while(traverseHood->next != NULL) {
								traverseHood = traverseHood->next;
							}
							traverseHood->next = malloc(sizeof(Folder));
							strcpy(traverseHood->next->name, k->neighborhood);
							strcpy(traverseHood->next->info, "code: ");
							strcat(traverseHood->next->info, k->code);
							strcat(traverseHood->next->info, "\nneighborhood: ");
							strcat(traverseHood->next->info, k->neighborhood);
							strcat(traverseHood->next->info, "\ncity: ");
							strcat(traverseHood->next->info, k->city);
							strcat(traverseHood->next->info, "\ndistrict: ");
							strcat(traverseHood->next->info, k->district);
							strcat(traverseHood->next->info, "\nlatitude: ");
							strcat(traverseHood->next->info, k->latitude);
							strcat(traverseHood->next->info, "\nlongitude: ");
							strcat(traverseHood->next->info, k->longitude);
							strcat(traverseHood->next->info, "\n");
							isAdded = 1;
						}
						previousDistrict = traverseDistrict;
						traverseDistrict = traverseDistrict->next;
					}
					if (!isAdded) {
						previousDistrict->next = malloc(sizeof(Folder));
						strcpy(previousDistrict->next->name, k->district);
						previousDistrict->next->child = malloc(sizeof(Folder));
						strcpy(previousDistrict->next->child->name, k->neighborhood);
						strcpy(previousDistrict->next->child->info, "code: ");
						strcat(previousDistrict->next->child->info, k->code);
						strcat(previousDistrict->next->child->info, "\nneighborhood: ");
						strcat(previousDistrict->next->child->info, k->neighborhood);
						strcat(previousDistrict->next->child->info, "\ncity: ");
						strcat(previousDistrict->next->child->info, k->city);
						strcat(previousDistrict->next->child->info, "\ndistrict: ");
						strcat(previousDistrict->next->child->info, k->district);
						strcat(previousDistrict->next->child->info, "\nlatitude: ");
						strcat(previousDistrict->next->child->info, k->latitude);
						strcat(previousDistrict->next->child->info, "\nlongitude: ");
						strcat(previousDistrict->next->child->info, k->longitude);
						strcat(previousDistrict->next->child->info, "\n");
						isAdded = 1;
					}
				}
				previousCity = traverseCity;
				traverseCity = traverseCity->next;
			}
			if (!isAdded) {
				previousCity->next = malloc(sizeof(Folder));
				strcpy(previousCity->next->name, k->city);
				previousCity->next->child = malloc(sizeof(Folder));
				strcpy(previousCity->next->child->name, k->district);
				previousCity->next->child->child = malloc(sizeof(Folder));
				strcpy(previousCity->next->child->child->name, k->neighborhood);
				strcpy(previousCity->next->child->child->info, "code: ");
				strcat(previousCity->next->child->child->info, k->code);
				strcat(previousCity->next->child->child->info, "\nneighborhood: ");
				strcat(previousCity->next->child->child->info, k->neighborhood);
				strcat(previousCity->next->child->child->info, "\ncity: ");
				strcat(previousCity->next->child->child->info, k->city);
				strcat(previousCity->next->child->child->info, "\ndistrict: ");
				strcat(previousCity->next->child->child->info, k->district);
				strcat(previousCity->next->child->child->info, "\nlatitude: ");
				strcat(previousCity->next->child->child->info, k->latitude);
				strcat(previousCity->next->child->child->info, "\nlongitude: ");
				strcat(previousCity->next->child->child->info, k->longitude);
				strcat(previousCity->next->child->child->info, "\n");
			}
		}
		isAdded = 0;
		if (CODES == NULL){
			CODES = malloc(sizeof(Folder));
			strncpy(CODES->name, k->code, 2);
			CODES->name[2] = '\0';
			CODES->child = malloc(sizeof(Folder));
			strcpy(CODES->child->name, k->code);
			strcpy(CODES->child->info, "code: ");
			strcat(CODES->child->info, k->code);
			strcat(CODES->child->info, "\nneighborhood: ");
			strcat(CODES->child->info, k->neighborhood);
			strcat(CODES->child->info, "\ncity: ");
			strcat(CODES->child->info, k->city);
			strcat(CODES->child->info, "\ndistrict: ");
			strcat(CODES->child->info, k->district);
			strcat(CODES->child->info, "\nlatitude: ");
			strcat(CODES->child->info, k->latitude);
			strcat(CODES->child->info, "\nlongitude: ");
			strcat(CODES->child->info, k->longitude);
			strcat(CODES->child->info, "\n");
		}
		else{
			Folder *tr = CODES;
			Folder *previousCode;
			while(!isAdded && tr != NULL) {
				if (strncmp(tr->name, k->code, 2) == 0) {
					Folder *trZipcode = tr->child; //trZipCode = 34413
					Folder *prev;
					while(trZipcode != NULL) {
						if(strcmp(trZipcode->name, k->code) == 0) {
							strcpy(trZipcode->info, "\ncode: ");
							strcat(trZipcode->info, k->code);
							strcat(trZipcode->info, "\nneighborhood: ");
							strcat(trZipcode->info, k->neighborhood);
							strcat(trZipcode->info, "\ncity: ");
							strcat(trZipcode->info, k->city);
							strcat(trZipcode->info, "\ndistrict: ");
							strcat(trZipcode->info, k->district);
							strcat(trZipcode->info, "\nlatitude: ");
							strcat(trZipcode->info, k->latitude);
							strcat(trZipcode->info, "\nlongitude: ");
							strcat(trZipcode->info, k->longitude);
							strcat(trZipcode->info, "\n");
							isAdded = 1;
							break;
						}
						prev = trZipcode;
						trZipcode = trZipcode->next;
					}
					if(!isAdded) {
						prev->next = malloc(sizeof(Folder));
						strcpy(prev->next->name, k->code);
						strcpy(prev->next->info, "code: ");
						strcat(prev->next->info, k->code);
						strcat(prev->next->info, "\nneighborhood: ");
						strcat(prev->next->info, k->neighborhood);
						strcat(prev->next->info, "\ncity: ");
						strcat(prev->next->info, k->city);
						strcat(prev->next->info, "\ndistrict: ");
						strcat(prev->next->info, k->district);
						strcat(prev->next->info, "\nlatitude: ");
						strcat(prev->next->info, k->latitude);
						strcat(prev->next->info, "\nlongitude: ");
						strcat(prev->next->info, k->longitude);
						strcat(prev->next->info, "\n");
						isAdded = 1;
					}
				}
				previousCode = tr;
				tr = tr->next;
			} // endl for while
			if (!isAdded) {
				previousCode->next = malloc(sizeof(Folder));
				strncpy(previousCode->next->name, k->code, 2);
				previousCode->next->child = malloc(sizeof(Folder));
				strcpy(previousCode->next->child->name, k->code);
				strcpy(previousCode->next->child->info, "code: ");
				strcat(previousCode->next->child->info, k->code);
				strcat(previousCode->next->child->info, "\nneighborhood: ");
				strcat(previousCode->next->child->info, k->neighborhood);
				strcat(previousCode->next->child->info, "\ncity: ");
				strcat(previousCode->next->child->info, k->city);
				strcat(previousCode->next->child->info, "\ndistrict: ");
				strcat(previousCode->next->child->info, k->district);
				strcat(previousCode->next->child->info, "\nlatitude: ");
				strcat(previousCode->next->child->info, k->latitude);
				strcat(previousCode->next->child->info, "\nlongitude: ");
				strcat(previousCode->next->child->info, k->longitude);
				strcat(previousCode->next->child->info, "\n");
				isAdded = 1;
			}
		} // end for else
    }
    free(k);
    fclose(file);
}

int find_the_nth_occurance(char* s, int n) {
	int i = 0;
	while(n > 0) {
		if(s[i] == '/') {
			n--;
		}
		i++;
	}
	return i;
}

static int postal_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;
    
    int i, count = 0;
	for(i = 0; i < strlen(path); i++) {
		if(path[i] == '/') {
			count++;
		}
	}

    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if (strncmp(path, "/NAMES", 6) == 0) {
		if(count == 1) {	
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		}
		else if(count == 2) {
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		}
		else if(count == 3) {
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		}
		else if(count == 4) {
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_size = 400;
		}
	}
	else if (strncmp(path, "/CODES", 6) == 0) {
		if(count == 1) {	
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		}
		else if(count == 2) {
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		}
		else if(count == 3) {
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_size = 400;
		}
	}
    else
        res = -ENOENT;

    return res;
}

static int postal_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;
	
	int i, count = 0;
	for(i = 0; i < strlen(path); i++) {
		if(path[i] == '/') {
			count++;
		}
	}

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

    if (strcmp(path, "/") == 0) {
		filler(buf, "NAMES", NULL, 0);
		filler(buf, "CODES", NULL, 0);
	}

	else if (strncmp(path, "/NAMES", 6) == 0) {
		if(count == 1) {	
			Folder *traverseCity = NAMES;
			while(traverseCity != NULL) {
				filler(buf, traverseCity->name, NULL, 0);
				traverseCity = traverseCity->next;
			}
		}
		else if(count == 2) {
			Folder *traverseCity = NAMES;
			int start = find_the_nth_occurance(path, 2);
			int end = find_the_nth_occurance(path, 3);
			strncpy(cityName, path + start, end - start);
			while(strcmp(cityName, traverseCity->name) != 0){
				traverseCity = traverseCity->next;
			}
			Folder *traverseDistrict = traverseCity->child;
			while(traverseDistrict != NULL) {
				filler(buf, traverseDistrict->name, NULL, 0);
				traverseDistrict = traverseDistrict->next;
			}
		} else if(count == 3) {
			Folder *traverseCity = NAMES;
			int start = find_the_nth_occurance(path, 2);
			int end = find_the_nth_occurance(path, 3);
			strncpy(cityName, path + start, end - start - 1);
			while(strcmp(cityName, traverseCity->name) != 0){
				traverseCity = traverseCity->next;
			}
			Folder *traverseDistrict = traverseCity->child;
			start = find_the_nth_occurance(path, 3);
			end = find_the_nth_occurance(path, 4);
			strncpy(cityName, path + start, end - start);
			while(strcmp(cityName, traverseDistrict->name) != 0){
				traverseDistrict = traverseDistrict->next;
			}
			Folder *traverseHood = traverseDistrict->child;
			while(traverseHood != NULL) {
				strcpy(cityName, traverseHood->name);
				strcat(cityName, ".txt");
				filler(buf, cityName, NULL, 0);
				traverseHood = traverseHood->next;
			}
		}
	}
	
	else if (strncmp(path, "/CODES", 6) == 0) {
		if(count == 1) {	
			Folder *traverseCity = CODES;
			while(traverseCity != NULL) {
				filler(buf, traverseCity->name, NULL, 0);
				traverseCity = traverseCity->next;
			}
		}
		else if(count == 2) {
			Folder *traverseCity = CODES;
			int start = find_the_nth_occurance(path, 2);
			int end = find_the_nth_occurance(path, 3);
			strncpy(cityName, path + start, end - start);
			while(strcmp(cityName, traverseCity->name) != 0){
				traverseCity = traverseCity->next;
			}
			Folder *traverseDistrict = traverseCity->child;
			while(traverseDistrict != NULL) {
				strcpy(cityName, traverseDistrict->name);
				strcat(cityName, ".txt");
				filler(buf, cityName, NULL, 0);
				traverseDistrict = traverseDistrict->next;
			}
		}
	}
    return 0;
}

static int postal_open(const char *path, struct fuse_file_info *fi)
{
	return 0;
}

static int postal_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
	size_t len;
    (void) fi;
	
	int i, count = 0;
	for(i = 0; i < strlen(path); i++) {
		if(path[i] == '/') {
			count++;
		}
	}
	
	char *info;
	
	if (strncmp(path, "/NAMES", 6) == 0) {
		if(count == 4) {
			int start = find_the_nth_occurance(path, 4);
			strcpy(cityName, path + start);
			if (strcmp(cityName, ".directory") == 0) {
				return 0;
			}
			
			Folder *traverseCity = NAMES;
			start = find_the_nth_occurance(path, 2);
			int end = find_the_nth_occurance(path, 3);
			strncpy(cityName, path + start, end - start - 1);
			cityName[end-start-1] = '\0';
			while(strcmp(cityName, traverseCity->name) != 0){
				
				traverseCity = traverseCity->next;
			}
			Folder *traverseDistrict = traverseCity->child;
			start = find_the_nth_occurance(path, 3);
			end = find_the_nth_occurance(path, 4);
			strncpy(cityName, path + start, end - start - 1);
			cityName[end-start-1] = '\0';
			while(strcmp(cityName, traverseDistrict->name) != 0){
				
				traverseDistrict = traverseDistrict->next;
			}
			
			
			Folder *traverseHood = traverseDistrict->child;
			strncpy(cityName, path + end, strlen(path+end) - 4);
			cityName[strlen(path+end) - 4] = '\0';
			while(strcmp(cityName, traverseHood->name) != 0) {
				traverseHood = traverseHood->next;
			}
			
			info = traverseHood->info;
			
			len = strlen(info);
			if (offset < len) {
				if (offset + size > len)
					size = len - offset;
				memcpy(buf, info + offset, size);
			} 
			else {
				size = 0;
			}
			return size;
		}
	}
	
	else if (strncmp(path, "/CODES", 6) == 0) {
		if(count == 3) {
			int start = find_the_nth_occurance(path, 3);
			strcpy(cityName, path + start);
			if (strcmp(cityName, ".directory") == 0) {
				return 0;
			}
			
			Folder *traverseCity = CODES;
			start = find_the_nth_occurance(path, 2);
			int end = find_the_nth_occurance(path, 3);
			strncpy(cityName, path + start, end - start - 1);
			cityName[end-start-1] = '\0';
			while(strcmp(cityName, traverseCity->name) != 0){
				traverseCity = traverseCity->next;
			}
			Folder *traverseDistrict = traverseCity->child;
			strcpy(cityName, path + end);
			while(strcmp(cityName, traverseDistrict->name) != 0) {
				traverseDistrict = traverseDistrict->next;
			}
			info = traverseDistrict->info;
			
			len = strlen(info);
			if (offset < len) {
				if (offset + size > len)
					size = len - offset;
				memcpy(buf, info + offset, size);
			} 
			else {
				size = 0;
			}
			return size;
		}
	}
	
    return 0;
}

static struct fuse_operations postal_oper = {
    .getattr	= postal_getattr,
    .readdir	= postal_readdir,
    .open	= postal_open,
    .read	= postal_read,
};

int main(int argc, char *argv[])
{
	cityName = malloc(NAME_LENGTH);

	readFromFile("postal-codes.csv");

    return fuse_main(argc, argv, &postal_oper, NULL);
}
