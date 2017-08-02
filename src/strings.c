/*
 * strings.c
 *
 *  Created on: 2017. 8. 2.
 *      Author: root
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "strings.h"


char **new_splits(const char *str, char *sep, int *ntoks)
{
	char *org;
	char *rtoks;
	char *lasts;
	int n = 0;
	char **rstr = NULL;
	int i;

	*ntoks = 0;
	if (!str || !strlen(str)) return NULL;

	org = strdup(str);
	rtoks = strtok_r(org, sep, &lasts);
	do {
		n++;
	} while ((rtoks = strtok_r(NULL, sep, &lasts)) != NULL);
	free(org);

	rstr = (char **)malloc(sizeof(char **) * n);

	org = strdup(str);
	lasts = NULL;
	i = 0;
	rtoks = strtok_r(org, sep, &lasts);
	do {
		rstr[i++] = strdup(rtoks);
	} while ((rtoks = strtok_r(NULL, sep, &lasts)) != NULL);

	*ntoks = n;
	free(org);
	return rstr;
}

void free_splits(char **toks, int ntoks)
{
	if (!toks || ntoks <= 0) return;

	do {
		ntoks--;
		free(toks[ntoks]);
	} while (ntoks);

	free(toks);
}

#if 0
int main()
{
	char **toks;
	int ntoks;
	int i;
	toks = new_splits("aaa:name=kinow,age=30;bbb:module=31,", ";", &ntoks);

	printf("n = %d\n", ntoks);
	for (i=0; i<ntoks; i++) {
		printf("[%d] %s\n", i, toks[i]);

		char **t;
		int n, k;
		t = new_splits(toks[i], ",", &n);
		for (k=0; k<n; k++) {
			printf("\t%s\n", t[k]);
		}
		free_splits(t, n);
	}

	free_splits(toks, ntoks);
}
#endif
