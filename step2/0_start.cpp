#include <cstddef>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <array>

namespace algo
{
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
    return m_arr.begin();
  }

  constexpr auto begin() const
  {
    return m_arr.begin();
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
    return m_arr.end();
  }
  constexpr auto reserved_end() const
  {
    return m_arr.end();
  }

private:
  std::array<ty, n> m_arr;
  size_t m_size;
};

int main()
{
  //constexpr vector<int, 10> v;
  return 0;
}

