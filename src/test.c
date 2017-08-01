#include <stdio.h>
#include <unistd.h>
#include <linux/types.h>
#include <stdlib.h>
#include "format.h"
#include "log.h"
#include "alloc.h"
#include "parser.h"



int main()
{
	mac_t m;

	printf("sizeof %ld\n", sizeof(m));

	echo.i("kakakak");
	echo.e("kakakak");
	echo.d("kakakak");
//	echo.f("kakakak");

	printf("-------------\n");

	int *pi = alloc_sizeof(int);
	echo.i("%p", pi);
	free(pi);

	init_config_module();

	echo.i("load config");

	Config_t *config = load_config("xml", "test.xml");
	echo.i("%p", config);

	free_config(config);
	free_config_modules(NULL);
}
