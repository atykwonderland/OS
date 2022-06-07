void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
int mem_store_script (char *script, int size, char *lines[], int *start, int *end);
char *mem_get_index(int i);
int mem_del_range (int start, int end);