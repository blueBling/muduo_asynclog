#ifndef _PTR_VECTOR_HH
#define _PTR_VECTOR_HH

#include <vector>
#include <memory>
#include <assert.h>

namespace myself
{

#define ASSERT(If, Msg) \
	if(!(If)) \
	{\
		fprintf(stderr, "Error/(%s, %d): %s, abort.\n", __FILE__, __LINE__, Msg); abort();\
	}

template<typename T>
class ptr_vector
{
public:
	typedef unsigned int size_type;
	typedef std::vector<T*> vector;
	typedef T* value_type;
	typedef value_type& reference;

	explicit ptr_vector()
	{
	}

	//ptr_vector析构的时候会析构自己开辟出来的存放指针的空间,同时析构指针本身指向的空间
	//而一般容器不会析构指针本身指向的空间
	~ptr_vector()
	{
		clear();
	}

	void clear()
	{
		if(!m_vector.empty()) {
			//typename vector::iterator it;
			for(auto it = m_vector.begin(); it != m_vector.end(); ++it) {
				delete *it;//释放指针指向的内存.
			}
		}

		m_vector.clear();//释放指针本身.
	}

	//在尾部追加元素
	//unique_ptr对象包装一个原始指针，并负责其生命周期。
	//当该对象被销毁时，会在其析构函数中删除关联的原始指针,防止内存泄漏
	void push_back(T* const &v)
	{
		ASSERT(v, "NULL point at ptr_vector push_back()");
		std::unique_ptr<T> tmp(v);
		m_vector.push_back(v); //使用 unique_ptr 保证push_back失败时，v也能正常释放.
		tmp.release();
	}

	//删除最后一个元素并返回指向删除对象的指针
	std::unique_ptr<T> pop_back()
	{
		ASSERT( !m_vector.empty(), "'pop_back()' on empty container");
		std::unique_ptr<T> tmp(m_vector.back());
		m_vector.pop_back();
		return std::move(tmp);
	}

	//提供像c数组通过下表访问的方法
	reference operator[](size_type n)
	{
		ASSERT(n < size(), "operator[] n out of the border")
		return m_vector[n];
	}

	bool empty()
	{
		return m_vector.empty();
	}

	size_type size()
	{
		return m_vector.size();
	}

	void reserve(size_type n)
	{
		m_vector.reserve(n);
	}

	void resize(size_type s)
	{
		size_type size = this->size();
		if(s < size) {
			for(auto it = m_vector.begin() + s; it != m_vector.end(); ++it) {
				delete *it;//释放指针指向的内存.
			}
			m_vector.resize(s);
		} else if(s > size) {
			for(; size != s; ++size) {
				push_back(new T);
			}
		}
		ASSERT(s == this->size(), "'resize' error size asymmetry");
	}

	void swap(ptr_vector<T>& v)
	{
		m_vector.swap(v.base());
	}

private:
	//将拷贝构造和赋值  设置为私有方法,防止所有权转移
	ptr_vector<T>& operator=(const ptr_vector<T>&);
	ptr_vector<T>(ptr_vector<T>&);

	vector& base()
	{
		return m_vector;
	}

	vector m_vector;
};

}

#endif