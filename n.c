#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "n"
#define MAX_STRING 512

struct notes* initialize_notes(char value[]);
struct notes* create(char value[]);
void print_note(char *note, int n);
void print_notes();
void error(char *message);
int delete(int index);
void read_notes();
void save_notes();
void file_name(char *buffer);

struct notes {
  char value[MAX_STRING];
  struct notes *next;
};

struct notes *head = NULL;
struct notes *current = NULL;

char name[MAX_STRING];

int main(int argc, char* argv[]) {
  struct notes *pointer = NULL;

  read_notes();

  strlcpy(name, argv[0], sizeof(name));

  if (argv[1] != NULL) {
    if (strcmp(argv[1], "-o") == 0) {
      printf("open URL of specifed note [-o 1]\n");
      return 0;
    } else if (strcmp(argv[1], "-d") == 0) {
      int retval = delete(atoi(argv[2]));
      save_notes();
      return retval;
    }

    /* add all args together */
    char buffer[MAX_STRING];
    int i = 1;
    while (i < argc) {
      strlcat(buffer, argv[i], sizeof(buffer));
      if (++i < argc)
        strlcat(buffer, " ", sizeof(buffer));
    }
    create(buffer);
  }

  if (argc == 1)
    print_notes();

  save_notes();

  return 0;
}

void error(char *message) {
  fprintf(stderr, "%s: %s\n", name, message);
}

struct notes* initialize_notes(char value[]) {
  struct notes *pointer = (struct notes*)malloc(sizeof(struct notes));

  if (NULL == pointer) {
    /* node creation failed */
    return NULL;
  }

  strlcpy(pointer->value, value, sizeof(pointer->value));
  pointer->next = NULL;

  head = current = pointer;
  return pointer;
}

int delete(int index) {
  /* minus 1 here to account for 0-index linked list */
  index--;

  struct notes *prev = NULL;
  struct notes *delete = NULL;
  struct notes *pointer = head;

  /* loop over notes to find the note matching the `index' */
  int i = 0;
  while (pointer != NULL) {
    if (i == index) {
      delete = pointer;
      break;
    }
    prev = pointer;
    pointer = pointer->next;
    i++;
  }

  if (delete == NULL) {
    char error_message[MAX_STRING];
    snprintf(error_message, sizeof(error_message), "%d: no such note", index + 1);
    error(error_message);

    return -1;
  } else {
    if (prev != NULL)
      prev->next = delete->next;

    if (delete == current)
      current = prev;

    if (delete == head)
      head = delete->next;
  }

  free(delete);
  delete = NULL;

  return 0;
}

struct notes* create(char value[]) {
  if (NULL == head) {
    return (initialize_notes(value));
  }

  struct notes *pointer = (struct notes*)malloc(sizeof(struct notes));
  if (NULL == pointer) {
    /* node creation failed */
    return NULL;
  }
  strlcpy(pointer->value, value, sizeof(pointer->value));
  pointer->next = NULL;

  current->next = pointer;
  current = pointer;

  return pointer;
}

void print_note(char *note, int n) {
  printf("- %d. %s\n", n, note);
}

void print_notes() {
  struct notes *ptr = head;
  int i = 0;
  while (ptr != NULL) {
    print_note(ptr->value, ++i);
    ptr = ptr->next;
  }
}

void read_notes() {
  char fn[MAX_STRING];
  file_name(fn);
  FILE *file = fopen(fn, "r");

  if (file == NULL)
    return;

  char buffer[MAX_STRING] = "";

  char c;
  while ((c = getc(file)) != EOF) {
    if (c == '\n') {
      create(buffer);
      buffer[0] = '\0';
    } else {
      strncat(buffer, &c, 1);
    }
  }
}

/* TODO: don't create a file if there are no notes to save */
void save_notes() {
  char fn[MAX_STRING];
  file_name(fn);
  FILE *file = fopen(fn, "w");

  struct notes *ptr = head;
  while (ptr != NULL) {
    int i = 0;
    while (i < strlen(ptr->value)) {
      putc(ptr->value[i], file);
      i++;
    }
    putc('\n', file);

    ptr = ptr->next;
  }
}

void file_name(char *buffer) {
  if (getenv("N_FILE") == NULL) {
    snprintf(buffer, MAX_STRING, "%s/.%s",
      getenv("HOME"),
      FILE_NAME
    );
  } else {
    snprintf(buffer, MAX_STRING, "%s", getenv("N_FILE"));
  }
}
