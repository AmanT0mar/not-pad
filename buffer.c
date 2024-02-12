#include <assert.h>
#include "./buffer.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define LINE_INIT_CAPACITY 1024
#define EDITOR_INIT_CAPACITY 128

static void lineGrow(Line *line, size_t n) {
	size_t new_capacity = line->capacity;
	
	assert(new_capacity >= 	line->size);
	while (new_capacity - line->size < n) {
		if (new_capacity == 0) {
			new_capacity = LINE_INIT_CAPACITY;
		}
		else {
			new_capacity *= 2;
		}
	}
	if (new_capacity != line->capacity) {
		line->chars = realloc(line->chars, new_capacity);
		line->capacity = new_capacity;
	}
}

void lineInsert(Line *line, const char *text, size_t *col) {
	if (*col > line->size) {
		*col = line->size;
	}
	
	const size_t text_size = strlen(text);
	lineGrow(line, text_size);
	
	memmove(line->chars + *col + text_size, line->chars + *col, line->size - *col);
	memcpy(line->chars + *col, text, text_size);
	line->size += text_size;	
	*col += text_size;		
}

void lineBackspace(Line *line, size_t *col) {
	if (*col > line->size) {
		*col = line->size;
	}
	
	if (*col > 0 && line->size > 0) {
		memmove(line->chars + *col - 1, line->chars + *col, line->size - *col);
		line->size -= 1;
		*col -= 1;
	}
}

void lineDelete(Line *line, size_t *col) {
	if (*col > line->size) {
		*col = line->size;
	}
	
	if (*col < line->size && line->size > 0) {
		memmove(line->chars + *col, line->chars + *col + 1, line->size - *col);
		line->size -= 1;
	}
}

static void editorGrow(Editor *editor, size_t n) {
	size_t new_capacity = editor->capacity;
	
	assert(new_capacity >= editor->size);
	while(new_capacity - editor->size < n) {
		if (new_capacity == 0) {
			new_capacity = EDITOR_INIT_CAPACITY;
		}
		else {
			new_capacity *= 2;
		}
	}
	
	if (new_capacity != editor->capacity) {
		editor->lines = realloc(editor->lines, new_capacity * sizeof(editor->lines[0]));
		editor->capacity = new_capacity;
	} 

}

void editorInsertNewLine(Editor *editor) {
	if (editor->cursor_row > editor->size) {
		editor->cursor_row = editor->size;
	}
	
	editorGrow(editor, 1);
	
	memmove(editor->lines + editor->cursor_row + 1, editor->lines + editor->cursor_row, (editor->size - editor->cursor_row) * sizeof(editor->lines[0]));
	memset(&editor->lines[editor->cursor_row + 1], 0, sizeof(editor->lines[0]));
	editor->size += 1;
	editor->cursor_row += 1;
	editor->cursor_col = 0;
}

void editorPushNewLine(Editor *editor) {
	editorGrow(editor, 1);
	memset(&editor->lines[editor->size], 0, sizeof(editor->lines[0]));
	editor->size += 1;
}

void editorInsert(Editor *editor, const char *text) {
	if (editor->cursor_row >= editor->size) {
		if (editor->size > 0) {
			editor->cursor_row = editor->size - 1;
		}
		else {
			editorPushNewLine(editor);
		}
	}	
	
	lineInsert(&editor->lines[editor->cursor_row], text, &editor->cursor_col);
}
void editorBackspace(Editor *editor) {
	if (editor->cursor_row >= editor->size) {
		if (editor->size > 0) {
			editor->cursor_row = editor->size - 1;
		}
		else {
			editorPushNewLine(editor);
		}
	}
	
	lineBackspace(&editor->lines[editor->cursor_row], &editor->cursor_col);
}
void editorDelete(Editor *editor) {
	if (editor->cursor_row >= editor->size) {
		if (editor->size > 0) {
			editor->cursor_row = editor->size - 1;
		}
		else {
			editorPushNewLine(editor);
		}
	}

	lineDelete(&editor->lines[editor->cursor_row], &editor->cursor_col);
}

const char *editorCharUnderCursor(const Editor *editor) {
	if (editor->cursor_row < editor->size) {
		if (editor->cursor_col < editor->lines[editor->cursor_row].size) {
			return &editor->lines[editor->cursor_row].chars[editor->cursor_col];
		}
	}
	return NULL;
}

void editorLoadNewLine(Editor *editor, char *text) {
	editorInsertNewLine(editor);
	editorInsert(editor, text);
}

void editorSaveToFile(const Editor *editor, const char *file_path) {
	FILE *f = fopen(file_path, "w");
	if (f == NULL) {
		printf("CANNOT OPEN FILE");	
		exit(1);
	}
	
	for (size_t row = 0; row<editor->size; ++row) {
		fwrite(editor->lines[row].chars, 1, editor->lines[row].size, f);
		fputc('\n',f);
	}
	fclose(f);
}

void editorLoadFromFile(Editor *editor, const char *file_path) {	
	FILE *f = fopen(file_path, "r");
	if (f == NULL) {
		fprintf(stderr, "ERROR: cannot load file");
		exit(1);
	}
	
	for (size_t i = 0; i < editor->size; ++i) {
		free(editor->lines[i].chars);
	}
	free(editor->lines);
	
	editor->lines = NULL;
	editor->size = 0;
	editor->capacity = 0;
	editor->cursor_row = 0;
	editor->cursor_col = 0;

	char chunk[1024];
	while (fgets(chunk, sizeof(chunk), f) != NULL) {
		editorLoadNewLine(editor, chunk);
	}
	editor->cursor_col = 0;
	fclose(f);
}





