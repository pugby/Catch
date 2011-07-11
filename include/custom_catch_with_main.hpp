#include "catch_runner.hpp"
#include "catch.hpp"
#include <cassert>

#ifdef _MSC_VER
	#include <crtdbg.h>

	int TisDebugHook( int reportType, char *userMessage, int *retVal)
	{
		// Define this if you want additional functionality for asserts, warnings, and errors

		return 0;
	}

	#ifdef assert
		#undef assert
	#endif
	#define assert(expr) ((!!(expr)) || printf("%s(%d) : Assertion (%s) failed!\n",__FILE__,__LINE__,#expr))
	
	#ifdef TISASSERT
		#undef TISASSERT
	#endif
#define TISASSERT(expr) ((!!(expr)) || printf("%s(%d) : Assertion (%s) failed!\n",__FILE__,__LINE__,#expr))
#else
	#ifdef assert
		#undef assert
	#endif
	#define assert(expr) ((!!(expr)) || printf("%s(%d) : Assertion (%s) failed!\n",__FILE__,__LINE__,#expr))
	
	#ifdef TISASSERT
		#undef TISASSERT
	#endif
#define TISASSERT(expr) ((!!(expr)) || printf("%s(%d) : Assertion (%s) failed!\n",__FILE__,__LINE__,#expr))
#endif

int main (int argc, char * const argv[])
{
	#ifdef __OBJC__

		NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
		Catch::registerTestMethods();
    
		int result = Catch::Main( argc, (char* const*)argv );
    
		[pool drain];
		return result;

	#else

		// Additional setup code goes here
	
		#ifdef _MSC_VER
			// Initialize Debug routines
			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
			_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
			_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
			_CrtSetReportHook( TisDebugHook );
		#endif

		int retVal = Catch::Main( argc, argv );

		// Additional Shutdown code goes here

		return retVal;

	#endif
}