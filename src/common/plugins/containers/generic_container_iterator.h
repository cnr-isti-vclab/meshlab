#ifndef HEADERFILE_H
#define HEADERFILE_H

#include <vector>

template <typename PluginType>
class ConstPluginIterator
{
public:
	using Container         = std::vector<PluginType*>;
	using iterator_category = std::forward_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = PluginType*;
	using pointer           = typename Container::const_iterator;
	using reference         = PluginType*&;

	ConstPluginIterator(
			const Container& c,
			const pointer& it,
			bool iterateAlsoDisabledPlugins = false)
		: c(c), m_ptr(it), iterateAlsoDisabledPlugins(iterateAlsoDisabledPlugins)
	{
		if (m_ptr != c.end() && !iterateAlsoDisabledPlugins && !(*m_ptr)->isEnabled())
			++(*this);
	}
	value_type operator*() const {return *m_ptr; }
	pointer operator->() { return m_ptr; }
	ConstPluginIterator& operator++() {
		if (iterateAlsoDisabledPlugins)
			m_ptr++;
		else {
			do {
				m_ptr++;
			} while((m_ptr != c.end()) && !(*m_ptr)->isEnabled());
		}
		return *this;
	}
	ConstPluginIterator operator++(int) {ConstPluginIterator tmp = *this; ++(*this); return tmp; }
	friend bool operator== (const ConstPluginIterator& a, const ConstPluginIterator& b) { return a.m_ptr == b.m_ptr; };
	friend bool operator!= (const ConstPluginIterator& a, const ConstPluginIterator& b) { return a.m_ptr != b.m_ptr; };

private:
	const Container& c;
	pointer m_ptr;
	bool iterateAlsoDisabledPlugins;
};

#endif // HEADERFILE_H
