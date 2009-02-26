///////////////////////////////////////////////////////////////////////////////
// TEST.H
// This file contains the Test class along with the macros which make effective
// in the harness.
///////////////////////////////////////////////////////////////////////////////

#ifndef TEST_H
#define TEST_H

#include <cmath>
#include "SimpleString.h"
#include <stdio.h>
#include "TVTInterface.h"

class TestResult;

class ITest
{
public:
	ITest (const EasyString& testName, const EasyString& testGroup);
	~ITest ();	//	01.07.2005
	virtual void	Run			(TestResult& result)	= 0;
	virtual void	Analysis	(const bool,const bool) = 0;
	void			SetNext		(ITest *test);
	ITest*			GetNext		(void) const;
	void			Error		(void)	{ error_ = true; }
	bool			Failed		(void)	{ return error_; }
	virtual void	Statistics	(const bool enable);

	char*			Name		(void)	{ return name_.AsCharString(); }
	char*			Group		(void)	{ return group_.AsCharString(); }

protected:
	bool Check (long expected, long actual, TestResult& result, const EasyString& fileName, long lineNumber);
	bool Check (const EasyString& expected, const EasyString& actual, TestResult& result, const EasyString& fileName, long lineNumber);

	EasyString	name_;
	EasyString	group_;
	ITest*		next_;
	bool		error_;

public:
	ITest*		test_;
	bool		continue_if_error_;
//	Statistics
	int			check_;
	int			assertTrue_;
	int			assertFalse_;
	int			checkEqual_;
	int			longsEqual_;
	int			doublesEqual_;
	int			failEqual_;
};

#define TEST(testName,testGroup)\
	class testGroup##testName##Test : public ITest \
{	public: testGroup##testName##Test () : ITest (#testName,#testGroup) {} \
	void Analysis (const bool,const bool); \
	void Run (TestResult& result_); } \
	testGroup##testName##Instance; \
	void testGroup##testName##Test::Analysis(const bool enableStatistics, const bool enableTrace) { \
	if (!error_) { \
	if (enableTrace) { \
	fprintf (stdout, "[ ] Test(%s,%s) is executed successfully.\n",name_,group_); \
	TVTSendTestSuccess( \
		name_.AsCharString(),	\
		group_.AsCharString(),	\
		check_,					\
		assertTrue_,			\
		assertFalse_,			\
		checkEqual_,			\
		longsEqual_,			\
		doublesEqual_,			\
		failEqual_				\
	); }} \
	else { \
	if (enableTrace) { \
	TVTSendTestError(name_.AsCharString(),group_.AsCharString(), \
		check_,					\
		assertTrue_,			\
		assertFalse_,			\
		checkEqual_,			\
		longsEqual_,			\
		doublesEqual_,			\
		failEqual_				\
	); }} \
	Statistics(enableStatistics); } \
	void testGroup##testName##Test::Run(TestResult& result_) 

#define USE(function)\
		Test_##function(this,result_)

#define TEST_FUNCTION(function)\
	void Test_##function(ITest* test_, TestResult& result_)

#define CHECK(condition)\
{	test_->check_++; \
	result_.AddCheckPoint(); if (!(condition)) \
	{	result_.AddFailure(Failure(test_->Name(),test_->Group(),"CHECK",__FILE__,__LINE__,#condition)); test_->Error(); if (!test_->continue_if_error_) return; } \
		result_.AddSuccess(Success(test_->Name(),test_->Group(),"CHECK",__FILE__,__LINE__,#condition)); } 

#define ASSERT_TRUE(condition)\
{	test_->assertTrue_++; \
	result_.AddCheckPoint(); if (!(condition)) \
	{	result_.AddFailure(Failure(test_->Name(),test_->Group(),"ASSERT_TRUE",__FILE__,__LINE__,#condition)); test_->Error(); if (!test_->continue_if_error_) return; } \
		result_.AddSuccess(Success(test_->Name(),test_->Group(),"ASSERT_TRUE",__FILE__,__LINE__,#condition)); } 

#define ASSERT_FALSE(condition)\
{	test_->assertFalse_++; \
	result_.AddCheckPoint(); if ((condition)) \
	{	result_.AddFailure(Failure(test_->Name(),test_->Group(),"ASSERT_FALSE",__FILE__,__LINE__,#condition)); test_->Error(); if (!test_->continue_if_error_) return; } \
		result_.AddSuccess(Success(test_->Name(),test_->Group(),"ASSERT_FALSE",__FILE__,__LINE__,#condition)); } 

#define CHECK_EQUAL(expected,actual)\
{	test_->checkEqual_++;	\
	result_.AddCheckPoint(); if (StringFrom(expected) == StringFrom(actual)) \
	{	result_.AddSuccess(Success(test_->Name(),test_->Group(),"CHECK_EQUAL",__FILE__,__LINE__,StringFrom(expected),StringFrom(actual))); } \
	else { result_.AddFailure(Failure(test_->Name(),test_->Group(),"CHECK_EQUAL",__FILE__,__LINE__,StringFrom(expected),StringFrom(actual))); test_->Error(); if (!test_->continue_if_error_) return; } } 

#define LONGS_EQUAL(expected,actual)\
{	\
	test_->longsEqual_++;	\
	long actualTemp		= actual; \
	long expectedTemp	= expected; \
	result_.AddCheckPoint(); \
	if ((expectedTemp) != (actualTemp)) \
	{ \
		result_.AddFailure(Failure(test_->Name(),test_->Group(),"LONGS_EQUAL",__FILE__,__LINE__,StringFrom(expectedTemp),StringFrom(actualTemp))); test_->Error(); if (!test_->continue_if_error_) return; \
	} \
	else result_.AddSuccess(Success(test_->Name(),test_->Group(),"LONGS_EQUAL",__FILE__,__LINE__,StringFrom(expectedTemp),StringFrom(actualTemp))); \
} 

#define DOUBLES_EQUAL(expected,actual,threshold)\
{	\
	test_->doublesEqual_++;	\
	double actualTemp = actual; \
	double expectedTemp = expected; \
	result_.AddCheckPoint(); \
	if (fabs((expectedTemp)-(actualTemp)) > threshold) \
	{	result_.AddFailure (Failure (test_->Name(),test_->Group(), "DOUBLES_EQUAL", __FILE__, __LINE__, \
		StringFrom((double)expectedTemp), StringFrom((double)actualTemp)));\
		test_->Error(); if (!test_->continue_if_error_) return; \
	}	\
	else	\
	{	\
		result_.AddSuccess(Success(test_->Name(),test_->Group(),"DOUBLES_EQUAL",__FILE__,__LINE__,StringFrom((double)expectedTemp),StringFrom((double)actualTemp))); \
	}	\
}

#define FAIL(text) \
{	test_->failEqual_++;	\
	result_.AddCheckPoint(); result_.AddFailure (Failure(test_->Name(),test_->Group(),"FAIL",__FILE__,__LINE__,(text))); if (!test_->continue_if_error_) return; } 

#endif
