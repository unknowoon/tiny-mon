#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "quekka.h"

int main(void) {

	char *ip = "127.0.0.1";
	char *port = "8080";

	int quekka = qukka_init(ip, port);

	int status = quekka_connect(quekka);
	if (status == -1) {
		printf("Can't connect to quekka server\n");
	}

	char *topic = "hello";
	int rslt = publisher_greet(quekka, topic);
	if (rslt) {
		printf("Success");
	}
}
