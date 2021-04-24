#include <windows.h>

#include "translator.h"

int main(){
	translator our_trans;
	our_trans.lex_analysis("testpr.txt", "token.txt", "errors.txt");
	system("pause");

	return 0;
}