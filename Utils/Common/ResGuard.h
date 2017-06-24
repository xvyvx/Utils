#ifndef RESGUARD_H
#define RESGUARD_H

#include <utility>

template<typename T,typename D> class ResGuard
{
public:
	ResGuard(T obj, D deleter);

	ResGuard(const ResGuard&) = delete;

	ResGuard(ResGuard &&rhs) noexcept(noexcept(swap(rhs)));

	~ResGuard();

	ResGuard& operator=(const ResGuard&) = delete;

	ResGuard& operator=(ResGuard &&rhs) noexcept(noexcept(swap(rhs)));

	T get()
	{
		return m_obj;
	}

	void swap(ResGuard<T, D> &rhs) noexcept(std::is_nothrow_move_constructible<T>::value&&std::is_nothrow_move_assignable<T>::value
		&&std::is_nothrow_move_constructible<D>::value&&std::is_nothrow_move_assignable<D>::value);

private:
	T m_obj;

	D m_deleter;
};

template<typename T, typename D> void  ResGuard<T, D>::swap(ResGuard<T, D> &rhs) noexcept(std::is_nothrow_move_constructible<T>::value&&std::is_nothrow_move_assignable<T>::value
	&&std::is_nothrow_move_constructible<D>::value&&std::is_nothrow_move_assignable<D>::value)
{
	std::swap(m_obj, rhs.m_obj);
	std::swap(m_deleter, rhs.m_deleter);
}

template<typename T, typename D> ResGuard<T, D>::ResGuard(T obj, D deleter) :m_obj(obj), m_deleter(deleter)
{
}

template<typename T, typename D> ResGuard<T, D>::ResGuard(ResGuard<T,D> &&rhs) noexcept(noexcept(swap(rhs)))  : m_obj(0), m_deleter()
{
	swap(rhs);
}

template<typename T, typename D> ResGuard<T, D>::~ResGuard()
{
	if(m_obj)
	{
		m_deleter(m_obj);
	}
}

template<typename T, typename D> ResGuard<T, D>& ResGuard<T, D>::operator=(ResGuard<T, D> &&rhs) noexcept(noexcept(swap(rhs)))
{
	swap(rhs);
	return *this;
}

#endif /* RESGUARD_H */