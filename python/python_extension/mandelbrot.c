#include "Python.h"
#include "algo.h"

/* make this static if you don't want other code to call this function */
/* I don't make it static because want to access this via ctypes */
/* static */
int iterate_point(double x0, double y0, int max_iterations) {
	int iteration = 0;
	double x=x0, y=y0, x2=x*x, y2=y*y;
	
	while (x2+y2<4 && iteration<max_iterations) {
		y = 2*Multiplex_Export(x,y) + y0;
		x = x2-y2 + x0;
		x2 = Multiplex_Export(x,x);
		y2 = Multiplex_Export(y,y);
		iteration++;
	}
	return iteration;
}


/* The module doc string */
PyDoc_STRVAR(mandelbrot__doc__,
"Mandelbrot point evalutation kernel");

/* The function doc string */
PyDoc_STRVAR(iterate_point__doc__,
"x,y,max_iterations -> iteration count at that point, up to max_iterations");

/* The wrapper to the underlying C function */
static PyObject *
py_iterate_point(PyObject *self, PyObject *args)
{
	double x=0, y=0;
	int iterations, max_iterations=1000;
	/* "args" must have two doubles and may have an integer */
	/* If not specified, "max_iterations" remains unchanged; defaults to 1000 */
	/* The ':iterate_point' is for error messages */
	if (!PyArg_ParseTuple(args, "dd|i:iterate_point", &x, &y, &max_iterations))
		return NULL;
	/* Verify the parameters are correct */
	if (max_iterations < 0) max_iterations = 0;
	
	/* Call the C function */
	iterations = iterate_point(x, y, max_iterations);
	
	/* Convert from a C integer value to a Python integer instance */
	return PyInt_FromLong((long) iterations);
}

/* A list of all the methods defined by this module. */
/* "iterate_point" is the name seen inside of Python */
/* "py_iterate_point" is the name of the C function handling the Python call */
/* "METH_VARGS" tells Python how to call the handler */
/* The {NULL, NULL} entry indicates the end of the method definitions */
static PyMethodDef mandelbrot_methods[] = {
	{"iterate_point",  py_iterate_point, METH_VARARGS, iterate_point__doc__},
	{NULL, NULL}      /* sentinel */
};

/* When Python imports a C module named 'X' it loads the module */
/* then looks for a method named "init"+X and calls it.  Hence */
/* for the module "mandelbrot" the initialization function is */
/* "initmandelbrot".  The PyMODINIT_FUNC helps with portability */
/* across operating systems and between C and C++ compilers */
PyMODINIT_FUNC
initmandelbrot(void)
{
	/* There have been several InitModule functions over time */
	Py_InitModule3("mandelbrot", mandelbrot_methods,
                   mandelbrot__doc__);
}

