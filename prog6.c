#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <error.h>
#include <omp.h>
#include <strings.h>

#define WCOUNT 10
#define FILE_NAME "words.txt"
char search_words[WCOUNT][100] = {"The", "around", "from", "by", "be", "any", "which", "various", "graphics", "mount"};
long counts[WCOUNT] = {0};

int is_equal(char *a, char *key, int ic){
	if (strlen(a) != strlen(key))
		return 0;
	if (ic == 1)
		return (strcasecmp(a,key) == 0);
	else
		return (strcmp(a,key) == 0);
}

void read_word(char *tmp, FILE *fp){
	char ch;
	int i = 0;

	while ((ch = fgetc(fp)) != EOF && isalpha(ch) == 0);	//Find 1st occ of alphabet
	
	while (ch != EOF && isalpha(ch) != 0){
		tmp[i++] = ch;
		ch = fgetc(fp);
	}

	tmp[i] = '\0';
}

long determine_count(const char *filename, const char *key, int ic){
	long count = 0;
	char temp[40];
	FILE *fp = fopen(filename,"r");
	if (fp == NULL) error(1,0,"Error in file");

	while (feof(fp) == 0){
		read_word(temp,fp);
		if (is_equal(temp,key,ic))
			count++;
	}

	fclose(fp);
	return count;
}

int main(int argc, char **argv){
	int i, nth;
	if (argc != 2)
		error(1,0,"Use : a.out <no.of.threads>");
	nth = atoi(argv[1]);

	double t = omp_get_wtime();
	#pragma omp parallel for private(i) num_threads(nth)
	for (i = 0; i < WCOUNT; i++){
		counts[i] = determine_count(FILE_NAME,search_words[i],1);
	}
	t = omp_get_wtime() - t;

	for (i = 0; i < WCOUNT; i++){
		printf("\nCount of %s = %ld\n",search_words[i],counts[i]);
	}
	printf("\nTime = %lf\n",t);
	return 0;
}
