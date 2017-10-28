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
      const auto val = *first;
      ++first;
          *d_first++ = val;
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
      //todo
      auto tmp = *first;
      *first = *last;
      ++first;
      *last = tmp;
        //std::iter_swap(first++, last);
    }
  }

  template <typename it>
  constexpr it next(it iterator, size_t n = 1)
  {
    return iterator + n;
  }

  template <typename it>
  constexpr it prev(it iterator, size_t n = 1)
  {
    return iterator - n;
  }

  template <typename it>
  constexpr void advance(it &iterator, size_t n = 1)
  {
    iterator += n;
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

  constexpr array(const array& rhs)
    : arr{}
  {
    for(size_t i = 0u; i < N; ++i)
    {
      arr[i] = rhs[i];
    }
    //algo::copy(rhs.cbegin(), rhs.cend(), begin());
  }

  constexpr array& operator=(const array& rhs)
  {
    for(size_t i = 0u; i < N; ++i)
    {
      arr[i] = rhs[i];
    }
    return *this;
  }

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

  constexpr auto size() const
  {
      return N;
  }

  template <typename rhs_t, size_t rhs_N>
  constexpr bool operator==(const array<rhs_t, rhs_N>& rhs) const
  {
    return std::is_same<T, rhs_t>::value && 
           rhs_N == N && 
           algo::equal(cbegin(), cend(), rhs.cbegin());
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
    return array{chars..., '\0'};
}

namespace tests
{
    namespace _s_tests
    {
        constexpr auto arr = "arr"_s;

        static_assert(arr.size() == 4u);
        static_assert(arr[0] == 'a');
        static_assert(arr[1] == 'r');
        static_assert(arr[2] == 'r');
        static_assert(arr[3] == '\0');
    }
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

  constexpr small_string(const small_string& rhs)
    : m_arr{}
    , m_size{ rhs.m_size }
  {
    m_arr = rhs.m_arr;
  }

  constexpr small_string& operator=(const small_string& rhs)
  {
    m_size = rhs.size();
    m_arr = rhs.m_arr;
    return *this;
  }
  
  template <typename it>
  constexpr small_string(it b, it e)
    : m_arr{}
    , m_size{ static_cast<size_t>(e - b) }
  {
    for(size_t i = 0u; i < m_size; ++i)
    {
      m_arr[i] = *b++;
    }
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

  constexpr const char* begin() const
  {
    return m_arr.cbegin();
  }

  constexpr char* end()
  {
    return begin() + m_size;
  }

  constexpr const char* end() const
  {
    return begin() + m_size;
  }

  constexpr char& operator[](size_t i)
  {
    return m_arr[i];
  }

  constexpr const char& operator[](size_t i) const
  {
    return m_arr[i];
  }

  constexpr size_t to_uint() const
  {
      size_t result{ 0u };

      for(const char c : *this)
      {
          result += static_cast<size_t>(c - '0');
          result *= 10;
      }

      return result;
  }

  template <size_t rhs_N>
  constexpr bool operator==(const array<char, rhs_N>& rhs) const
  {
    return rhs_N - 1 == m_size && // -1 because we assume that array has '\0' at the end
           algo::equal(m_arr.cbegin(), m_arr.cbegin() + m_size, rhs.cbegin());
  }

  constexpr bool operator==(const small_string& rhs) const
  {
    return m_size == rhs.m_size &&
           m_arr == rhs.m_arr;
  }

private:
  static constexpr auto k_max_size = 10;

  array<char, k_max_size> m_arr;
  size_t m_size;
};



template <typename obj>
constexpr int debug_object(obj object)
{
  return object[999922];
}

namespace algo
{
  constexpr small_string to_string(size_t number)
  {
    small_string result;

    if(number == 0u)
    {
      small_string str;
      str.push_back('0');
      return str;
    }

    while(number > 0)
    {
      result.push_back(static_cast<char>(number % 10 + '0'));
      number /= 10;
    }

    algo::reverse(result.begin(), result.end());

    return result;
  }
}

namespace tests
{
  namespace small_string_tests
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
}

template <typename T, size_t N>
class small_vector
{
public:
  constexpr small_vector()
    : m_size{ 0u }
  {}

  constexpr auto begin()
  {
    return m_arr.begin();
  }

  constexpr auto cbegin() const
  {
    return m_arr.cbegin();
  }

  constexpr auto end()
  {
    return begin() + m_size;
  }

  constexpr auto cend() const
  {
    return cbegin() + m_size;
  }

  constexpr void push_back(T val)
  {
    m_arr[m_size++] = val;
  }

  constexpr size_t size() const
  {
    return m_size;
  }

  constexpr T& operator[](size_t i)
  {
    return m_arr[i];
  }

  constexpr const T& operator[](size_t i) const
  {
    return m_arr[i];
  }

private:
  array<T, N> m_arr;
  size_t m_size;
};

template <size_t tokens_count>
class splitter
{
public:
  using tokens_array_t = small_vector<small_string, tokens_count>;

  template <typename string_t>
  constexpr auto split(string_t string) const
  {
    tokens_array_t tokens;
    auto ptr = string.begin();

    for(size_t i = 0u; i < tokens_count; ++i)
    {
      auto token = get_token(ptr);
      ptr += token.size() + 1;
      tokens.push_back(token);
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

namespace tests
{
  namespace split
  {
    constexpr auto text = "a b c"_s;
    constexpr auto splt = splitter<3u>{};

    static_assert(splt.split(text).size() == 3);

    constexpr auto tokens = splt.split(text);
    constexpr auto a = "a"_s;
    constexpr auto b = "b"_s;
    constexpr auto c = "c"_s;
    static_assert(tokens[0] == a);
    static_assert(tokens[1] == b);
    static_assert(tokens[2] == c);
  }
}



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

namespace regs
{
    enum class reg
    {
        eax,
        ebx,
        ecx,
        edx,
        ebp,
        esp,

        undef
    };

    constexpr auto to_size_t(reg r)
    {
        return static_cast<size_t>(r);
    }

    template <typename token_t>
    constexpr reg token_to_reg(token_t token)
    {
        if(token == tokens::eax) return reg::eax;
        if(token == tokens::ebx) return reg::ebx;
        if(token == tokens::ecx) return reg::ecx;
        if(token == tokens::edx) return reg::edx;
        if(token == tokens::ebp) return reg::ebp;
        if(token == tokens::esp) return reg::esp;

        return reg::undef;
    }
}

template <size_t AmountOfRAM>
struct machine
{
  constexpr machine()
    : ram{}
    , eax{ reg_ref(regs::reg::eax) }
    , ebx{ reg_ref(regs::reg::ebx) }
    , ecx{ reg_ref(regs::reg::ecx) }
    , edx{ reg_ref(regs::reg::edx) }
    , ebp{ reg_ref(regs::reg::ebp) }
    , esp{ reg_ref(regs::reg::esp) }
  {}

  constexpr machine(const machine& rhs)
    : ram{ rhs.ram }
    , regs_vals{ rhs.regs_vals }
    , eax{ reg_ref(regs::reg::eax) }
    , ebx{ reg_ref(regs::reg::ebx) }
    , ecx{ reg_ref(regs::reg::ecx) }
    , edx{ reg_ref(regs::reg::edx) }
    , ebp{ reg_ref(regs::reg::ebp) }
    , esp{ reg_ref(regs::reg::esp) }
  {}

  constexpr machine& operator=(const machine& rhs)
  {
      ram = rhs.ram;
      regs_vals = rhs.regs_vals;
  }

  constexpr uint32_t get_reg(regs::reg r)
  {
      return reg_ref(r);
  }

  constexpr void set_reg(regs::reg r, uint32_t val)
  {
      reg_ref(r) = val;
  }

  array<size_t, AmountOfRAM> ram;

  uint32_t& eax;
  uint32_t& ebx;
  uint32_t& ecx;
  uint32_t& edx;
  uint32_t& ebp;
  uint32_t& esp;

  bool zf{false};

private:
  constexpr uint32_t& reg_ref(regs::reg r)
  {
      return regs_vals[regs::to_size_t(r)];
  }

  array<uint32_t, static_cast<size_t>(regs::reg::undef)> regs_vals{};
};

namespace instructions
{
  enum instruction
  {
    none,

    je,
    jmp,
    cmp,
    add_reg_mem,
    sub_reg_val,
    mov_mem_reg_ptr_reg_plus_val,
    mov_reg_mem_ptr_reg_plus_val,
    mov_reg_reg,
    mov_reg_val,
    inc,
    exit,

    instruction_count
  };

  constexpr size_t get_ip_change(instruction ints)
  {
    switch(ints)
    {
      case je: return 2u;
      case jmp: return 2u;
      case cmp: return 3u;
      case add_reg_mem: return 3u;
      case sub_reg_val: return 3u;
      case mov_mem_reg_ptr_reg_plus_val: return 4u;
      case mov_reg_mem_ptr_reg_plus_val: return 4u;
      case mov_reg_reg: return 3u;
      case mov_reg_val: return 3u;
      case inc: return 2u;
      case exit: return 1u;

      default: return 0u;
    }
  }

  constexpr size_t get_token_count(instruction ints)
  {
    switch(ints)
    {
      case je: return 2u;
      case jmp: return 2u;
      case cmp: return 4u;
      case add_reg_mem: return 8u;
      case sub_reg_val: return 4u;
      case mov_mem_reg_ptr_reg_plus_val: return 8u;
      case mov_reg_mem_ptr_reg_plus_val: return 8u;
      case mov_reg_reg: return 4u;
      case mov_reg_val: return 4u;
      case inc: return 2u;
      case exit: return 1u;

      default: return 500u;
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

  constexpr auto is_register(small_string token)
  {
    return token == tokens::eax ||
      token == tokens::ebx ||
      token == tokens::ecx ||
      token == tokens::edx ||
      token == tokens::ebp ||
      token == tokens::esp;
  }

  template <typename tokens_it>
  constexpr auto get_next_instruction(tokens_it token_it)
  {
    auto token = *token_it;
    if(token == tokens::je) return instruction::je;
    if(token == tokens::jmp) return instruction::jmp;
    if(token == tokens::add) return instruction::add_reg_mem;
    if(token == tokens::sub) return instruction::sub_reg_val;
    if(token == tokens::inc) return instruction::inc;
    if(token == tokens::exit) return instruction::exit;
    if(token == tokens::cmp) return instruction::cmp;
    if(token == tokens::mov)
    {
      auto next_token = *algo::next(token_it);

      if(next_token == tokens::open_square_bracket)
      {
        return instruction::mov_mem_reg_ptr_reg_plus_val;
      }
      else if(is_register(next_token))
      {
        auto token_after_comma = *algo::next(token_it, 3);

        if(is_register(token_after_comma))
        {
          return instruction::mov_reg_reg;
        }
        else if(token_after_comma == tokens::open_square_bracket)
        {
          return instruction::mov_reg_mem_ptr_reg_plus_val;
        }
        else
        {
          return instruction::mov_reg_val;
        }
      }
    }
    
    return instruction::none;
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
  constexpr small_string label_name_from_token(token_t token)
  {
    auto it = algo::next(token.begin());

    small_string name;

    while(*it != '\0')
    {
      name.push_back(*it++);
    }

    return name;
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
        auto name = label_name_from_token(*current_token_it);

        label_metadata metadata(name, ip);
        labels.push_back(metadata);

        algo::advance(current_token_it);
      }
      else
      {
        const auto instruction = instructions::get_next_instruction(current_token_it);
        const auto ip_change = instructions::get_ip_change(instruction);
        const auto token_count = instructions::get_token_count(instruction);

        algo::advance(current_token_it, token_count);

        ip += ip_change;
      }
    }

    return labels;
  }

  template <typename labels_t, typename token_t>
  constexpr size_t get_label_ip(labels_t labels, token_t label_token)
  {
    auto label_name = label_name_from_token(label_token);

    for(auto label_metadata : labels)
    {
      if(label_metadata.name == label_name)
      {
        return label_metadata.ip;
      }
    }

    return static_cast<size_t>(label_name[0]);
  }

  namespace tests
  {
    constexpr auto text = ":begin "
                          "mov eax , 1 "
                          ":middle "
                          "mov eax , 1 "
                          ":end"_s;
    constexpr auto tokens_count = algo::count(text.cbegin(), text.cend(), ' ') + 1;
    constexpr splitter<tokens_count> ams_tokenizer;
    constexpr auto tokens = ams_tokenizer.split(text);
    constexpr auto extracted_labels = labels::extract_labels<tokens_count, decltype(tokens)>(tokens);

    static_assert(extracted_labels.size() == 3);
    
    constexpr auto begin_label = "begin"_s;
    constexpr auto middle_label = "middle"_s;
    constexpr auto end_label = "end"_s;

    static_assert(extracted_labels[0].name == begin_label);
    static_assert(extracted_labels[1].name == middle_label);
    static_assert(extracted_labels[2].name == end_label);

    constexpr auto begin_label_ip = 0u;
    constexpr auto middle_label_ip = instructions::get_ip_change(instructions::instruction::mov_reg_val);
    constexpr auto end_label_ip =middle_label_ip + instructions::get_ip_change(instructions::instruction::mov_reg_val);
    static_assert(extracted_labels[0].ip == 0u);
    static_assert(extracted_labels[1].ip == middle_label_ip);
    static_assert(extracted_labels[2].ip == end_label_ip);
    
    constexpr auto begin_label_token = ".begin"_s;
    constexpr auto middle_label_token = ".middle"_s;
    constexpr auto end_label_token = ".end"_s;

    static_assert(get_label_ip(extracted_labels, begin_label_token) == 0u);
    static_assert(get_label_ip(extracted_labels, middle_label_token) == 3u);
    static_assert(get_label_ip(extracted_labels, end_label_token) == 6u);
  }

  

  template <typename tokens_t, typename labels_metadata_t, size_t result_tokens_size>
  constexpr auto substitute_labels(tokens_t tokens, labels_metadata_t labels)
  {
    small_vector<small_string, result_tokens_size> result_tokens;
    
    auto end = tokens.cend();

    auto current_token_it = tokens.cbegin();
    while(current_token_it != end)
    {
      if(current_token_it->front() == ':')
      {
        algo::advance(current_token_it);
      }
      else if(current_token_it->front() == '.')
      {
        const auto ip = get_label_ip(labels, *current_token_it);
        auto str_ip = algo::to_string(ip);
        result_tokens.push_back(str_ip);

        algo::advance(current_token_it);
      }
      else
      {
        result_tokens.push_back(*current_token_it++);
      }
    }

    return result_tokens;
  }

  namespace tests
  {
    namespace substitute
    {
      constexpr auto text = ":begin "
                            "je .begin "
                            "mov eax , 1 "
                            ":middle "
                            "je .middle "
                            "mov eax , 1 "
                            ":end "
                            "je .end"_s;
      constexpr auto tokens_count = algo::count(text.cbegin(), text.cend(), ' ') + 1;
      constexpr splitter<tokens_count> ams_tokenizer;
      constexpr auto tokens = ams_tokenizer.split(text);
      constexpr auto extracted_labels = labels::extract_labels<tokens_count, decltype(tokens)>(tokens);
      constexpr auto substitued_labels = substitute_labels<decltype(tokens), decltype(extracted_labels), tokens_count>(tokens, extracted_labels);

      constexpr auto begin_label_ip_str = "0"_s;
      constexpr auto middle_label_ip_str = "5"_s;
      constexpr auto end_label_ip_str = "10"_s;

      static_assert(substitued_labels[1] == begin_label_ip_str);
      static_assert(substitued_labels[7] == middle_label_ip_str);
      static_assert(substitued_labels[13] == end_label_ip_str);
    }
  }
}

/*
if(token == tokens::je) return instruction::je;
    if(token == tokens::jmp) return instruction::jmp;
    if(token == tokens::add) return instruction::add_reg_mem;
    if(token == tokens::sub) return instruction::sub_reg_val;
    if(token == tokens::inc) return instruction::inc;
    if(token == tokens::exit) return instruction::exit;
    if(token == tokens::cmp) return instruction::cmp;
    if(token == tokens::mov)
    {
      auto next_token = *algo::next(token_it);

      if(next_token == tokens::open_square_bracket)
      {
        return instruction::mov_mem_reg_ptr_reg_plus_val;
      }
      else if(is_register(next_token))
      {
        auto token_after_comma = *algo::next(token_it, 3);

        if(is_register(token_after_comma))
        {
          return instruction::mov_reg_reg;
        }
        else if(token_after_comma == tokens::open_square_bracket)
        {
          return instruction::mov_reg_mem_ptr_reg_plus_val;
        }
        else
        {
          return instruction::mov_reg_val;
        }
      }
    }
*/


namespace assemble
{
  template <typename token_it_t>
  constexpr auto get_next_opcodes(token_it_t &token_it)
  {
    using opcodes_t = small_vector<size_t, instructions::get_max_eip_change()>;

    opcodes_t opcodes;
    algo::fill(opcodes.begin(), opcodes.end(), instructions::instruction::none);
    
    auto token = *token_it;

    const auto instruction = instructions::get_next_instruction(token_it);

    opcodes.push_back(instruction);

    switch(instruction)
    {
        case instructions::instruction::exit: //exit
        break;

        case instructions::instruction::je: // je pointer
        {
            const auto ip = algo::next(token_it)->to_uint();
            opcodes.push_back(ip);
        }break;

        case instructions::instruction::jmp: // jmp pointer
        {
            const auto ip = algo::next(token_it)->to_uint();
            opcodes.push_back(ip);
        }break;

        case instructions::instruction::add_reg_mem: // add reg , [ reg2 + val ]
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it));
            const auto reg2 = regs::token_to_reg(*algo::next(token_it, 4));
            const auto val = algo::next(token_it, 6)->to_uint();

            opcodes.push_back(regs::to_size_t(reg));
            opcodes.push_back(regs::to_size_t(reg2));
            opcodes.push_back(val);
        }break;

        case instructions::instruction::sub_reg_val: // sub reg , val
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it));
            const auto val = algo::next(token_it, 3)->to_uint();

            opcodes.push_back(regs::to_size_t(reg));
            opcodes.push_back(val);
        }break;

        case instructions::instruction::inc: // inc reg
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it));
            opcodes.push_back(regs::to_size_t(reg));
        }break;

        case instructions::instruction::cmp: // cmp reg , val
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it));
            const auto val = algo::next(token_it, 3)->to_uint();

            opcodes.push_back(regs::to_size_t(reg));
            opcodes.push_back(val);
        }break;

        case instructions::instruction::mov_mem_reg_ptr_reg_plus_val: // mov [ reg + val ] , reg2
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it, 2));
            const auto val = algo::next(token_it, 4)->to_uint();
            const auto reg2 = regs::token_to_reg(*algo::next(token_it, 7));

            opcodes.push_back(regs::to_size_t(reg));
            opcodes.push_back(val);
            opcodes.push_back(regs::to_size_t(reg2));
        }break;

        case instructions::instruction::mov_reg_mem_ptr_reg_plus_val: // mov reg , [ reg2 + val ]
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it));
            const auto reg2 = regs::token_to_reg(*algo::next(token_it, 4));
            const auto val = algo::next(token_it, 6)->to_uint();

            opcodes.push_back(regs::to_size_t(reg));
            opcodes.push_back(regs::to_size_t(reg2));
            opcodes.push_back(val);
        }break;

        case instructions::instruction::mov_reg_reg: // mov reg , reg2
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it));
            const auto reg2 = regs::token_to_reg(*algo::next(token_it, 3));

            opcodes.push_back(regs::to_size_t(reg));
            opcodes.push_back(regs::to_size_t(reg2));
        }break;

        case instructions::instruction::mov_reg_val: // mov reg , val
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it));
            const auto val = algo::next(token_it, 3)->to_uint();

            opcodes.push_back(regs::to_size_t(reg));
            opcodes.push_back(val);
        }break;

        default:
        break;
    }

    const auto token_count = instructions::get_token_count(instruction);
    algo::advance(token_it, token_count);

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
      machine<AmountOfRAM> m;

      auto token_it = tokens.begin();
      while(token_it != tokens.cend())
      {
        auto opcodes = get_next_opcodes(token_it);
        for(auto opcode : opcodes)
        {
            m.ram[ip++] = opcode;//todo algo::copy
        }
      }
      
      m.esp = ip;

      return m;
    }
  };
}

