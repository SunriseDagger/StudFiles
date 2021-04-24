/*
1. Процесс открывает N файлов, реально существующих на диске, либо 
вновь созданных. Разработать программу, демонстрирующую динамику 
формирования таблицы описателей файлов и изменения информации в ее 
элементах (при изменении информации в файлах).
*/

#include <limits.h>
#define FNAME_MAX (NAME_MAX+1)
// create three files
const char names[][FNAME_MAX] = {
	"file1.dat",
	"file2.inf",
	"file3.txt"
};

// there's nothing to be modified in code below

#include <stdio.h>
#include <sys/stat.h>

#include <sys/sendfile.h>
#include <unistd.h>

#pragma region system(pause) Unix-implementation
#ifndef WIN32
#include <termios.h>
void systempause() {
	struct termios info, info_prev;
	tcgetattr(0, &info);          /* get current terminal attirbutes; 0 is the file descriptor for stdin */
	//memcpy((void*)&info_prev, (void*)&info, (size_t)sizeof(struct termios));
	tcgetattr(0, &info_prev);
	info.c_lflag &= ~ICANON;      /* disable canonical mode */
	info.c_cc[VMIN] = 1;          /* wait until at least one keystroke available */
	info.c_cc[VTIME] = 0;         /* no timeout */
	tcsetattr(0, TCSANOW, &info); /* set immediately */
	printf("Press any key to continue...\n");
	getchar();
	// TODO: ignore left chars in buffer
	tcsetattr(0, TCSANOW, &info_prev); /* set buffered */
}
#else
#include <stdlib.h>
void systempause() {
	system("pause");
}
#endif
#pragma endregion

// TODO: avoid using of C++/Clang
#include <string>
#include <vector>
using namespace std;

#pragma region "Typedefs"
struct file_tr;
struct file_d_tr;

/*
	Структура таблицы файлов в программах лабораторной работы:
	filename, number inode, locate point (смещение?), ссылка на таблицу описателей файлов (opt)

	Структура таблицы открытых файлов в программах должна иметь вид: 
	filename, дескриптор файла, ссылка на таблицу файлов (opt) 
*/

struct file_tr {
	string filename;
	int inode;
	int locate;
	file_d_tr *file_d;
	file_tr(string filename) {
		this->filename = filename;
	};
};

struct file_d_tr {
	string filename;
	FILE *stream;
	int fd;
	int inode;
	int locate;
	int size;
	file_tr *file;
	file_d_tr(string filename) {
		this->filename = filename;
		stream = NULL;
	};
};
#pragma endregion

extern const unsigned int FILE_COUNT = sizeof(names) / FNAME_MAX;

/* Updates existing table of disk files. */
void update_table_f(vector<file_tr> &table) {
	struct stat buf;
	for (int i = 0; i < table.size(); i++) {
		int ret = stat(table[i].filename.c_str(), &buf);
		table[i].inode = buf.st_ino;
		table[i].locate = -1; // what is 'locate point'?
	}
}

/* Updates existing table of currently opened files. */
void update_table_d(vector<file_d_tr> &table) {
	struct stat buf;
	for (int i = 0; i < table.size(); i++) {
		if (table[i].stream != NULL) {
			int ret = fstat(table[i].fd, &buf);
			table[i].inode = buf.st_ino;
			//table[i].locate = table[i].stream->_IO_read_ptr - table[i].stream->_IO_read_base; // what is 'locate point'?
			table[i].locate = ftell(table[i].stream);
			table[i].fd = fileno(table[i].stream);
			table[i].size = buf.st_size;
		}
	}
}

/* Prints table of existing files on filesystem. */
void print_table_f(vector<file_tr> table, FILE *f = stdout) {
	fprintf(f, "File name\tinode\tlocate\t\n");
	for (int i = 0; i < table.size(); i++) {
		fprintf(f, "%s\t%d\t%d\t\n", table[i].filename.c_str(), table[i].inode, table[i].locate);
	}
}

/* Prints table of currently opened files. */
void print_table_d(vector<file_d_tr> table, FILE *f = stdout) {
	fprintf(f, "File name\tDescriptor\tinode\tlocate\tsize\t\n");
	for (int i = 0; i < table.size(); i++) {
		if (table[i].stream != NULL) {
			fprintf(f, "%s\t%d\t\t%d\t%d\t%d\n", table[i].filename.c_str(), table[i].fd, table[i].inode, table[i].locate, table[i].size);
		}
	}
}

int main(int argc, char **argv) {
/*
	Сценарий программы может быть следующим: 
		- открытие первого пользовательского файла; 
		- открытие второго пользовательского файла; 
		- открытие третьего пользовательского файла; 
		- изменение размера третьего файла до нулевой длины; 
		- копирование второго файла в третий файл. 
*/
	vector<file_tr> table_f; // таблица файлов
	vector<file_d_tr> table_d; // таблица дескрипторов
	
	FILE *f;
	for (int i = 0; i < FILE_COUNT; i++) {
		fclose(fopen(names[i], "w"));
		//table_f.push_back(*(new struct file_tr(names[i])));
		table_d.push_back(*(new struct file_d_tr(names[i])));
	}
	
	// build open files table
	print_table_d(table_d);
	systempause();
	
	printf("Opening 1st file.\n");
	table_d[0].stream = fopen(table_d[0].filename.c_str(), "r");
	update_table_d(table_d);
	print_table_d(table_d);
	systempause();
	
	printf("Opening 2nd file.\n");
	table_d[1].stream = fopen(table_d[1].filename.c_str(), "r");
	update_table_d(table_d);
	print_table_d(table_d);
	systempause();
	
	printf("Opening 3rd file.\n");
	table_d[2].stream = fopen(table_d[2].filename.c_str(), "w");
	update_table_d(table_d);
	print_table_d(table_d);
	systempause();
	
	printf("Truncating 3rd file.\n");
	ftruncate(table_d[2].fd, 0);
	update_table_d(table_d);
	print_table_d(table_d);
	systempause();
	
	printf("Copying content of 2nd file to 3rd file.\n");
	//struct stat buf;
	off_t offset = 0;
	//size_t size = fstat(table_d[1].fd, &buf);
	sendfile(table_d[2].fd, table_d[1].fd, &offset, table_d[1].size);
	update_table_d(table_d);
	print_table_d(table_d);
	systempause();
	
	printf("=== END OF SCENARIO ===\n");
	systempause();
	return 0;
}