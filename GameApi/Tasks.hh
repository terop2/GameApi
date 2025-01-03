
#ifndef TASKS_HH
#define TASKS_HH

void tasks_init();
void tasks_add(int id, void *(*fptr)(void*), void *data);
void tasks_join(int id);
#endif
