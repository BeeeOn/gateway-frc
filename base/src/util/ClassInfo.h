#ifndef BEEEON_CLASS_INFO_H
#define BEEEON_CLASS_INFO_H

#include <string>
#include <typeindex>
#include <type_traits>

namespace BeeeOn {

class ClassInfo {
private:
	class NoneType {
		NoneType() = delete;
	};

public:
	ClassInfo();
	ClassInfo(const std::type_index &index);
	ClassInfo(const std::type_info &info);

	template <typename T>
	ClassInfo(const T &t): m_index(typeid(t))
	{
		static_assert(!std::is_pointer<T>::value,
			"for pointers use forPointer() method");
	}

	/**
	 * Determine class type info for the value of the given
	 * pointer. If the pointer is NULL, it returns ClassInfo(),
	 * i.e. type info of ClassInfo::NoneType.
	 */
	template <typename T>
	static ClassInfo forPointer(const T *t)
	{
		if (t == NULL)
			return ClassInfo();

		return ClassInfo(*t);
	}

	template <typename T>
	static ClassInfo forClass()
	{
		return ClassInfo(typeid(T));
	}

	std::string id() const;
	std::string name() const;
	std::type_index index() const;

	/**
	 * Test whether the ClassInfo represents the given type T.
	 * It does not work for inheritance.
	 */
	template <typename T>
	bool is() const
	{
		return ClassInfo(typeid(T)) == *this;
	}

	bool operator <(const ClassInfo &info) const
	{
		return info.index() < m_index;
	}

	bool operator ==(const ClassInfo &info) const
	{
		return info.index() == m_index;
	}

	bool operator !=(const ClassInfo &info) const
	{
		return info.index() != m_index;
	}

	static ClassInfo byName(const std::string &name);

	template <typename T>
	static void registerClass(const std::string &name)
	{
		registerClassInfo(name, typeid(T));
	}

protected:
	static void registerClassInfo(
			const std::string &name,
			const std::type_info &info);

private:
	std::type_index m_index;
};

#define BEEEON_CLASS_REGISTER(T, name, reg)     \
class reg {                                     \
public:                                         \
	reg()                                   \
	{                                       \
		ClassInfo::                     \
			registerClass<T>(name); \
	}                                       \
};                                              \
static reg reg##Register;

#define BEEEON_CLASS(T)                         \
namespace BeeeOn {                              \
namespace Reflex {                              \
BEEEON_CLASS_REGISTER(T, #T, Class##__COUNTER__)\
}                                               \
}

}

#endif