namespace execute
{
    template <typename machine_t>
    constexpr auto get_next_instruction(machine_t& machine)
    {
        return static_cast<instructions::instruction>(machine.ram[machine.eip]);
    }

    template <typename machine_t>
    constexpr void execute_next_instruction(machine_t& machine)
    {
        const auto instruction = get_next_instruction(machine);

        switch(instruction)
        {
            case instructions::instruction::je: // je pointer
            {
                if(machine.sf == machine.of)
                {
                    const auto new_ip = machine.ram[machine.eip + 1];
                    machine.eip = new_ip;
                }
            }break;

            case instructions::instruction::jmp: // jmp pointer
            {
                const auto new_ip = machine.ram[machine.eip + 1];
                machine.eip = new_ip;
            }break;

            case instructions::instruction::add_reg_mem: // add reg reg2 val
            {
                const auto reg = machine.ram[machine.eip + 1];
                const auto reg_val = machine.get_reg(reg);
                const auto reg2_val = machine.get_reg(machine.ram[machine.eip + 2]);
                const auto val = machine.ram[machine.eip + 3];

                const auto mem_ptr = reg2_val + val;
                const auto val_to_add = machine.ram[mem_ptr];

                const auto new_reg_val = reg_val + val_to_add;
                machine.set_reg(reg, new_reg_val);
            }break;

            case instructions::instruction::sub_reg_val: // sub reg val
            {
                const auto reg = machine.ram[machine.eip + 1];
                const auto reg_val = machine.get_reg(reg);
                const auto val = machine.ram[machine.eip + 2];

                const auto new_reg_val = reg_val + val;
                machine.set_reg(reg, new_reg_val);
            }break;

            case instructions::instruction::inc: // inc reg
            {
                const auto reg = machine.ram[machine.eip + 1];
                const auto reg_val = machine.get_reg(reg);

                machine.set_reg(reg, reg_val + 1);
            }break;

            case instructions::instruction::cmp: // cmp reg val
            {
                const auto reg = machine.ram[machine.eip + 1];
                const auto reg_val = machine.get_reg(reg);
                const auto val = machine.ram[machine.eip + 2];

                machine.zf = reg_val == val;
            }break;

            case instructions::instruction::mov_mem_reg_ptr_reg_plus_val: // mov [ reg + val ] , reg2
            {
                const auto reg = machine.ram[machine.eip + 1];
                const auto reg_val = machine.get_reg(reg);
                const auto val = machine.ram[machine.eip + 2];
                const auto reg2 = machine.ram[machine.eip + 3];
                const auto reg2_val = machine.get_reg(reg2);

                const auto mem_ptr = reg_val + val;
                machine.ram[mem_ptr] = reg2_val;
            }break;

            case instructions::instruction::mov_reg_mem_ptr_reg_plus_val: // mov reg , [ reg2 + val ]
            {
                const auto reg = machine.ram[machine.eip + 1];
                const auto reg2 = machine.ram[machine.eip + 2];
                const auto reg2_val = machine.get_reg(reg2);
                const auto val = machine.ram[machine.eip + 3];

                const auto mem_ptr = reg2_val + val;
                const auto new_reg_val = machine.ram[mem_ptr];
                machine.set_reg(reg, new_reg_val);
            }break;

            case instructions::instruction::mov_reg_reg: // mov reg , reg2
            {
                const auto reg = machine.ram[machine.eip + 1];
                const auto reg2 = machine.ram[machine.eip + 2];
                const auto reg2_val = machine.get_reg(reg2);

                machine.set_reg(reg, reg2_val);
            }break;

            case instructions::instruction::mov_reg_val: // mov reg , val
            {
                const auto reg = machine.ram[machine.eip + 1];
                const auto val = machine.ram[machine.eip + 2];

                machine.set_reg(reg, val);
            }break;
        }
    }

