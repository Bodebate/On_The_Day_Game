#include <Python.h>;
#include <iostream>;
#include <string>;

class PythonEnvironment
{

public:

	PythonEnvironment();
	
	std::string ExecutePythonScript(const std::string PythonScript) {
		// Use __main__ module's globals so executed code can import/use sys
		PyObject* main_mod = PyImport_AddModule("__main__"); 
		PyObject* globals = PyModule_GetDict(main_mod);      
		std::string output_string;

		// Redirect Python stdout to an in-memory StringIO
		const char* redirect_code =
			"import io, sys\n"
			"old_stdout = sys.stdout\n"
			"sys.stdout = io.StringIO()\n";	
		PyRun_String(redirect_code, Py_file_input, globals, globals);

		// Run the user's script (multiline allowed). Replace with desired code.
		const char* user_script = PythonScript.c_str();
		PyRun_String(user_script, Py_file_input, globals, globals);

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
			output_string = "Error running script!!!";
		}

		// Restore original stdout
		PyRun_String("sys.stdout = old_stdout\n", Py_file_input, globals, globals);

		Py_FinalizeEx();

		return output_string;
	}
};

