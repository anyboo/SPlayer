#pragma once

#ifdef VERIFIER_LIB
#	define VERIFIER_API
#else
#	if defined(VERIFIER_EXPORTS)
#		if defined(_MSC_VER)
#			define VERIFIER_API __declspec(dllexport)
#		else
#			define VERIFIER_API
#		endif
#	else
#		if defined(_MSC_VER)
#			define VERIFIER_API __declspec(dllimport)
#		else
#			define VERIFIER_API
#		endif
#	endif
#endif // VERIFIER_LIB

extern "C"{
	bool VERIFIER_API IsPresent();
	bool VERIFIER_API Verifiy(unsigned long PID);
}


