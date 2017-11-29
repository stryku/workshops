#include <cstddef>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <array>

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
    return m_arr.end();
  }
  constexpr auto reserved_end() const
  {
    return m_arr.end();
  }

private:
  std::array<ty, n> m_arr{};
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

  template <typename string_t>
  constexpr auto tokenize(string_t str) const
  {
    using tokens_t = vector<string, tokens_count>;

    tokens_t tokens;
    auto iter = str.begin();

    for(size_t i = 0u; i < tokens_count; ++i)
    {
      auto token = get_token(iter);
      tokens.push_back(token);

      algo::advance(iter, token.size() + 1u); //+1 to omit space
    }

    return tokens;
  }

private:
  constexpr auto get_token(const char* ptr) const
  {
    string str;

    while(*ptr != ' ' && *ptr != '\0')
    {
      str.push_back(*ptr++);
    }

    return str;
  }
};

namespace regs
{
  using reg_t = unit_t;

  enum class reg
  {
      eax,
      ebx,
      ecx,
      edx,
      ebp,
      esp,
      eip,

      undef
  };

  template <typename reg_t>
  constexpr auto to_unit_t(reg_t r)
  {
      return static_cast<unit_t>(r);
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

namespace instructions
{
  enum instruction
  {
    none,

    je,                           // je ip
    jmp,                          // jmp ip
    cmp,                          // cmp reg , val
    add_reg_mem_ptr_reg_plus_val, // add reg , [ reg2 + val ]
    sub_reg_val,                  // sub reg , val
    mov_mem_reg_ptr_reg_plus_val, // mov [ reg + val ] , reg2
    mov_mem_val_ptr_reg_plus_val, // mov [ reg + val ] , val2
    mov_reg_mem_ptr_reg_plus_val, // mov reg , [ reg2 + val ]
    mov_reg_reg,                  // mov reg , reg2
    mov_reg_val,                  // mov reg , val
    inc,                          // inc reg
    exit,                         // exit

    instruction_count
  };

  constexpr size_t get_ip_change(instruction inst)
  {
    switch(inst)
    {
      case je: return 2u;                           // je ip
      case jmp: return 2u;                          // jmp ip
      case cmp: return 3u;                          // cmp reg val
      case add_reg_mem_ptr_reg_plus_val: return 4u; // add reg reg2 val
      case sub_reg_val: return 3u;                  // sub reg val
      case mov_mem_reg_ptr_reg_plus_val: return 4u; // mov reg val reg2
      case mov_mem_val_ptr_reg_plus_val: return 4u; // mov reg val val2
      case mov_reg_mem_ptr_reg_plus_val: return 4u; // mov reg reg2 val
      case mov_reg_reg: return 3u;                  // mov reg reg2
      case mov_reg_val: return 3u;                  // mov reg val
      case inc: return 2u;                          // inc reg
      case exit: return 1u;                         // exit

      default: return 0u;
    }
  }

  constexpr size_t get_token_count(instruction inst)
  {
    switch(inst)
    {
      case je: return 2u;                           // je ip
      case jmp: return 2u;                          // jmp ip
      case cmp: return 4u;                          // cmp reg , val
      case add_reg_mem_ptr_reg_plus_val: return 8u; // add reg , [ reg2 + val ]
      case sub_reg_val: return 4u;                  // sub reg , val
      case mov_mem_reg_ptr_reg_plus_val: return 8u; // mov [ reg + val ] , reg2
      case mov_mem_val_ptr_reg_plus_val: return 8u; // mov [ reg + val ] , val2
      case mov_reg_mem_ptr_reg_plus_val: return 8u; // mov reg , [ reg2 + val ]
      case mov_reg_reg: return 4u;                  // mov reg , reg2
      case mov_reg_val: return 4u;                  // mov reg , val
      case inc: return 2u;                          // inc reg
      case exit: return 1u;                         // exit

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

  constexpr auto is_register(string token)
  {
    return token == tokens::eax ||
      token == tokens::ebx ||
      token == tokens::ecx ||
      token == tokens::edx ||
      token == tokens::ebp ||
      token == tokens::esp;
  }

  template <typename token_it_t>
  constexpr auto get_next_instruction(token_it_t token_it)
  {
    if(auto token = *token_it; token == tokens::je) return instruction::je;
    else if(token == tokens::jmp) return instruction::jmp;
    else if(token == tokens::add) return instruction::add_reg_mem_ptr_reg_plus_val;
    else if(token == tokens::sub) return instruction::sub_reg_val;
    else if(token == tokens::inc) return instruction::inc;
    else if(token == tokens::exit) return instruction::exit;
    else if(token == tokens::cmp) return instruction::cmp;
    else if(token == tokens::mov)
    {
      auto next_token = *algo::next(token_it);

      if(next_token == tokens::open_square_bracket) // mov [
      {
        auto token_after_comma = *algo::next(token_it, 7);
        if(is_register(token_after_comma)) 
        {
          return instruction::mov_mem_reg_ptr_reg_plus_val;// mov [ reg + val ] , reg2
        }
        else
        {
          return instruction::mov_mem_val_ptr_reg_plus_val;// mov [ reg + val ] , val2
        }
      }
      else if(is_register(next_token)) // mov reg
      {
        auto token_after_comma = *algo::next(token_it, 3);

        if(is_register(token_after_comma))
        {
          return instruction::mov_reg_reg; // mov reg , reg2
        }
        else if(token_after_comma == tokens::open_square_bracket)
        {
          return instruction::mov_reg_mem_ptr_reg_plus_val; //mov reg , [ reg2 + val ]
        }
        else
        {
          return instruction::mov_reg_val; // mov reg , val
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
    constexpr label_metadata() = default;

    constexpr label_metadata(string name, size_t ip)
      : name{ name }
      , ip{ ip }
    {}

    string name{};
    size_t ip{ 0u };
  };

  template <typename token_t>
  constexpr string label_name_from_token(token_t token)
  {
    auto it = algo::next(token.begin());
    string name;

    while(*it != '\0')
    {
      name.push_back(*it++);
    }

    return name;
  }

  template <size_t labels_count>
  class labels_extractor
  {
  public:
    template <typename tokens_t>
    constexpr auto extract(tokens_t tokens) const
    {
      vector<label_metadata, labels_count> labels;
      size_t ip{ 0u };
      auto current_token_it = tokens.begin();

      while(current_token_it != tokens.end())
      {
        if(current_token_it->front() == ':')
        {
          auto name = label_name_from_token(*current_token_it);

          labels.push_back(label_metadata(name, ip));

          algo::advance(current_token_it);
        }
        else
        {
          const auto instruction = instructions::get_next_instruction(current_token_it);

          const auto token_count = instructions::get_token_count(instruction);
          algo::advance(current_token_it, token_count);

          const auto ip_change = instructions::get_ip_change(instruction);
          ip += ip_change;
        }
      }

      return labels;
    }
  };

  template <typename labels_t, typename token_t>
  constexpr size_t get_label_ip(token_t token, labels_t labels)
  {
    const auto pred = [label_name = label_name_from_token(token)](const auto& label_metadata)
    {
      return label_name == label_metadata.name;
    };

    const auto found = algo::find_if(labels.begin(), labels.end(), pred);

    return found == labels.end()
           ? static_cast<size_t>(-1)
           : found->ip;
  }

  template <size_t result_tokens_size>
  class labels_replacer
  {
  public:
    template <typename tokens_t, typename labels_metadata_t>
    constexpr auto replace(tokens_t tokens, labels_metadata_t labels) const
    {
      using result_tokens_t = vector<string, result_tokens_size>;

      result_tokens_t result_tokens;

      for(const auto& token : tokens)
      {
        if(token.front() == ':') 
        {
          //Label declaration. Omit it
        }
        else if(token.front() == '.')
        {
          //Label reference. Replace with instruciton pointer

          const auto label_ip = get_label_ip(token, labels);
          const auto string_ip = algo::to_string(label_ip);

          result_tokens.push_back(string_ip);
        }
        else
        {
          //Regular token
          result_tokens.push_back(token);
        }
      }

      return result_tokens;
    }
  };
}

template <size_t amount_of_ram>
class machine
{
public:
  using reg_t = unit_t;

  constexpr machine()
  {
    ram.resize_to_reserved();
    regs_vals.resize_to_reserved();
    init_regs();
  }

  constexpr machine(const machine& rhs)
    : ram{ rhs.ram }
    , regs_vals{ rhs.regs_vals }
  {}

  template <typename reg_t>
  constexpr reg_t get_reg(reg_t r)
  {
    return reg_ref(r);
  }

  template <typename reg_t>
  constexpr void set_reg(reg_t r, reg_t val)
  {
    reg_ref(r) = val;
  }

  constexpr decltype(auto) eax() { return reg_ref(regs::reg::eax); }
  constexpr decltype(auto) eax() const { return reg_ref(regs::reg::eax); }
  constexpr decltype(auto) ebx() { return reg_ref(regs::reg::ebx); }
  constexpr decltype(auto) ebx() const { return reg_ref(regs::reg::ebx); }
  constexpr decltype(auto) ecx() { return reg_ref(regs::reg::ecx); }
  constexpr decltype(auto) ecx() const { return reg_ref(regs::reg::ecx); }
  constexpr decltype(auto) edx() { return reg_ref(regs::reg::edx); }
  constexpr decltype(auto) edx() const { return reg_ref(regs::reg::edx); }
  constexpr decltype(auto) ebp() { return reg_ref(regs::reg::ebp); }
  constexpr decltype(auto) ebp() const { return reg_ref(regs::reg::ebp); }
  constexpr decltype(auto) esp() { return reg_ref(regs::reg::esp); }
  constexpr decltype(auto) esp() const { return reg_ref(regs::reg::esp); }
  constexpr decltype(auto) eip() { return reg_ref(regs::reg::eip); }
  constexpr decltype(auto) eip() const { return reg_ref(regs::reg::eip); }

  vector<unit_t, amount_of_ram> ram;
  bool zf{false};

private:
  constexpr void init_regs()
  {
    eax() = ebx() = ecx() = edx() = ebp() = esp() = eip() = 0u;
  }

  template <typename register_t>
  constexpr decltype(auto) reg_ref(register_t r)
  {
    return regs_vals[static_cast<size_t>(r)];
  }

  template <typename register_t>
  constexpr decltype(auto) reg_ref(register_t r) const
  {
    return regs_vals[static_cast<size_t>(r)];
  }

  vector<reg_t, static_cast<size_t>(regs::reg::undef)> regs_vals{};
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
  tokenizer<tokens_count> ams_tokenizer;
  auto tokens = ams_tokenizer.tokenize(asm_code);

  constexpr auto labels_count = algo::count(asm_code.begin(), asm_code.end(), ':');
  labels::labels_extractor<labels_count> labels_extractor;
  auto extracted_labels_metadata = labels_extractor.extract(tokens);
  labels::labels_replacer<tokens_count> labels_replacer;
  auto tokens_replaced_labels = labels_replacer.replace(tokens, extracted_labels_metadata);

  return 0;
}
