/*
 ============================================================================
 Name        : id3edit.c
 Author		 : ERL67 Eric Laslo
 Compile     : gcc --std=c99 -Wall -Werror -o id3edit id3edit.c
 Test	     : ./id3edit ../mp3/test1.mp3
 Test	     : ./id3edit ../mp3/test1.mp3 -title "Test Title" -year 2001
 Coloring	 : http://www.linuxforums.org/forum/linux-programming-scripting/88-color-console.html
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

char str_input[100];
int confirm;

typedef struct Fields {
	char tag_id[3];
	char title[30];
	char artist[30];
	char album[30];
	char year[4];
	char comment[28];
	char zeroB[1];
	char track[1];
	char genre[1];
} Fields;

void get_line(char* input, int size);
void check_sizes(Fields);
FILE* checked_fopen(const char* name, const char* mode);
void print_tag(Fields* tag);
Fields verify_tag(Fields tag, const char* name);	//name not used
//void confirm_write(const char* name, Fields* tag); //only works in main
void quit();

int main(int argc, char** argv) {

	Fields id3tag;

	//	for (int i = 0; i < argc; i++) {
	//		int len = strlen(argv[i]);
	//		printf("argc[%d/%d]: %s (%d)\n", i + 1, argc, argv[i], len);
	//	}

	if (argc == 1) {
		printf("\n\n\033[22;36mUseage Instructions:\n");
		printf("\t./id3edit \tDisplay Help and Struct\n");
		printf("\t./id3edit FILENAME -field value\t\tUpdate Tags\n");
		printf(
				"\t flags: -title -artist -album -year -comment -track\033[0m\n\n");

		printf("\nStruct:");
		check_sizes(id3tag);

		quit();
	}

	if (argc > 1) {
		printf("opening file: %s\n", argv[1]);
		FILE* f = checked_fopen(argv[1], "rb+");

		fseek(f, 0, SEEK_END);
		fseek(f, -128, SEEK_CUR);
		fread(&id3tag, sizeof(struct Fields), 1, f);
		fclose(f);

		id3tag = verify_tag(id3tag, argv[1]);

		print_tag(&id3tag);

		for (int i = 0; i < argc; i++) {

			if (strcmp(argv[i], "-title") == 0) {
				memset(id3tag.title, 0, sizeof(id3tag.title));
				strncpy(id3tag.title, argv[i + 1], sizeof(id3tag.title));
			} else if (strcmp(argv[i], "-artist") == 0) {
				memset(id3tag.artist, 0, sizeof(id3tag.artist));
				strncpy(id3tag.artist, argv[i + 1], sizeof(id3tag.artist));
			} else if (strcmp(argv[i], "-album") == 0) {
				memset(id3tag.album, 0, sizeof(id3tag.album));
				strncpy(id3tag.album, argv[i + 1], sizeof(id3tag.album));
			} else if (strcmp(argv[i], "-year") == 0) {
				memset(id3tag.year, 0, sizeof(id3tag.year));
				strncpy(id3tag.year, argv[i + 1], sizeof(id3tag.year));
			} else if (strcmp(argv[i], "-comment") == 0) {
				memset(id3tag.comment, 0, sizeof(id3tag.comment));
				strncpy(id3tag.comment, argv[i + 1], sizeof(id3tag.comment));
			} else if (strcmp(argv[i], "-track") == 0) {
				memset(id3tag.track, 0, sizeof(id3tag.track));
				strncpy(id3tag.track, argv[i + 1], sizeof(id3tag.track));
			}

		}

		if (argc > 2) {
			printf("\n\033[22;32mMODIFIED TAG:\033[0m\n");
			print_tag(&id3tag);

			//		confirm_write(argv[1], &id3tag); //code below not working in function

			while (confirm != EOF && (confirm != 121 || confirm != 110)) {
				printf("\nConfirm ID3 Overwrite? (y/n) ");
				confirm = getchar();
				//			printf("\ngetchar=%d\n",confirm);

				switch (confirm) {
				case 'Y':
				case 'y':
					printf("\nUpdating Tag\n");

					FILE* f = checked_fopen(argv[1], "rb+");

					fseek(f, 0, SEEK_END);
					fseek(f, -128, SEEK_CUR);
					fwrite(&id3tag, sizeof(id3tag), 1, f);
					fclose(f);

					quit();
					break;
				case 'N':
				case 'n':
					quit();
					break;
				default:
					printf("\033[01;31m \t y/n Try Again.\033[0m\n");
					break;
				}
			}
		}
	}

	return 0;
}

void get_line(char* input, int size) {
	fgets(input, size, stdin);
	int len = strlen(input);
	input[len - 1] = '\0';
}

void check_sizes(Fields tag) {
	printf("\n\033[01;37msizeof(id3tag): %lu \n", sizeof(tag));
	printf("sizeof(tag_id): %lu  offset: %lu\n", sizeof(tag.tag_id), offsetof(struct Fields, tag_id));
	printf("sizeof(title): %lu  offset: %lu\n", sizeof(tag.title), offsetof(struct Fields, title));
	printf("sizeof(artist): %lu  offset: %lu\n", sizeof(tag.artist), offsetof(struct Fields, artist));
	printf("sizeof(album): %lu  offset: %lu\n", sizeof(tag.album), offsetof(struct Fields, album));
	printf("sizeof(year): %lu  offset: %lu\n", sizeof(tag.year), offsetof(struct Fields, year));
	printf("sizeof(comment): %lu  offset: %lu\n", sizeof(tag.comment), offsetof(struct Fields, comment));
	printf("sizeof(zeroB): %lu  offset: %lu\n", sizeof(tag.zeroB), offsetof(struct Fields, zeroB));
	printf("sizeof(track): %lu  offset: %lu\n", sizeof(tag.track), offsetof(struct Fields, track));
	printf("sizeof(genre): %lu  offset: %lu\033[0m\n\n", sizeof(tag.genre), offsetof(struct Fields, genre));
}

void print_tag(Fields* tag) {
	printf("tag_id: %.*s\n", 3, tag->tag_id);
	printf("title: %.*s\n", 30, tag->title);
	printf("artist: %.*s\n", 30, tag->artist);
	printf("album: %.*s\n", 30, tag->album);
	printf("year: %.*s\n", 4, tag->year);
	printf("comment: %.*s\n", 28, tag->comment);
	printf("zeroB: %.*s\n", 1, tag->zeroB);
	printf("track: %.*s\n", 1, tag->track);
	printf("genre: %.*s\n\n", 1, tag->genre);
}

Fields verify_tag(Fields tag, const char* name) {

	if (tag.tag_id[0] == 84 && tag.tag_id[1] == 65 && tag.tag_id[2] == 71) {
		printf("\n\033[22;32mTAG FOUND:\033[0m\n");

		memset(tag.tag_id, 0, sizeof(tag.tag_id));

		memset(&tag.tag_id[0], 84, 1); //T A G fix null terminators
		memset(&tag.tag_id[1], 65, 1);
		memset(&tag.tag_id[2], 71, 1);
	} else if (strcmp(tag.tag_id, "TAG") != 0) {
		printf("\n\033[01;33mNO TAG FOUND. USE ARGS TO CREATE.\033[0m\n");
		//		quit();

		memset(tag.tag_id, 0, sizeof(tag.tag_id));
		memset(&tag.tag_id[0], 84, 1); //T A G
		memset(&tag.tag_id[1], 65, 1);
		memset(&tag.tag_id[2], 71, 1);
		memset(tag.title, 0, sizeof(tag.title));
		memset(tag.artist, 0, sizeof(tag.artist));
		memset(tag.album, 0, sizeof(tag.album));
		memset(tag.year, 0, sizeof(tag.year));
		memset(tag.comment, 0, sizeof(tag.comment));
		memset(tag.zeroB, 0, sizeof(tag.zeroB));
		memset(tag.track, 0, sizeof(tag.track));
		memset(tag.genre, 0, sizeof(tag.genre));

		//confirm_write(name, &tag); //not working as function
	}

	return tag;
}

FILE* checked_fopen(const char* name, const char* mode) {
	FILE* f = fopen(name, mode);

	if (f == NULL) {
		fprintf(stderr, "404 File Not Found '%s'\n", name);
		exit(1);
	}

	return f;
}

/* this function isn't working maybe and shouldn't be used
 void confirm_write(const char* name, Fields* tag) {
 int confirm;

 while (confirm != EOF && (confirm != 121 || confirm != 110)) {
 printf("\nConfirm ID3 Overwrite? (y/n) ");
 confirm = getchar();

 switch (confirm) {
 case 'Y':
 case 'y':
 printf("\nUpdating Tag\n");
 FILE* f = checked_fopen(name, "rb+");
 fseek(f, 0, SEEK_END);
 fseek(f, -128, SEEK_CUR);
 fwrite(tag, sizeof(tag), 1, f);
 fclose(f);
 quit();
 break;
 case 'N':
 case 'n':
 quit();
 break;
 default:
 printf("\033[01;31m \t y/n Try Again.\033[0m\n");
 break;
 }
 }
 }
 */

void quit() {
	printf("\n\033[22;35mGoodbye.\033[0m\n\n");
	exit(1);
}
