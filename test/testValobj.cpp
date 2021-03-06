#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>

struct Udata{int x;};
LUAPP_USERDATA(Udata, "Test.Userdata")

BOOST_AUTO_TEST_SUITE(Valobj)


#define v vs[0]
#define l context.global["val"]

BOOST_FIXTURE_TEST_CASE(refCopy, fxGlobalVal)
{
	v = 3.14;
	lua::Valref copy = v;
	BOOST_CHECK_EQUAL(copy.cast<double>(), 3.14);
}



BOOST_FIXTURE_TEST_CASE(Assignment, fxGlobalVal)
{
	l = 3.14;
	v = l;
	BOOST_CHECK_EQUAL(v.cast<double>(), 3.14);
	v = 1.28;
	l = v;
	BOOST_CHECK_EQUAL(l.cast<double>(), 1.28);
}



BOOST_FIXTURE_TEST_CASE(Length, fxGlobalVal)
{
	context.runString("val = {1, 2, 3}");
#if(LUAPP_API_VERSION >= 52)
	BOOST_CHECK_EQUAL(l.len().cast<unsigned int>(), 3u);
	v = l;
	BOOST_CHECK_EQUAL(v.len().cast<unsigned int>(), 3u);
	BOOST_CHECK_EQUAL(v.rawlen(), 3u);
	BOOST_CHECK_EQUAL(v.rawlen(), 3u);
#else	// V51-
	BOOST_CHECK_EQUAL(l.len(), 3u);
	v = l;
	BOOST_CHECK_EQUAL(v.len(), 3u);
#endif
}


#if(LUAPP_API_VERSION >= 53)
BOOST_FIXTURE_TEST_CASE(Linked, fxContext)
{
	using lua::Value;
	using lua::Nil;
	context.mt<Udata>() = lua::Table::records(context);
	{
		Value val{Udata{7}, context};
		BOOST_CHECK(val.linked().is<Nil>());
		val.linked() = 8;
		BOOST_CHECK_EQUAL(val.linked().to<int>(), 8);
	}
	{
		Value val{7, context};
		BOOST_CHECK(val.linked().is<Nil>());
		val.linked() = 8;
		BOOST_CHECK(val.linked().is<Nil>());
	}
	{
		context.global["tmp"] = Udata{7};
		BOOST_CHECK(context.global["tmp"].linked().is<Nil>());
		context.global["tmp"].linked() = 8;
		BOOST_CHECK_EQUAL(context.global["tmp"].linked().to<int>(), 8);
	}
	{
		context.global["tmp"] = 7;
		BOOST_CHECK(context.global["tmp"].linked().is<Nil>());
		context.global["tmp"].linked() = 8;
		BOOST_CHECK(context.global["tmp"].linked().is<Nil>());
	}
}
#endif	// V53+


BOOST_FIXTURE_TEST_CASE(Metatables, fxGlobalVal)
{
	context.runString("val = {1, 2, 3}");
	v = l;
	context.runString("val = {1, 2, 3, 4}");
	context.runString("mt1 = {1}");
	context.runString("mt2 = {2}");

	v.mt() = context.global["mt1"];
	l.mt() = context.global["mt2"];

	BOOST_CHECK(v.mt() == context.global["mt1"]);
	BOOST_CHECK_EQUAL(v.mt()[1].cast<int>(), 1);
	BOOST_CHECK(l.mt() == context.global["mt2"]);
	BOOST_CHECK_EQUAL(l.mt()[1].cast<int>(), 2);
}




BOOST_AUTO_TEST_SUITE_END()
