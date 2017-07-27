#include <cppunit/extensions/HelperMacros.h>

#include "cppunit/BetterAssert.h"
#include "util/ClassInfo.h"

using namespace std;

namespace BeeeOn {

class ClassInfoTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ClassInfoTest);
	CPPUNIT_TEST(testEmptyConstructor);
	CPPUNIT_TEST(testConstruct);
	CPPUNIT_TEST(testForPointerNull);
	CPPUNIT_TEST(testId);
	CPPUNIT_TEST(testName);
	CPPUNIT_TEST(testNameWithInheritance);
	CPPUNIT_TEST(testIs);
	CPPUNIT_TEST(testByName);
	CPPUNIT_TEST_SUITE_END();

public:
	void testEmptyConstructor();
	void testConstruct();
	void testForPointerNull();
	void testId();
	void testName();
	void testNameWithInheritance();
	void testIs();
	void testByName();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ClassInfoTest);

class TestObject0 {
};

class TestObject1 {
};

void ClassInfoTest::testEmptyConstructor()
{
	ClassInfo info0 = ClassInfo::forClass<TestObject0>();
	ClassInfo info1 = ClassInfo::forClass<TestObject0>();
	ClassInfo info2 = ClassInfo::forClass<TestObject1>();

	CPPUNIT_ASSERT(info0 == info1);
	CPPUNIT_ASSERT(info0 != info2);
	CPPUNIT_ASSERT(info1 != info2);
}

void ClassInfoTest::testConstruct()
{
	TestObject0 o0;
	TestObject1 o1;

	ClassInfo info0(typeid(o0));
	ClassInfo info1(typeid(o1));
	ClassInfo info2(typeid(TestObject0));
	ClassInfo info3(typeid(TestObject1));
	ClassInfo info4(o0);
	ClassInfo info5(o1);
	ClassInfo info6 = ClassInfo::forPointer(&o0);
	ClassInfo info7 = ClassInfo::forPointer(&o1);

	CPPUNIT_ASSERT(info0 == info2);
	CPPUNIT_ASSERT(info2 == info4);
	CPPUNIT_ASSERT(info4 == info6);

	CPPUNIT_ASSERT(info1 == info3);
	CPPUNIT_ASSERT(info3 == info5);
	CPPUNIT_ASSERT(info5 == info7);

	CPPUNIT_ASSERT(info0 != info1);
	CPPUNIT_ASSERT(info2 != info3);
	CPPUNIT_ASSERT(info4 != info5);
	CPPUNIT_ASSERT(info6 != info7);
}

void ClassInfoTest::testForPointerNull()
{
	TestObject0 *o = NULL;
	CPPUNIT_ASSERT(ClassInfo::forPointer(o) == ClassInfo());
}

void ClassInfoTest::testId()
{
	TestObject0 o;
	ClassInfo info0(typeid(o));
	ClassInfo info1(typeid(TestObject0));
	ClassInfo info2(o);
	ClassInfo info3 = ClassInfo::forPointer(&o);

	CPPUNIT_ASSERT_EQUAL("N6BeeeOn11TestObject0E", info0.id());
	CPPUNIT_ASSERT_EQUAL("N6BeeeOn11TestObject0E", info1.id());
	CPPUNIT_ASSERT_EQUAL("N6BeeeOn11TestObject0E", info2.id());
	CPPUNIT_ASSERT_EQUAL("N6BeeeOn11TestObject0E", info3.id());
}

void ClassInfoTest::testName()
{
	TestObject0 o;
	ClassInfo info0(typeid(o));
	ClassInfo info1(typeid(TestObject0));
	ClassInfo info2(o);
	ClassInfo info3 = ClassInfo::forPointer(&o);

	CPPUNIT_ASSERT_EQUAL("BeeeOn::TestObject0", info0.name());
	CPPUNIT_ASSERT_EQUAL("BeeeOn::TestObject0", info1.name());
	CPPUNIT_ASSERT_EQUAL("BeeeOn::TestObject0", info2.name());
	CPPUNIT_ASSERT_EQUAL("BeeeOn::TestObject0", info3.name());
}

