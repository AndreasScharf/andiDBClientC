#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"

#include "andiDBClient.h"


typedef struct
{
    PyObject_HEAD
    PyObject *table; /* first name */
    PyObject *name;      /* last name */
    int index;
} CustomObject;

static void
Value_dealloc(CustomObject *self)
{
    Py_XDECREF(self->table);
    Py_XDECREF(self->name);

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
Value_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{

    CustomObject *self;
    self = (CustomObject *)type->tp_alloc(type, 0);
    if (self != NULL)
    {
        self->table = PyUnicode_FromString("");
        if (self->table == NULL)
        {
            Py_DECREF(self);
            return NULL;
        }
        self->name = PyUnicode_FromString("");
        if (self->name == NULL)
        {
            Py_DECREF(self);
            return NULL;
        }

        self->index = -1;
    }
    return (PyObject *)self;
}

static int Value_init(CustomObject *self, PyObject *args, PyObject *kwds)
{

    static char *kwlist[] = {"table", "name",  NULL};
    PyObject *table = NULL, *name = NULL, *tmp;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO", kwlist, &table, &name)){
        printf("Init Error \n");
    }

    if (table)
    {
        tmp = self->table;
        Py_INCREF(table);
        self->table = table;
        Py_XDECREF(tmp);
    }
    if (name)
    {
        tmp = self->name;
        Py_INCREF(name);
        self->name = name;
        Py_XDECREF(tmp);
    }
    if(table && name){
        self->index = -2;
        self->index = c_get_index(PyUnicode_AsUTF8(table), PyUnicode_AsUTF8(name));
    }
    return 0;
}

static PyMemberDef Value_members[] = {
    {"table", T_OBJECT_EX, offsetof(CustomObject, table), 0,
     "value table"},
    {"name", T_OBJECT_EX, offsetof(CustomObject, name), 0,
     "value name"},
    {"index", T_INT, offsetof(CustomObject, index), 0,
     "value index"},
    {NULL} /* Sentinel */
};

static PyObject *
Value_get(CustomObject *self, PyObject *Py_UNUSED(ignored))
{
    if (self->table == NULL)
    {
        PyErr_SetString(PyExc_AttributeError, "table");
        return NULL;
    }
    if (self->index < 0)
    {
        PyErr_SetString(PyExc_AttributeError, "index");
        return NULL;
    }
    float value = c_pull( PyUnicode_AsUTF8(self->table) , self->index);
    return Py_BuildValue("f", value);
}

static PyObject *
Value_set(CustomObject *self, PyObject *args)
{
    float value;
    if (!PyArg_ParseTuple(args, "f",&value))
    {
        printf("wrong Parameters");
        return Py_BuildValue("i", -1);
    }
    if (self->table == NULL)
    {
        PyErr_SetString(PyExc_AttributeError, "table");
        return NULL;
    }
    if (self->index < 0)
    {
        PyErr_SetString(PyExc_AttributeError, "index");
        return NULL;
    }
   

    c_push(PyUnicode_AsUTF8(self->table), self->index, value);
    return Py_BuildValue("i", 0);
}

static PyMethodDef Value_methods[] = {
    {"get", (PyCFunction)Value_get, METH_NOARGS, "Getting Module Connected value from andiDB"},
    {"set", (PyCFunction)Value_set, METH_VARARGS, "Setting Module Connected value to andiDB"},

    {NULL} /* Sentinel */
};

static PyTypeObject ValueType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "andiDB.value",
    .tp_doc = "Andi DB Value",
    .tp_basicsize = sizeof(CustomObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = Value_new,
    .tp_init = (initproc)Value_init,
    .tp_dealloc = (destructor)Value_dealloc,
    .tp_members = Value_members,
    .tp_methods = Value_methods,
};

static PyModuleDef Valuemodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "andiDB",
    .m_doc = "Example module that creates an extension type.",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_andiDB(void)
{
    PyObject *m;
    if (PyType_Ready(&ValueType) < 0)
        return NULL;

    m = PyModule_Create(&Valuemodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&ValueType);
    if (PyModule_AddObject(m, "value", (PyObject *)&ValueType) < 0)
    {
        Py_DECREF(&ValueType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}