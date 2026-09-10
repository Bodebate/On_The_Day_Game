#include <Python.h>

#include <iostream>
#include <string>

class PythonEnvironment
{
private:
	// (Optional) store thread state if you use Save/RestoreThread patterns later
	// PyThreadState* mainThreadState = nullptr;

public:

	PythonEnvironment() {
		if (!Py_IsInitialized()) {
			Py_Initialize();
		}
	}

	~PythonEnvironment() {
		if (Py_IsInitialized()) {
			Py_FinalizeEx();
		}
	}

	std::string ExecutePythonScript(const std::string PythonScript) {
		std::string output_string;

		if (!Py_IsInitialized()) {
			return "Python interpreter not initialized";
		}

		// Ensure GIL for this thread (safe even if called from main thread)
		PyGILState_STATE gstate = PyGILState_Ensure();

		// Use __main__ module's globals so executed code can import/use sys
		PyObject* main_mod = PyImport_AddModule("__main__");
		if (!main_mod) {
			PyGILState_Release(gstate);
			return "Failed to import __main__";
		}

		PyObject* globals = PyModule_GetDict(main_mod);
		if (!globals) {
			PyGILState_Release(gstate);
			return "Failed to get __main__ globals";
		}

		// Redirect Python stdout to an in-memory StringIO
		const char* redirect_code =
			"import io, sys\n"
			"old_stdout = sys.stdout\n"
			"sys.stdout = io.StringIO()\n";
		PyRun_String(redirect_code, Py_file_input, globals, globals);

		// Run the user's script (multiline allowed). Replace with desired code.
		const char* user_script = PythonScript.c_str();
		PyObject* run_result = PyRun_String(user_script, Py_file_input, globals, globals);

		// If user script raised an exception, capture traceback text
		if (PyErr_Occurred()) {
			// Robustly fetch the exception and format it using the traceback module
			PyObject *ptype = NULL, *pvalue = NULL, *ptraceback = NULL;
			PyErr_Fetch(&ptype, &pvalue, &ptraceback);
			PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);

			PyObject* traceback_mod = PyImport_ImportModule("traceback");
			if (traceback_mod && ptype && pvalue) {
				PyObject* format_exc = PyObject_GetAttrString(traceback_mod, "format_exception");
				if (format_exc && PyCallable_Check(format_exc)) {
					PyObject* exc_list = PyObject_CallFunctionObjArgs(format_exc, ptype, pvalue, ptraceback ? ptraceback : Py_None, NULL);
					if (exc_list) {
						// Join list into single string
						PyObject* sep = PyUnicode_FromString("");
						PyObject* joined = PyUnicode_Join(sep, exc_list);
						Py_DECREF(sep);
						if (joined) {
							const char* tb_cstr = PyUnicode_AsUTF8(joined);
							if (tb_cstr) output_string = std::string(tb_cstr);
							else output_string = "Python error occurred but could not convert traceback to UTF-8";
							Py_DECREF(joined);
						}
						else {
							output_string = "Failed to join traceback list";
						}
						Py_DECREF(exc_list);
					}
					else {
						output_string = "traceback.format_exception call failed";
					}
					Py_DECREF(format_exc);
				}
				else {
					output_string = "traceback.format_exception not available";
				}
				Py_DECREF(traceback_mod);
			}
			else {
				output_string = "Failed to import traceback module";
			}

			// Cleanup exception objects
			Py_XDECREF(ptype);
			Py_XDECREF(pvalue);
			Py_XDECREF(ptraceback);

			// Clear any remaining error indicator
			PyErr_Clear();

			if (run_result) Py_DECREF(run_result);
		}
		else {
			// Read captured stdout
			PyObject* out_obj = PyRun_String("sys.stdout.getvalue()", Py_eval_input, globals, globals);
			if (out_obj) {
				const char* captured = PyUnicode_AsUTF8(out_obj);
				if (captured) {
					output_string = std::string(captured);
				}
				Py_DECREF(out_obj);
			}
			else {
				output_string = "Error running script: could not retrieve stdout";
			}
			if (run_result) Py_DECREF(run_result);
		}

		// Restore original stdout
		PyRun_String("sys.stdout = old_stdout\n", Py_file_input, globals, globals);

		PyGILState_Release(gstate);

		return output_string;
	}
};