    template <typename machine_t>
    constexpr auto execute(machine_t machine)
    {
        while(get_next_instruction(machine) != instructions::instruction::exit)
        {
            execute_next_instruction(machine);
        }

        return machine.eax;
    }
}

constexpr auto asm_code = 
    "mov ebp , esp "
    "sub esp , 4 "
    "mov [ ebp + 2 ] , 0 "
    "mov [ ebp + 3 ] , 1 "
    "mov [ ebp + 4 ] , 1 "
    "mov [ ebp + 1 ] , 1 "
    "mov ecx , 1 "
":loop "
    "cmp ecx , 15 " //we want to get 15th fibonacci element
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

constexpr auto asm_code2 = 
    "mov ebp , esp "
    "sub esp , 4 "
    "mov [ ebp + 2 ] , 0 "
    "mov [ ebp + 3 ] , 1 "
    "mov [ ebp + 4 ] , 1 "
    "mov [ ebp + 1 ] , 1 "
    "mov ecx , 1 "
":loop "
    "cmp ecx , 15 " //we want to get 15th fibonacci element
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

  constexpr auto tokens_count = algo::count(asm_code.cbegin(), asm_code.cend(), ' ') + 1;
  constexpr splitter<tokens_count> ams_tokenizer;
  constexpr auto tokens = ams_tokenizer.split(asm_code);


  constexpr auto labels_count = algo::count(asm_code.cbegin(), asm_code.cend(), ':');
  constexpr auto labels_metadata = labels::extract_labels<labels_count, decltype(tokens)>(tokens);
  constexpr auto extracted_labels_metadata = labels::extract_labels<tokens_count, decltype(tokens)>(tokens);
  constexpr auto substitued_labels = labels::substitute_labels<decltype(tokens), decltype(extracted_labels_metadata), tokens_count>(tokens, extracted_labels_metadata);

  constexpr assemble::assembler<1024> assembler;
  constexpr auto m = assembler.assemble_tokens(substitued_labels);

  //constexpr auto tmp = debug_object(machine.ram);


  return m.esp;
}