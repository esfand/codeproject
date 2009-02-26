///////////////////////////////////////////////////////////////////////////////
// TESTRESULT.H
// A TestResult is a collection of the history of some test runs.  Right now
// it just collects failures.
///////////////////////////////////////////////////////////////////////////////

#ifndef TESTRESULT_H
#define TESTRESULT_H

class Failure;
class Success;

class TestResult
{
public:
					TestResult		(void);
					TestResult		(const bool, const bool, const bool);
	virtual void	TestsStarted	(void);
	virtual void	AddFailure		(const Failure& failure);
	virtual void	AddSuccess		(const Success& failure);
	virtual void	AddCheckPoint	(void)	{ allCheckCount_++; };
	virtual void	TestsEnded		(int);
	virtual	bool	Statistics		(void)	{ return statistics_; }
	virtual	bool	Trace			(void)	{ return trace_; }
	virtual int		Failures		(void)	{ return failureCount_; }
	virtual int		Summary			(void)	{ return allCheckCount_; }

private:
	int		failureCount_;
	int		successCount_;
	int		allCheckCount_;
	bool	debug_;
	bool	trace_;
	bool	statistics_;
};

#endif
