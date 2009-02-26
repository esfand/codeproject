///////////////////////////////////////////////////////////////////////////////
// TESTREGISTRY.H
// TestRegistry is a singleton collection of all the tests to run in a system.  
///////////////////////////////////////////////////////////////////////////////

#ifndef TESTREGISTRY_H
#define TESTREGISTRY_H

class ITest;
class TestResult;

class TestRegistry
{
public:
	TestRegistry();
	~TestRegistry();
	static void AddTest		(ITest *test);
	static void RunAllTests (TestResult& result);
private:
	static	TestRegistry&
			Instance();
	void	Add		(ITest *test);
	void	Run		(TestResult& result);
	void	Finalize(int,int);
	ITest*	tests_;
	ITest*	last_;
	int		testsNumber_;
};

#endif