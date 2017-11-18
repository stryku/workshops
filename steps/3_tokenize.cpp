#include <cstddef>
#include <iterator>
#include <algorithm>
#include <type_traits>

//unit used in machine for memory cells, registers etc.
using unit_t = uint64_t;

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
  constexpr void fill(it_t first,it_t last, value_t value)
  {
    while (first != last) 
    {
      *first++ = value;
    }
  }

  template <typename it_t, typename value_t>
  constexpr size_t count(it_t first, it_t last, value_t value)
  {
    size_t ret{ 0u };
    while (first != last) 
    {
      ret += *first++ == value;
    }
    return ret;
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

template <typename ty, size_t N>
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

  constexpr const auto begin() const
  {
    return m_arr;
  }

  constexpr auto end()
  {
    return begin() + size();
  }

  constexpr const auto end() const
  {
    return begin() + size();
  }

  constexpr char front() const
  {
    return *begin();
  }

  constexpr void push_back(ty val)
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
  constexpr bool operator==(const vector<ty, rhs_n>& rhs) const
  {
    return size() == rhs.size() &&
           algo::equal(begin(), end(), rhs.begin());
  }

  constexpr void resize_to_reserved()
  {
    m_size = N;
  }

protected:
  constexpr auto reserved_end()
  {
    return begin() + N;
  }
  constexpr const auto reserved_end() const
  {
    return begin() + N;
  }

private:
  ty m_arr[N]{};
  size_t m_size{ 0u };
};

template <size_t N>
class fixed_string : public vector<char, N>
{
private:
  using base = vector<char, N>;

public:
  constexpr fixed_string()
  {
    algo::fill(base::begin(), base::reserved_end(), '\0');
  }

  template <typename... ts>
  constexpr fixed_string(ts... chars)
  {
    static_assert(traits::all_true<std::is_same<ts, char>...>);

    const auto list = { chars... };
    for(const auto c : list)
    {
      base::push_back(c);
    }
  }
};


template <typename... ts>
fixed_string(ts... args) -> fixed_string<sizeof...(ts) + 1>;

using string = fixed_string<10u>;

template <typename T, T... chars>
constexpr auto operator"" _s()
{
  return fixed_string{ chars... };
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

    while(val > 0)
    {
      result.push_back(static_cast<char>(val % 10 + '0'));
      val /= 10;
    }

    algo::reverse(result.begin(), result.end());

    return result;
  }

  constexpr size_t stoui(string str)
  {
    size_t result{ 0u };

    for(const char c : str)
    {
        result *= 10;
        result += static_cast<size_t>(c - '0');
    }

    return result;
  }
}

namespace tokens
{
  constexpr auto exit = "exit"_s;
  constexpr auto mov = "mov"_s;
  constexpr auto sub = "sub"_s;
  constexpr auto add = "add"_s;
  constexpr auto cmp = "cmp"_s;
  constexpr auto je = "je"_s;
  constexpr auto jmp = "jmp"_s;
  constexpr auto inc = "inc"_s;

  constexpr auto comma = ","_s;
  constexpr auto open_square_bracket = "["_s;
  constexpr auto close_square_bracket = "]"_s;
  constexpr auto plus = "+"_s;

  constexpr auto eax = "eax"_s;
  constexpr auto ebx = "ebx"_s;
  constexpr auto ecx = "ecx"_s;
  constexpr auto edx = "edx"_s;

  constexpr auto esp = "esp"_s;
  constexpr auto ebp = "ebp"_s;
}

template <size_t tokens_count>
class tokenizer
{
public:
  using tokens_array_t = vector<string, tokens_count>;

  template <typename string_t>
  constexpr auto tokenize(string_t str) const
  {
    tokens_array_t tokens;
    auto iter = str.begin();

    for(size_t i = 0u; i < tokens_count; ++i)
    {
      auto token = get_token(iter);
      tokens.push_back(token);

      algo::advance(iter, token.size() + 1);
    }

    return tokens;
  }

private:
  constexpr string get_token(const char* ptr) const
  {
    if(*ptr == '\0')
    {
      return {};
    }
    else
    {
      string str;

      while(*ptr != ' ' && *ptr != '\0')
      {
        str.push_back(*ptr++);
      }

      return str;
    }
  }
};

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

int main()
{
  constexpr auto tokens_count = algo::count(asm_code.begin(), asm_code.end(), ' ') + 1;
  constexpr tokenizer<tokens_count> ams_tokenizer;
  constexpr auto tokens = ams_tokenizer.tokenize(asm_code);

  //constexpr auto labels_count = algo::count(asm_code.begin(), asm_code.end(), ':');
  //constexpr labels::labels_extractor<labels_count> labels_extractor;
  //constexpr auto extracted_labels_metadata = labels_extractor.extract(tokens);

  return tokens.size();
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

  namespace fixed_string_tests
  {
    constexpr auto zero = algo::to_string(0u);
    constexpr auto one = algo::to_string(1u);
    constexpr auto twelve = algo::to_string(12u);
    constexpr auto big = algo::to_string(147129847u);

    constexpr auto expected_zero = "0"_s;
    constexpr auto expected_one = "1"_s;
    constexpr auto expected_twelve = "1"_s;
    constexpr auto expected_big = "147129847"_s;

    static_assert(zero == expected_zero);
    static_assert(one == expected_one);
  }

  namespace tokenize
  {
    constexpr auto text = "a b c"_s;
    constexpr auto splt = tokenizer<3u>{};

    static_assert(splt.tokenize(text).size() == 3);

    constexpr auto tokens = splt.tokenize(text);
    constexpr auto a = "a"_s;
    constexpr auto b = "b"_s;
    constexpr auto c = "c"_s;
    static_assert(tokens[0] == a);
    static_assert(tokens[1] == b);
    static_assert(tokens[2] == c);
  }
}

