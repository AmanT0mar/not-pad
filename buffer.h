#ifndef BUFFER_H_
#define BUFFER_H_
#include <stdlib.h>

typedef struct {
	size_t capacity;
	size_t size;
	char *chars;
} Line;

void lineInsert(Line *line, const char *text, size_t *col);
void lineBackspace(Line *line, size_t *col);
void lineDelete(Line *line, size_t *col);

typedef struct {
	size_t capacity;
	size_t size;
	Line *lines;
	size_t cursor_row;
	size_t cursor_col;

} Editor;

void editorSaveToFile(const Editor *editor, const char *file_path);
void editorLoadFromFile(Editor *editor, const char *file_path);
void editorInsert(Editor *editor, const char *text);
void editorInsertNewLine(Editor *editor);
void editorBackspace(Editor *editor);
void editorDelete(Editor *editor);
const char *editorCharUnderCursor(const Editor *editor);

#endif //BUFFER_H_
