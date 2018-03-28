#ifndef RESGUARD_H
#define RESGUARD_H

#include <utility>

/**
 * Moveonly resource guard.
 *
 * @tparam T Resource type.
 * @tparam D Function type to release resource.
 */
template<typename T,typename D> class ResGuard
{
public:

	/**
	 * Constructor,ownership of obj is transfer to this ResGuard.
	 *
	 * @param obj	  Resource object.
	 * @param deleter Deleter for release resource.
	 */
	ResGuard(T obj, D deleter);

	/**
	 * Copy constructor(deleted)
	 */
	ResGuard(const ResGuard&) = delete;

	/**
	 * Move constructor
	 *
	 * @param [in,out] rhs The right hand side.
	 */
	ResGuard(ResGuard &&rhs) noexcept(noexcept(swap(rhs)));

	/**
	 * Destructor,release the reource.
	 */
	~ResGuard();

	/**
	 * Assignment operator(deleted)
	 */
	ResGuard& operator=(const ResGuard&) = delete;

	/**
	 * Move assignment operator
	 *
	 * @param [in,out] rhs The right hand side.
	 *
	 * @return A shallow copy of this object.
	 */
	ResGuard& operator=(ResGuard &&rhs) noexcept(noexcept(swap(rhs)));

	/**
	 * Gets the raw resource.
	 *
	 * @return Resource object owned by this ResGuard.
	 */
	T get()
	{
		return m_obj;
	}

	/**
	 * Swaps the given right hand side
	 *
	 * @param [in,out] rhs The right hand side.
	 */
	void swap(ResGuard<T, D> &rhs) noexcept(std::is_nothrow_move_constructible<T>::value&&std::is_nothrow_move_assignable<T>::value
		&&std::is_nothrow_move_constructible<D>::value&&std::is_nothrow_move_assignable<D>::value);

private:
	T m_obj;	/**< The resource object */

	D m_deleter;	/**< The deleter */
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