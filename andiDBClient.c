#include <Python.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//new line
#include <stdio.h>

#include "andiDBClient.h"

#define PORT 1337
//delace functions

int sock = 0;
struct sockaddr_in serv_addr;
int connect_sock(){
    if(sock){
        close(sock);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        return -1;
    }

    return 0;
}
int is_connected(){
    if(!sock){
        return 0;
    }
    int retval = send(sock, "", 0, SOCK_NONBLOCK);
    return retval == 0;
}
int countChar(char *s, char letter)
{
    int count = 0;
    for (int i = 0; i < strlen(s); i++)
    {
        if(s[i] == letter) 
            count++;
    }
    return count;
}
void split_str(const char *str, char *buffer, char* token)
{
    int i = 0;
    char *p = strtok(str, token);

    while (p != NULL)
    {
        buffer[i++] = p;
        p = strtok(NULL, token);
    }

    

    return;
}

// Push Methods these are for Setting Values in Python Interpreter and C Code 

static PyObject *push(PyObject *self, PyObject *args) {
    char* table;
    int index;
    float value;
    // Parse arguments 
    if (!PyArg_ParseTuple(args, "sif", &table, &index, &value))
    { 
        printf("wrong Parameters");
        return Py_BuildValue("i", -1);
    }
    c_push(table, index, value);
    return Py_BuildValue("i", 0);
}
int c_push(const char *table, int index, float value){
    while (!is_connected())
    {
        connect_sock();
    }
    const int buffSize = snprintf(NULL, 0, "PUSH;%s;%i;%f\n", table, index, value); // caculate the size of bufferstring
    char *msg = malloc(buffSize + 1);                                               // make dynamic
    sprintf(msg, "PUSH;%s;%i;%f\n", table, index, value);
    send(sock, msg, strlen(msg), 0);
    free(msg);
    return 0;
}
//Pull Methods for getting Values from DMS

static PyObject *pull(PyObject *self, PyObject *args)
{
    char *table;
    int index;
    // Parse arguments
    if (!PyArg_ParseTuple(args, "si", &table, &index))
    {
        printf("wrong Parameters");
        return Py_BuildValue("i", -1);
    }

    float value = c_pull(table, index);
    return Py_BuildValue("f", value);
}
float c_pull(const char *table, int index){
    while (!is_connected())
    {
        connect_sock();
    }
    const int buffSize = snprintf(NULL, 0, "PUSH;%s;%i\n", table, index); // caculate the size of bufferstring
    char *msg = malloc(buffSize + 1);                                     // make dynamic
    sprintf(msg, "PULL;%s;%i\n", table, index);
    send(sock, msg, strlen(msg), 0); // Send pull Message
    free(msg);
    //Wait for response

    char *response = malloc(128);

    int valread = read(sock, response, 128);
    int slicer_count = countChar(response, ';');

    char *chunks = strtok(response, ";");

    for (int i = 0; i < slicer_count; i++)
    {
        chunks = strtok(NULL, ";");
    }

    free(response);
    char * remainingString;
    return (float)strtod(chunks, &remainingString);
}
char *c_pull_str(const char *table, int index){
    while (!is_connected())
    {
        connect_sock();
    }
    const int buffSize = snprintf(NULL, 0, "PUSH;%s;%i\n", table, index); // caculate the size of bufferstring
    char *msg = malloc(buffSize + 1);                                     // make dynamic
    sprintf(msg, "PULL;%s;%i\n", table, index);
    send(sock, msg, strlen(msg), 0); // Send pull Message
    free(msg);
    // Wait for response

    char *response = malloc(128);

    int valread = read(sock, response, 128);
    int slicer_count = countChar(response, ';');

    char *chunks = strtok(response, ";");

    for (int i = 0; i < slicer_count; i++)
    {
        chunks = strtok(NULL, ";");
    }
    char *ptr;
    ptr = strchr(chunks, '\n');
    if (ptr != NULL)
    {
        *ptr = '\0';
    }

    free(response);
    return chunks;
}

static PyObject *get_index(PyObject *self, PyObject *args)
{
    char *table;
    char *valueText;
    // Parse arguments
    if (!PyArg_ParseTuple(args, "ss", &table, &valueText))
    {
        printf("wrong Parameters");
        return Py_BuildValue("i", -1);
    }
    int index = c_get_index(table, valueText);

    return Py_BuildValue("i", index);
}
int c_get_index(const char *table, const char *valueText){
    while (!is_connected())
    {
        connect_sock();
    }
    const int buffSize = snprintf(NULL, 0, "INDEX;%s;%s\n", table, valueText); // caculate the size of bufferstring
    char *msg = malloc(buffSize + 1);                                          // make dynamic
    sprintf(msg, "INDEX;%s;%s\n", table, valueText);
    send(sock, msg, strlen(msg), 0); // Send pull Message

    free(msg);
        // Wait for response

    char *response = malloc(128);

    int valread = read(sock, response, 128);
    int slicer_count = countChar(response, ';');

    char *chunks = strtok(response, ";");

    for (int i = 0; i < slicer_count; i++)
    {
        chunks = strtok(NULL, ";");
    }

    free(response);
    return atoi(chunks);
}

static PyMethodDef andiDBMethods[] = {
    {"push", push, METH_VARARGS, "Python interface for fputs C library function"},
    {"pull", pull, METH_VARARGS, "Python blabla"},

    {"getindex", get_index, METH_VARARGS, "Python blabla"},

    {NULL, NULL, 0, NULL}};

static struct PyModuleDef andiDBCmodule = {
    PyModuleDef_HEAD_INIT,
    "andiDBC",
    "Python interface for the fputs C library function",
    -1,
    andiDBMethods};
PyMODINIT_FUNC PyInit_andiDBC(void) {
    return PyModule_Create(&andiDBCmodule);
}
