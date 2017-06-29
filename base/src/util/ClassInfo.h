#ifndef BEEEON_CLASS_INFO_H
#define BEEEON_CLASS_INFO_H

#include <string>
#include <typeindex>
#include <type_traits>

namespace BeeeOn {

class ClassInfo {
public:
	ClassInfo(const std::type_index &index);
	ClassInfo(const std::type_info &info);

	template <typename T>
	ClassInfo(const T &t): m_index(typeid(t))
	{
		static_assert(!std::is_pointer<T>::value,
			"for pointers use forPointer() method");
	}

	template <typename T>
	static ClassInfo forPointer(const T *t)
	{
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

private:
	std::type_index m_index;
};

}

#endif
