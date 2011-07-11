/*
 *  catch_test_case_registry_impl.hpp
 *  Catch
 *
 *  Created by Phil on 7/1/2011
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch_test_registry.hpp"
#include "catch_test_case_info.hpp"
#include "catch_hub.h"

#include <vector>
#include <set>
#include <sstream>

#ifdef _MSC_VER
	#include "Windows.h"
	#include <signal.h>
	#include <crtdbg.h>
	#include <float.h>
#else
	#include <signal.h>
	#include <pthread.h>	// MUST LINK -lpthread
#endif

#include <iostream> // !TBD DBG
namespace Catch
{
	void SignalHandler(int sig);
	#ifdef _MSC_VER
	DWORD WINAPI RunTestInThread(LPVOID lpParam);
	#else
	void * RunTestInThread(void *lpParam);
	#endif

	class TestRegistry : public ITestCaseRegistry
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        TestRegistry
        ()
        : m_unnamedCount( 0 )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void registerTest
        (
            const TestCaseInfo& testInfo
        )
        {
            if( testInfo.getName() == "" )
            {
                std::ostringstream oss;
                oss << testInfo.getName() << "unnamed/" << ++m_unnamedCount;
                return registerTest( TestCaseInfo( testInfo, oss.str() ) );
            }
            if( m_functions.find( testInfo ) == m_functions.end() )
            {
                m_functions.insert( testInfo );
                m_functionsInOrder.push_back( testInfo );
            }
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual const std::vector<TestCaseInfo>& getAllTests
        ()
        const
        {
            return m_functionsInOrder;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual std::vector<TestCaseInfo> getMatchingTestCases
        ( 
            const std::string& rawTestSpec 
        )
        {
            TestSpec testSpec( rawTestSpec );
            
            std::vector<TestCaseInfo> testList;
            std::vector<TestCaseInfo>::const_iterator it = m_functionsInOrder.begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = m_functionsInOrder.end();
            for(; it != itEnd; ++it )
            {
                if( testSpec.matches( it->getName() ) )
                {
                    testList.push_back( *it );
                    std::cout << it->getName() << std::endl;
                }
            }
            return testList;
        }
        
    private:
        
        std::set<TestCaseInfo> m_functions;
        std::vector<TestCaseInfo> m_functionsInOrder;
        size_t m_unnamedCount;
    };

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    
    
    struct FreeFunctionTestCase : ITestCase
    {
        ///////////////////////////////////////////////////////////////////////////
        FreeFunctionTestCase
        (
            TestFunction fun
        )
        : m_fun( fun )
        {}
        
        ///////////////////////////////////////////////////////////////////////////
		virtual void invoke
		()
		const
		{
			#ifdef _MSC_VER
				// Invoke the test case in a worker thread for crash tolerance
				DWORD  result   = 0;
				DWORD  threadId = 0;
				HANDLE worker   = CreateThread(NULL,0,RunTestInThread,m_fun,0,&threadId);
			
				if(WaitForMultipleObjects(1,&worker,TRUE,INFINITE) == WAIT_OBJECT_0)
				{
					GetExitCodeThread(worker,&result);
				}
				else
				{
					result = FreeFunctionTestCase::EXCEPTION;
				}			
			#else
				// Invoke the test case in a worker thread for crash tolerance
				pthread_t worker;
				void *    retVal;
				pthread_create( &worker, NULL, RunTestInThread, (void*)m_fun );
				pthread_join( worker, &retVal);
				unsigned int result = *((unsigned int*)retVal);			
			#endif

			switch(result)
			{
			case FreeFunctionTestCase::EXCEPTION:
				throw "Test case aborted due to unhandled exception.";
				break;
			case FreeFunctionTestCase::CRASH:
				throw "Test case aborted due to suspected crash.";
				break;
			default:
				break;
			}
		}  
        
        ///////////////////////////////////////////////////////////////////////////
        virtual ITestCase* clone
        ()
        const
        {
            return new FreeFunctionTestCase( m_fun );
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual bool operator == 
        (
            const ITestCase& other 
        )
        const
        {
            const FreeFunctionTestCase* ffOther = dynamic_cast<const FreeFunctionTestCase*> ( &other );
            return ffOther && m_fun == ffOther->m_fun;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual bool operator <
        (
            const ITestCase& other 
        )
        const
        {
            const FreeFunctionTestCase* ffOther = dynamic_cast<const FreeFunctionTestCase*> ( &other );
            return ffOther && m_fun < ffOther->m_fun;
        }
        
		static const unsigned int SUCCESS	= 0U;
		static const unsigned int EXCEPTION = 1U;
		static const unsigned int CRASH		= 2U;
    private:
        TestFunction m_fun;		
    };
	
	void SignalHandler(int sig)
	{
		switch(sig)
		{
		case SIGFPE:
			#ifdef _MSC_VER
			_fpreset();
			#endif
			break;
		default:
			break;
		}

		signal(sig,SIG_DFL);
		
		#ifdef _MSC_VER
			DWORD error = FreeFunctionTestCase::CRASH;
			ExitThread(error);
		#else
			unsigned int * error = const_cast<unsigned int *>(&FreeFunctionTestCase::CRASH);
			pthread_exit(error);
		#endif
	}

	#ifdef _MSC_VER
	DWORD WINAPI RunTestInThread(LPVOID lpParam)
	{
		DWORD error = FreeFunctionTestCase::SUCCESS;

		signal(SIGSEGV,SignalHandler);
		signal(SIGFPE,SignalHandler);
		signal(SIGINT,SignalHandler);
		signal(SIGILL,SignalHandler);
		signal(SIGABRT,SignalHandler);
		try
		{
			((Catch::TestFunction)lpParam)();
		}
		catch(...)
		{
			// NOTE: CHECK_THROWS, REQUIRE_THROWS, etc. will still work.  This just consumes SEH crashes, 
			// and unexpected exceptions that could prevent the test suite from continuing.
			// TODO: pass exception information to calling thread
			error = FreeFunctionTestCase::EXCEPTION;
		}
		signal(SIGSEGV,SIG_DFL);
		signal(SIGFPE,SIG_DFL);
		signal(SIGINT,SIG_DFL);
		signal(SIGILL,SIG_DFL);
		signal(SIGABRT,SIG_DFL);
		return error;
	}
	#else
	void * RunTestInThread(void *lpParam)
	{
		unsigned int * error = const_cast<unsigned int *>(&FreeFunctionTestCase::SUCCESS);

		signal(SIGSEGV,SignalHandler);
		signal(SIGFPE,SignalHandler);
		signal(SIGINT,SignalHandler);
		signal(SIGILL,SignalHandler);
		signal(SIGABRT,SignalHandler);
		try
		{
			((Catch::TestFunction)lpParam)();
		}
		catch(...)
		{
			// NOTE: CHECK_THROWS, REQUIRE_THROWS, etc. will still work.  This just consumes SEH crashes, 
			// and unexpected exceptions that could prevent the test suite from continuing.
			// TODO: pass exception information to calling thread
			error = const_cast<unsigned int *>(&FreeFunctionTestCase::EXCEPTION);
		}
		signal(SIGSEGV,SIG_DFL);
		signal(SIGFPE,SIG_DFL);
		signal(SIGINT,SIG_DFL);
		signal(SIGILL,SIG_DFL);
		signal(SIGABRT,SIG_DFL);
		return error;
	}
	#endif
	
	const unsigned int FreeFunctionTestCase::SUCCESS;
	const unsigned int FreeFunctionTestCase::EXCEPTION;
	const unsigned int FreeFunctionTestCase::CRASH;
        
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////////////
    AutoReg::AutoReg
    (
        TestFunction function, 
        const char* name,
        const char* description,
        const char* filename,
        std::size_t line
    )
    {
        registerTestCase( new FreeFunctionTestCase( function ), name, description, filename, line );
    }    
    
    ///////////////////////////////////////////////////////////////////////////
    AutoReg::~AutoReg
    ()
    {
    }
    
    ///////////////////////////////////////////////////////////////////////////
    void AutoReg::registerTestCase
    (
        ITestCase* testCase, 
        const char* name, 
        const char* description,
        const char* filename,
        std::size_t line
    )
    {
        Hub::getTestCaseRegistry().registerTest( TestCaseInfo( testCase, name, description, filename, line ) );
    }
    
} // end namespace Catch

