#ifndef andiDBClient_h /* Include guard */
#define andiDBClient_h

int c_get_index(const char *table, const char *valueText);
float c_pull(const char *table, int index);
int c_push(const char *table, int index, float value);

int connect_sock();
int is_connected();
void split_str(const char *str, char *buffer, char *token);
int countChar(char *s, char letter);


#endif