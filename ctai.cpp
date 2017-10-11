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

  template <typename it>
  constexpr void reverse(it first, it last)
  {
    while ((first != last) && (first != --last)) 
    {
        std::iter_swap(first++, last);
    }
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
  
  template <typename it>
  constexpr small_string(it b, it e)
    : m_arr{}
    , m_size{ 0u }
  {
    algo::copy(b, e, begin());
  }

  constexpr void push_back(char c)
  {
    m_arr[m_size++] = c;
  }

  constexpr size_t size() const
  {
    return m_size;
  }

  constexpr char front() const
  {
    return m_arr[0];
  }

  constexpr char* begin()
  {
    return m_arr.begin();
  }

  constexpr char* end()
  {
    return m_arr.end();
  }

  template <size_t rhs_N>
  constexpr bool operator==(const array<char, rhs_N>& rhs) const
  {
    return rhs_N == m_size && 
           algo::equal(m_arr.cbegin(), m_arr.cbegin() + m_size, rhs.cbegin());
  }

private:
  static constexpr auto k_max_size = 10;

  array<char, k_max_size> m_arr;
  size_t m_size;
};

namespace algo
{
  constexpr small_string to_string(size_t number)
  {
    small_string result;

    while(number > 0)
    {
      result.push_back(static_cast<char>(number + '0'));
      number /= 10;
    }

    algo::reverse(result.begin(), result.end());

    return result;
  }
}

template <typename T, size_t N>
class small_vector
{
public:
  constexpr small_vector()
  {}

  constexpr void push_back(T val)
  {
    m_arr[m_size++] = val;
  }

  constexpr size_t size() const
  {
    return m_size;
  }

private:
  array<T, N> m_arr;
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
  constexpr small_string get_token(const char* ptr) const
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
  array<size_t, AmountOfRAM> ram;
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebp;
  uint32_t esp;
};

namespace tokens
{
  constexpr auto exit = "exit"_s;
  constexpr auto mov = "mov"_s;
  constexpr auto sub = "sub"_s;
  constexpr auto add = "add"_s;
  constexpr auto jge = "jge"_s;
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

namespace instructions
{
  enum instruction
  {
    none,

    jge,
    jmp,
    add_reg_mem,
    sub_reg_val,
    mov_mem_reg_ptr_reg_plus_val,
    mov_reg_mem_ptr_reg_plus_val,
    inc,
    exit,

    instruction_count
  };

  constexpr size_t get_ip_change(instruction ints)
  {
    switch(ints)
    {
      case jge: return 2u;
      case jmp: return 2u;
      case add_reg_mem: return 3u;
      case sub_reg_val: return 3u;
      case mov_mem_reg_ptr_reg_plus_val: return 4u;
      case mov_reg_mem_ptr_reg_plus_val: return 4u;
      case inc: return 2u;
      case exit: return 1u;

      default: return 0u;
    }
  }

  constexpr size_t get_max_eip_change()
  {
    size_t max{ 0u };

    for(size_t instruction_opcode_val = 0u; 
        instruction_opcode_val < instruction::instruction_count; 
        ++instruction_opcode_val)
    {
      const auto change = get_ip_change(static_cast<instruction>(instruction_opcode_val));
      if(change > max)
      {
        max = change;
      }
    }

    return max;
  }

  template <typename tokens_it>
  constexpr auto get_next_instruction(tokens_it token_it)
  {
    const auto token = *token_it;
    if(token == tokens::jge) return instruction::jge;
    if(token == tokens::jmp) return instruction::jmp;
    if(token == tokens::add) return instruction::add_reg_mem;
    if(token == tokens::sub) return instruction::sub_reg_val;
    if(token == tokens::inc) return instruction::inc;
    if(token == tokens::exit) return instruction::exit;
    if(token == tokens::mov)
    {
      if(*std::next(token_it) == tokens::open_square_bracket)
      {
        return instruction::mov_mem_reg_ptr_reg_plus_val;
      }
      else
      {
        return instruction::mov_reg_mem_ptr_reg_plus_val;
      }
    }
    else return instruction::none;
  }
}

namespace labels
{
  struct label_metadata
  {
    constexpr label_metadata()
      : name{}
      , ip{ 0 }
    {}

    constexpr label_metadata(small_string name, size_t ip)
      : name{ name }
      , ip{ ip }
    {}

    small_string name;
    size_t ip;
  };

