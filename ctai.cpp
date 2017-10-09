#include <cstddef>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace algo
{
  template <typename It, typename OutIt>
  constexpr void copy(It first, It last, OutIt d_first)
  {
    while (first != last) {
          *d_first++ = *first++;
      }
  }

  template <typename It, typename Value>
  constexpr void fill(It first, It last, Value value)
  {
    while (first != last) {
          *first++ = value;
      }
  }

  template <typename It, typename Value>
  constexpr size_t count(It first, It last, Value value)
  {
    size_t ret{ 0u };
    while (first != last) {
      ret += *first++ == value;
    }
    return ret;
  }
}

template <typename T, size_t N>
class array
{
public:
  using value_type = T;

  constexpr array()
    : arr{}
  {}

  template <typename... Ts>
  constexpr array(Ts... args) 
  {
    const auto list = { args... };
    algo::copy(list.begin(), list.end(), begin());
  }


  constexpr const T& operator[](size_t i) const
  {
      return arr[i];
  }

  constexpr T& operator[](size_t i)
  {
      return arr[i];
  }

  constexpr T* begin()
  {
      return arr;
  }
  constexpr T* end()
  {
      return arr + N;
  }

  constexpr const T* cbegin() const
  {
      return arr;
  }
  constexpr const T* cend() const
  {
      return arr + N;
  }

private:
  T arr[N];
};

template <typename... Ts>
array(Ts... args) -> array<std::common_type_t<Ts...>, 
                           sizeof...(Ts)>;

template <typename T, T... chars>
constexpr auto operator""_s()
{
    return array{chars...};
}

class small_string
{
public:
  constexpr small_string()
    : m_arr{}
    , m_size{ 0u }
  {
    algo::fill(m_arr.begin(), m_arr.end(), '\0');
  }

  constexpr void push_back(char c)
  {
    m_arr[m_size++] = c;
  }

  constexpr size_t size() const
  {
    return m_size;
  }

private:
  static constexpr auto k_max_size = 10;

  array<char, k_max_size> m_arr;
  size_t m_size;
};

template <size_t tokens_count>
class splitter
{
public:
  using tokens_array_t = array<small_string, tokens_count>;

  template <typename string_t>
  constexpr auto split(string_t string) const
  {
    tokens_array_t tokens;
    auto ptr = string.begin();

    for(size_t i = 0u; i < tokens_count; ++i)
    {
      const auto token = get_token(ptr);
      ptr += token.size();
      tokens[i] = token;
    }

    return tokens;
  }

private:
  small_string get_token(const char* ptr) const
  {
    if(*ptr == '\0')
      return {};
    else
    {
      small_string str;

      while(*ptr != ' ' && *ptr != '\0')
        str.push_back(*ptr++);

      return str;
    }
  }
};

constexpr auto asm_code = "exit"_s;


int main()
{
  constexpr auto tokens_count = algo::count(asm_code.cbegin(), asm_code.cend(), ' ');
  constexpr splitter<tokens_count> ams_tokenizer;
  constexpr auto tokens = ams_tokenizer.split(asm_code);

  return asm_code[0];
}