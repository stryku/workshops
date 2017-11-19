#include <cstddef>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace traits
{
  template <typename... types>
  constexpr auto all_true = std::conjunction<types...>::value;
}

namespace algo
{
  template <typename it_t, typename dit_t>
  constexpr void copy(it_t first, it_t last, dit_t d_first)
  {
    while (first != last) 
    {
      *d_first++ = *first++;
    }
  }

  template <typename it_t, typename value_t>
  constexpr void fill(it_t first,it_t last, const value_t& value)
  {
    while (first != last) 
    {
      *first++ = value;
    }
  }

  template <typename it_t, typename value_t>
  constexpr size_t count(it_t first, it_t last, const value_t& value)
  {
    size_t result{ 0u };
    while (first != last) 
    {
      result += *first++ == value;
    }
    return result;
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

  template <typename it_t>
  constexpr void iter_swap(it_t lhs, it_t rhs)
  {
    const auto tmp = *rhs;
    *rhs = *lhs;
    *lhs = tmp;
  }

  template <typename it_t>
  constexpr void reverse(it_t first, it_t last)
  {
    while ((first != last) && (first != --last)) 
    {
      iter_swap(first++, last);
    }
  }

  template <typename it_t>
  constexpr it_t next(it_t iterator, int n = 1)
  {
    return iterator + n;
  }

  template <typename it_t>
  constexpr it_t prev(it_t iterator, int n = 1)
  {
    return next(iterator, -n);
  }

  template <typename it_t>
  constexpr void advance(it_t &iterator, int n = 1)
  {
    iterator += n;
  }

  template <typename it_t, typename predicate_t>
  constexpr auto find_if(it_t first, it_t last, predicate_t pred)
  {
    for(; first != last; ++first)
    {
      if(pred(*first))
      {
        return first;
      }
    }

    return last;
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
private:
  using base = vector<char, n>;

public:
  constexpr fixed_string()
  {
    algo::fill(base::begin(), base::reserved_end(), '\0');
  }

  template <typename... ts>
  constexpr fixed_string(ts... args)
  {
    static_assert(traits::all_true<std::is_same<ts, char>...>);

    for(const auto c : { args... })
    {
      base::push_back(c);
    }
  }
};


template <typename... ts>
fixed_string(ts...) -> fixed_string<sizeof...(ts) + 1u>; // +1 for '\0'

using string = fixed_string<10u>;

template <typename T, T... args>
constexpr auto operator"" _s()
{
  return fixed_string{ args... };
}

namespace algo
{
  constexpr string to_string(size_t val)
  {
    string result;

    if(val == 0u)
    {
      result.push_back('0');
      return result;
    }

    while(val > 0u)
    {
      result.push_back(static_cast<char>(val % 10u + '0'));
      val /= 10u;
    }

    algo::reverse(result.begin(), result.end());

    return result;
  }

  constexpr size_t stoui(string str)
  {
    size_t result{ 0u };

    for(const char c : str)
    {
        result *= 10u;
        result += static_cast<size_t>(c - '0');
    }

    return result;
  }
}

constexpr auto asm_code = 
  "sub esp , 4 "
  "mov ebp , esp "
  "mov [ ebp + 2 ] , 0 "
  "mov [ ebp + 3 ] , 1 "
  "mov [ ebp + 4 ] , 1 "
  "mov [ ebp + 1 ] , 1 "
  "mov ecx , 1 "
":loop "
  "cmp ecx , 6 " //we want to get 6th fibonacci element
  "je .end "
  "mov eax , [ ebp + 3 ] "
  "add eax , [ ebp + 2 ] "
  "mov [ ebp + 4 ] , eax "
  "mov eax , [ ebp + 3 ] "
  "mov [ ebp + 2 ] , eax "
  "mov eax , [ ebp + 4 ] "
  "mov [ ebp + 3 ] , eax "
  "mov eax , [ ebp + 1 ] "
  "inc ecx "
  "jmp .loop "
":end "
  "mov eax , [ ebp + 4 ] "
  "exit"_s;


template <typename it_t>
constexpr auto do_smth(it_t begin, it_t end)
{
  //auto copy = vector<decltype(*it), std::distance(begin, end)>{};
  //                                  ^^^^^^^^^^^^^^^^^^^^^^^^^

  //...
}

constexpr const char foo[] = "foo";

int main()
{
  constexpr auto begin = std::begin(foo);
  constexpr auto end = std::end(foo);

  do_smth(begin, end);

  return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////



namespace tests
{
  namespace _s_tests
  {
      constexpr auto str = "arr"_s;

      static_assert(str.size() == 3u);
      static_assert(str[0] == 'a');
      static_assert(str[1] == 'r');
      static_assert(str[2] == 'r');
      static_assert(str[3] == '\0');
  }
}