  template <typename token_t>
  constexpr auto label_name_from_token(token_t token)
  {
    const auto name_begin = std::next(token.begin());
    const auto name_end = token.end();
    return small_string{ name_begin, name_end };
  }

  template <size_t labels_count, typename tokens_t>
  constexpr auto extract_labels(tokens_t tokens)
  {
    small_vector<label_metadata, labels_count> labels;
    size_t ip{ 0u };
    auto tokens_cp = tokens;
    auto end = tokens_cp.end();

    auto current_token_it = tokens_cp.begin();
    while(current_token_it != end)
    {
      if(current_token_it->front() == ':')
      {
        const auto name = label_name_from_token(*current_token_it);

        label_metadata metadata(name, ip);
        labels.push_back(metadata);

        std::advance(current_token_it, 1);
      }
      else
      {
        const auto instruction = instructions::get_next_instruction(current_token_it);
        const auto ip_change = instructions::get_ip_change(instruction);

        std::advance(current_token_it, ip_change);
        ip += ip_change;
      }
    }

    return labels;
  }

  template <typename labels_t, typename token_t>
  size_t get_label_ip(labels_t labels, token_t label_token)
  {
    const auto label_name = label_name_from_token(label_token);
    for(const auto label_metadata : labels)
    {
      if(label_metadata.name == label_name)
      {
        return label_metadata.ip;
      }
    }

    return static_cast<size_t>(-1);
  }

  template <typename tokens_t, typename labels_metadata_t, size_t result_tokens_size>
  constexpr auto substitute_labels(tokens_t tokens, labels_metadata_t labels)
  {
    small_vector<small_string, result_tokens_size> result_tokens;
    auto end = tokens.end();

    auto current_token_it = tokens.begin();
    while(current_token_it != end)
    {
      if(current_token_it->front() == ':')
      {
        std::advance(current_token_it, 1);
      }
      else if(current_token_it->front() == '.')
      {
        const auto ip = get_label_ip(labels, *current_token_it);
        const auto str_ip = algo::to_string(ip);
        result_tokens.push_back(str_ip);

        std::advance(current_token_it, 1);
      }
      else
      {
        const auto instruction = instructions::get_next_instruction(current_token_it);
        const auto ip_change = instructions::get_ip_change(instruction);

        std::advance(current_token_it, ip_change);
      }
    }

    return result_tokens;
  }
}

namespace assemble
{
  template <typename token_t>
  constexpr auto token_to_opcodes(token_t token)
  {
    using opcodes_t = array<size_t, instructions::get_max_eip_change()>;

    opcodes_t opcodes;
    algo::fill(opcodes.begin(), opcodes.end(), instructions::instruction::none);

    if(token == tokens::exit)
    {
      opcodes[0] = instructions::instruction::exit;
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

constexpr auto asm_code = 
    "mov ebp , esp "
    "sub esp , 4 "
    "mov [ ebp + 2 ] , 0 "
    "mov [ ebp + 3 ] , 1 "
    "mov [ ebp + 4 ] , 1 "
    "mov [ ebp + 1 ] , 1 "
    "mov ecx , 1 "
":loop_label "
    "cmp ecx , 15 " //we want to get 15th fibonacci element
    "jge .end_label "
    "mov eax , [ ebp + 3 ] "
    "add eax , [ ebp + 2 ] "
    "mov [ ebp + 4 ] , eax "
    "mov eax , [ ebp + 3 ] "
    "mov [ ebp + 2 ] , eax "
    "mov eax , [ ebp + 4 ] "
    "mov [ ebp + 3 ] , eax "
    "mov eax , [ ebp + 1 ] "
    "inc ecx "
    "jmp .loop_label "
":end_label "
    "mov eax , [ ebp + 4 ] "
    "exit"_s;


int main()
{
  constexpr auto tokens_count = algo::count(asm_code.cbegin(), asm_code.cend(), ' ');
  constexpr splitter<tokens_count> ams_tokenizer;
  constexpr auto tokens = ams_tokenizer.split(asm_code);
  constexpr auto labels_count = algo::count(asm_code.cbegin(), asm_code.cend(), ':');
  constexpr auto labels_metadata = labels::extract_labels<labels_count, decltype(tokens)>(tokens);

  constexpr assemble::assembler<128> assembler;
  //constexpr auto m = assembler.assemble_tokens(tokens);

  return tokens_count;
}