#include "format.h"
#include <stdio.h>
#include <string>

using namespace std;

int main() {
	string cmd = "ok commands 2 set noise <float>; set mode <mode>;";
	string cp = cmd;
	string tmp;
	
	printf("Popping words:\n");
	while ((tmp = popword(cp)) != "") {
		printf("popped: '%s'\n", tmp.c_str());
	}
	cp = cmd;

	printf("Popping groups:\n");
	printf("src: '%s'\n", cp.c_str());
	tmp = popword(cp);
	printf("popped: '%s'\n", tmp.c_str());
	
	printf("src: '%s'\n", cp.c_str());
	tmp = popword(cp);
	printf("popped: '%s'\n", tmp.c_str());

	printf("src: '%s'\n", cp.c_str());
	tmp = popword(cp);
	printf("popped: '%s'\n", tmp.c_str());
	
	printf("src: '%s'\n", cp.c_str());
	tmp = popword(cp, ";");
	printf("popped: '%s'\n", tmp.c_str());

	printf("src: '%s'\n", cp.c_str());
	tmp = popword(cp, ";");
	printf("popped: '%s'\n", tmp.c_str());	


	return 0;
}