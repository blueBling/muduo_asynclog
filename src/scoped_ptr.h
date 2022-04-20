#ifndef _SCOPE_PTR_HH
#define _SCOPE_PTR_HH

#include "stddef.h"

//  scoped_ptr mimics a built-in pointer except that it guarantees deletion
//  of the object pointed to, either on destruction of the scoped_ptr or via
//  an explicit reset(). scoped_ptr is a simple solution for simple needs;
//  use shared_ptr or std::auto_ptr if your needs are more complex.

/*
scoped_ptr 是局部智能指针 不允许转让所有权。
*/
template <class T>
class scoped_ptr
{
public:

	//存储p的一份拷贝。注意，p 必须是用operator new分配的，或者是null. 在构造的时候，不要求T必须是一个完整的类型。
	//当指针p是调用某个分配函数的结果而不是直接调用new得到的时候很有用：因为这个类型不必是完整的，只需要类型T的一个前向声明就可以了。
	//这个构造函数不会抛出异常。
	scoped_ptr(T *p = 0) :m_ptr(p)
	{
	}


	//当scoped_ptr对象的生命周期结束时，析构函数~scoped_ptr()会使用delete操作自动销毁所保存的指针对象，从而正确的回收资源。
	~scoped_ptr()
	{
		delete m_ptr;
	}

	//该运算符返回一个智能指针中存储的指针所指向的对象的引用。
	//由于不允许空的引用，所以解引用一个拥有空指针的scoped_ptr将导致未定义行为。
	//如果不能肯定所含指针是否有效，就用函数get替代解引用。这个函数不会抛出异常。
	T&operator*() const
	{
		return *m_ptr;
	}

	//返回智能指针所保存的指针。
	//如果保存的指针为空，则调用这个函数会导致未定义行为。
	//如果不能肯定指针是否空的，最好使用函数get。这个函数不会抛出异常。
	T*operator->() const
	{
		return m_ptr;
	}

	//拥有权不允许转让  但是可以让智能指针指向另一个空间
	//重置scoped_ptr；它删除原来报存的指针，再保存新的指针值p。
	//如果p是空指针，那么scoped_ptr将不能持有任何指针。
	//一般情况下reset()不应该被调用，因为它违背了scopd_ptr的本意——资源应该一直由scopd_ptr自己自动管理。
	void reset(T *p)
	{
		if (p != m_ptr && m_ptr != 0)
			delete m_ptr;
		m_ptr = p;
	}

	//返回保存的指针。
	//应该小心地使用get，因为它可以直接操作裸指针。
	//但是，get使得你可以测试保存的指针是否为空。这个函数不会抛出异常。get通常在调用那些需要裸指针的函数时使用。
	T* get() const
	{
		return m_ptr;
	}

	//返回scoped_ptr是否为非空。
	//在C++中，operator TypeName()语法用来将对象转换为指定的TypeName类型，当这里TypeName为bool时，就可以直接在条件判断式里面直接用该对象
	//参考https://www.cnblogs.com/HachikoT/p/12731372.html
	//在if语句中最好使用这个类型转换函数，而不要用get去测试scoped_ptr的有效性
	operator bool() const
	{
		return get() != NULL;
	}

private:
    //将拷贝构造和赋值  以及判等判不等  都设置为私有方法
	//对象不再能调用，即不能拷贝构造和赋值  也就达到了不让转移拥有权的目的
	scoped_ptr(const scoped_ptr<T> &y);
	const scoped_ptr<T> operator=(const scoped_ptr<T> &);
	void operator==(scoped_ptr<T> const &) const;
	void operator!=(scoped_ptr<T> const &) const;

	T* m_ptr;
};

#endif