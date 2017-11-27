#include <cstddef>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace algo
{
  template <typename it_t, typename value_t>
  constexpr void fill(it_t first,it_t last, const value_t& value)
  {
    while (first != last) 
    {
      *first++ = value;
    }
  }

  template <typename it_t, typename it2_t>
  constexpr bool equal(it_t first, it_t last, it2_t first2)
  {
    while(first != last)
    {
      if(!(*first++ == *first2++))
      {
        return false;
      }
    }

    return true;
  }
}

template <typename ty, size_t n>
class vector
{
public:
  constexpr size_t size() const
  {
    return m_size;
  }

  constexpr auto begin()
  {
    return m_arr;
  }

  constexpr auto begin() const
  {
    return m_arr;
  }

  constexpr auto end()
  {
    return begin() + size();
  }

  constexpr auto end() const
  {
    return begin() + size();
  }

  constexpr auto front() const
  {
    return *begin();
  }

  constexpr auto push_back(const ty& val)
  {
    m_arr[m_size++] = val;
  }

  constexpr decltype(auto) operator[](size_t i)
  {
    return m_arr[i];
  }

  constexpr decltype(auto) operator[](size_t i) const
  {
    return m_arr[i];
  }

  template <size_t rhs_n>
  constexpr auto operator==(const vector<ty, rhs_n>& rhs) const
  {
    return size() == rhs.size()
        && algo::equal(begin(), end(), rhs.begin());
  }

  constexpr auto resize_to_reserved()
  {
    m_size = n;
  }

protected:
  constexpr auto reserved_end()
  {
    return begin() + n;
  }
  constexpr auto reserved_end() const
  {
    return begin() + n;
  }

private:
  ty m_arr[n]{};
  size_t m_size{ 0u };
};

template <size_t n>
class fixed_string : public vector<char, n>
{
public:
  constexpr fixed_string()
  {
    //todo
  }
};

int main()
{
  return 0;
}
