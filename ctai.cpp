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

  template <typename It, typename It2>
  constexpr bool equal(It first, It last, It2 first2)
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

  template <typename rhs_t, size_t rhs_N>
  constexpr bool operator==(const array<rhs_t, N>& rhs) const
  {
    return std::is_same<T, rhs_t>::value && 
           rhs_N == N && 
           std::equal(cbegin(), cend(), rhs.cbegin());
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

  template <size_t rhs_N>
  constexpr bool operator==(const array<char, rhs_N>& rhs) const
  {
    return rhs_N == m_size && 
           std::equal(m_arr.cbegin(), m_arr.cbegin() + m_size, rhs.cbegin());
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

union reg_ex
{
  constexpr reg_ex()
    : ex{ 0 }
  {}

  uint32_t ex;
  uint16_t x;
};

union reg_exhl
{
  constexpr reg_exhl()
    : ex{ 0 }
  {}  

  uint32_t ex;
  uint16_t x;
  struct
  {
    uint8_t h;
    uint8_t l;
  } hl;
};

template <size_t AmountOfRAM>
struct machine
{
  array<uint8_t, AmountOfRAM> ram;
  reg_exhl a;
  reg_exhl b;
  reg_exhl c;
  reg_exhl d;
  reg_ex sp;
  reg_ex bp;
  reg_ex di;
  reg_ex si;
};

namespace tokens
{
  constexpr auto exit = "exit"_s;
}

namespace instructions
{
  enum opcode
  {
    none,

    exit,

    opcode_count
  };

  constexpr size_t get_ip_change(opcode instruction)
  {
    switch(instruction)
    {
      case exit: return 1u;

      default: return 0u;
    }
  }

  constexpr size_t get_max_eip_change()
  {
    size_t max{ 0u };

    for(size_t instruction_opcode_val = 0u; 
        instruction_opcode_val < opcode::opcode_count; 
        ++instruction_opcode_val)
    {
      const auto change = get_ip_change(static_cast<opcode>(instruction_opcode_val));
      if(change > max)
      {
        max = change;
      }
    }

    return max;
  }
}

namespace assemble
{
  template <typename token_t>
  constexpr auto token_to_opcodes(token_t token)
  {
    using opcodes_t = array<size_t, instructions::get_max_eip_change()>;

    opcodes_t opcodes;
    algo::fill(opcodes.begin(), opcodes.end(), instructions::opcode::none);

    if(token == tokens::exit)
    {
      opcodes[0] = instructions::opcode::exit;
    }

    return opcodes;
  }

  template <size_t AmountOfRAM>
  class assembler
  {
  public:
    template <typename tokens_t>
    constexpr auto assemble_tokens(tokens_t tokens) const
    {
      size_t ip{ 0 };

      for(const auto token : tokens)
      {
        const auto opcodes = token_to_opcodes(token);
      }

      return machine<AmountOfRAM>{};
    }
  };
}

int main()
{
  constexpr auto tokens_count = algo::count(asm_code.cbegin(), asm_code.cend(), ' ');
  constexpr splitter<tokens_count> ams_tokenizer;
  constexpr auto tokens = ams_tokenizer.split(asm_code);
  constexpr assemble::assembler<128> assembler;
  constexpr auto m = assembler.assemble_tokens(tokens);

  return m.a.ex;
}