class A {
public:
	virtual ~A() {}
};

class B {
public:
};

class C : public A {
public:
};

class D : public B {
public:
};

class E : public A, public B {
public:
};

void ClassInfoTest::testNameWithInheritance()
{
	A a;
	B b;
	C c;
	D d;
	E e;

	A &cAsA = c;
	B &dAsB = d;
	A &eAsA = e;
	B &eAsB = e;

	CPPUNIT_ASSERT("BeeeOn::A" == ClassInfo(typeid(a)).name());
	CPPUNIT_ASSERT("BeeeOn::B" == ClassInfo(typeid(b)).name());
	CPPUNIT_ASSERT("BeeeOn::C" == ClassInfo(typeid(c)).name());
	CPPUNIT_ASSERT("BeeeOn::D" == ClassInfo(typeid(d)).name());
	CPPUNIT_ASSERT("BeeeOn::E" == ClassInfo(typeid(e)).name());
	CPPUNIT_ASSERT("BeeeOn::C" == ClassInfo(typeid(cAsA)).name());
	CPPUNIT_ASSERT("BeeeOn::B" == ClassInfo(typeid(dAsB)).name());
	CPPUNIT_ASSERT("BeeeOn::E" == ClassInfo(typeid(eAsA)).name());
	CPPUNIT_ASSERT("BeeeOn::B" == ClassInfo(typeid(eAsB)).name());

	CPPUNIT_ASSERT("BeeeOn::A" == ClassInfo(typeid(&a)).name());
	CPPUNIT_ASSERT("BeeeOn::B" == ClassInfo(typeid(&b)).name());
	CPPUNIT_ASSERT("BeeeOn::C" == ClassInfo(typeid(&c)).name());
	CPPUNIT_ASSERT("BeeeOn::D" == ClassInfo(typeid(&d)).name());
	CPPUNIT_ASSERT("BeeeOn::E" == ClassInfo(typeid(&e)).name());
	CPPUNIT_ASSERT("BeeeOn::A" == ClassInfo(typeid(&cAsA)).name());
	CPPUNIT_ASSERT("BeeeOn::B" == ClassInfo(typeid(&dAsB)).name());
	CPPUNIT_ASSERT("BeeeOn::A" == ClassInfo(typeid(&eAsA)).name());
	CPPUNIT_ASSERT("BeeeOn::B" == ClassInfo(typeid(&eAsB)).name());
}

void ClassInfoTest::testIs()
{
	ClassInfo info(typeid(A));

	CPPUNIT_ASSERT(info.is<A>());
	CPPUNIT_ASSERT(!info.is<B>());
	CPPUNIT_ASSERT(!info.is<C>());

	A a;
	CPPUNIT_ASSERT(ClassInfo::forPointer(&a).is<A>());
	CPPUNIT_ASSERT(!ClassInfo::forPointer(&a).is<B>());
	CPPUNIT_ASSERT(!ClassInfo::forPointer(&a).is<C>());

	CPPUNIT_ASSERT(ClassInfo::forClass<A>().is<A>());
	CPPUNIT_ASSERT(!ClassInfo::forClass<A>().is<B>());
	CPPUNIT_ASSERT(!ClassInfo::forClass<A>().is<C>());
}

}

BEEEON_CLASS(BeeeOn::TestObject0)

namespace BeeeOn {

void ClassInfoTest::testByName()
{
	ClassInfo info = ClassInfo::byName("BeeeOn::TestObject0");
	CPPUNIT_ASSERT_EQUAL("BeeeOn::TestObject0", info.name());
	CPPUNIT_ASSERT(type_index(typeid(TestObject0)) == info.index());

	ClassInfo::registerClass<TestObject1>("xxx");
	ClassInfo xxx = ClassInfo::byName("xxx");
	CPPUNIT_ASSERT_EQUAL("BeeeOn::TestObject1", xxx.name());
	CPPUNIT_ASSERT(type_index(typeid(TestObject1)) == xxx.index());
}

}
