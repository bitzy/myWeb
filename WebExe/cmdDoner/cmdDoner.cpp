#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string>
#include <exception>
#include <iostream>

using namespace std;
int main(int argc, char** argv) 
{
	string get("");
	for(int i = 1; i < argc; i++) {
		string temp(argv[i]);
		get += " ";
		get += temp;
	}

	string cmd = get;
	setuid(0);
	cout << cmd << endl;
	system(cmd.c_str());
}
