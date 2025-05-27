#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_create_initd (const char *file_name);
pid_t process_fork (const char *name, struct intr_frame *if_);
int process_exec (void *f_name);
int process_wait (pid_t);
void process_exit (void);
void process_activate (struct thread *next);
void argument_stack(char **argv, int argc, void **rsp);
void process_off();
int process_add_file (struct file *f);
struct file *process_get_file(int fd);
struct thread *process_find_child (int pid);

#endif /* userprog/process.h */
