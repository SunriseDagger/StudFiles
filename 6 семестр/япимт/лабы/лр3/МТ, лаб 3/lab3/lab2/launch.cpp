#include <windows.h>

#include "translator.h"

int main(){
	translator our_trans;
	bool lex = our_trans.lex_analysis("testpr.txt", "token.txt", "errors.txt");
	if(lex){
		our_trans.parse("token.txt", "lex errors.txt");
		our_trans.out_tree("tree.txt");
	}
	system("pause");

	return 0;
}