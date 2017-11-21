#include <cstddef>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace traits
{
  template <typename... types>
  constexpr auto all = std::conjunction<types...>::value;
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
  constexpr it_t next(it_t iterator, size_t n = 1)
  {
    return iterator + n;
  }

  template <typename it_t>
  constexpr it_t prev(it_t iterator, size_t n = 1)
  {
    return iterator - n;
  }

  template <typename it_t>
  constexpr void advance(it_t &iterator, size_t n = 1)
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

template <typename T, size_t N>
class vector
{
public:
  using value_type = T;
  using iterator = T*;

  constexpr vector()
    : m_arr{}
    , m_size{ 0u }
  {}

  constexpr char front() const
  {
    return *begin();
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
    return begin() + m_size;
  }

  constexpr const auto end() const
  {
    return begin() + m_size;
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

  constexpr void resize_to_reserved()
  {
    m_size = N;
  }

  template <size_t rhs_n>
  constexpr bool operator==(const vector<T, rhs_n>& rhs) const
  {
    return size() == rhs.size() &&
           algo::equal(begin(), end(), rhs.begin());
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
  T m_arr[N];
  size_t m_size;
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
    static_assert(traits::all<std::is_same<ts, char>...>, "wrong types");

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

namespace regs
{
  using reg_t = uint32_t;

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
    constexpr auto to_size_t(reg_t r)
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


namespace instructions
{
  enum instruction
  {
    none,

    je,
    jmp,
    cmp,
    add_reg_mem_ptr_reg_plus_val,
    sub_reg_val,
    mov_mem_reg_ptr_reg_plus_val,
    mov_mem_val_ptr_reg_plus_val,
    mov_reg_mem_ptr_reg_plus_val,
    mov_reg_reg,
    mov_reg_val,
    inc,
    exit,

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
    constexpr label_metadata()
      : name{}
      , ip{ 0u }
    {}

    constexpr label_metadata(string name, size_t ip)
      : name{ name }
      , ip{ ip }
    {}

    string name;
    size_t ip;
  };

  template <typename token_t>
  constexpr string label_name_from_token(token_t token)
  {
    auto it = algo::next(token.begin());
    string name;

    //todo cleanup
    while(*it != '\0')
    {
      name.push_back(*it++);
    }

    return name;
  }

  template <size_t labels_count, typename tokens_t>
  constexpr auto extract_labels(tokens_t tokens)
  {
    vector<label_metadata, labels_count> labels;
    size_t ip{ 0u };
    auto tokens_cp = tokens;
    auto end = tokens_cp.end();

    auto current_token_it = tokens_cp.begin();
    while(current_token_it != end)
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

  template <typename labels_t, typename token_t>
  constexpr size_t get_label_ip(labels_t labels, token_t label_token)
  {
    const auto pred = [label_name = label_name_from_token(label_token)](const auto& label_metadata)
    {
      return label_name == label_metadata.name;
    };

    const auto found = algo::find_if(labels.begin(), labels.end(), pred);

    return found == labels.end()
           ? static_cast<size_t>(-1)
           : found->ip;
  }

  template <typename tokens_t, typename labels_metadata_t, size_t result_tokens_size>
  constexpr auto substitute_labels(tokens_t tokens, labels_metadata_t labels)
  {
    vector<string, result_tokens_size> result_tokens;
    
    const auto end = tokens.end();

    auto current_token_it = tokens.begin();
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
}

template <size_t amount_of_ram>
class machine
{
public:
  using reg_type = regs::reg_t;

  constexpr machine()
    : ram{}
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

  vector<size_t, amount_of_ram> ram;

  constexpr reg_type& eax() { return reg_ref(regs::reg::eax); }
  constexpr const reg_type& eax() const { return reg_ref(regs::reg::eax); }
  constexpr reg_type& ebx() { return reg_ref(regs::reg::ebx); }
  constexpr const reg_type& ebx() const { return reg_ref(regs::reg::ebx); }
  constexpr reg_type& ecx() { return reg_ref(regs::reg::ecx); }
  constexpr const reg_type& ecx() const { return reg_ref(regs::reg::ecx); }
  constexpr reg_type& edx() { return reg_ref(regs::reg::edx); }
  constexpr const reg_type& edx() const { return reg_ref(regs::reg::edx); }
  constexpr reg_type& ebp() { return reg_ref(regs::reg::ebp); }
  constexpr const reg_type& ebp() const { return reg_ref(regs::reg::ebp); }
  constexpr reg_type& esp() { return reg_ref(regs::reg::esp); }
  constexpr const reg_type& esp() const { return reg_ref(regs::reg::esp); }
  constexpr reg_type& eip() { return reg_ref(regs::reg::eip); }
  constexpr const reg_type& eip() const { return reg_ref(regs::reg::eip); }

  bool zf{false};

private:
  constexpr void init_regs()
  {
    eax() = ebx() = ecx() = edx() = ebp() = esp() = eip() = 0u;
  }

  template <typename reg_t>
  constexpr reg_type& reg_ref(reg_t r)
  {
      return regs_vals[regs::to_size_t(r)];
  }
  template <typename reg_t>
  constexpr const reg_type& reg_ref(reg_t r) const
  {
      return regs_vals[regs::to_size_t(r)];
  }

  vector<reg_type, static_cast<size_t>(regs::reg::undef)> regs_vals{};
};


namespace assemble
{
  template <typename token_it_t>
  constexpr auto get_next_opcodes(token_it_t &token_it)
  {
    using opcodes_t = vector<size_t, instructions::get_max_eip_change()>;

    opcodes_t opcodes;
    algo::fill(opcodes.begin(), opcodes.end(), instructions::instruction::none);
    
    const auto token = *token_it;
    const auto instruction = instructions::get_next_instruction(token_it);

    opcodes.push_back(instruction);

    switch(instruction)
    {
        case instructions::instruction::exit: //exit
        break;

        case instructions::instruction::je: // je pointer
        {
            const auto ip = algo::stoui(*algo::next(token_it));
            opcodes.push_back(ip);
        }break;

        case instructions::instruction::jmp: // jmp pointer
        {
            const auto ip = algo::stoui(*algo::next(token_it));
            opcodes.push_back(ip);
        }break;

        case instructions::instruction::add_reg_mem_ptr_reg_plus_val: // add reg , [ reg2 + val ]
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it));
            const auto reg2 = regs::token_to_reg(*algo::next(token_it, 4));
            const auto val = algo::stoui(*algo::next(token_it, 6));

            opcodes.push_back(regs::to_size_t(reg));
            opcodes.push_back(regs::to_size_t(reg2));
            opcodes.push_back(val);
        }break;

        case instructions::instruction::sub_reg_val: // sub reg , val
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it));
            const auto val = algo::stoui(*algo::next(token_it, 3));

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
            const auto val = algo::stoui(*algo::next(token_it, 3));

            opcodes.push_back(regs::to_size_t(reg));
            opcodes.push_back(val);
        }break;

        case instructions::instruction::mov_mem_reg_ptr_reg_plus_val: // mov [ reg + val ] , reg2
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it, 2));
            const auto val = algo::stoui(*algo::next(token_it, 4));
            const auto reg2 = regs::token_to_reg(*algo::next(token_it, 7));

            opcodes.push_back(regs::to_size_t(reg));
            opcodes.push_back(val);
            opcodes.push_back(regs::to_size_t(reg2));
        }break;

        case instructions::instruction::mov_mem_val_ptr_reg_plus_val: // mov [ reg + val ] , val2
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it, 2));
            const auto val = algo::stoui(*algo::next(token_it, 4));
            const auto val2 = algo::stoui(*algo::next(token_it, 7));

            opcodes.push_back(regs::to_size_t(reg));
            opcodes.push_back(val);
            opcodes.push_back(val2);
        }break;

        case instructions::instruction::mov_reg_mem_ptr_reg_plus_val: // mov reg , [ reg2 + val ]
        {
            const auto reg = regs::token_to_reg(*algo::next(token_it));
            const auto reg2 = regs::token_to_reg(*algo::next(token_it, 4));
            const auto val = algo::stoui(*algo::next(token_it, 6));

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
            const auto val = algo::stoui(*algo::next(token_it, 3));

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
      machine<AmountOfRAM> m;

      auto opcodes_dest = m.ram.begin();

      auto token_it = tokens.begin();
      while(token_it != tokens.end())
      {
        auto opcodes = get_next_opcodes(token_it);
        algo::copy(opcodes.begin(), opcodes.end(), opcodes_dest);
        algo::advance(opcodes_dest, opcodes.size());
      }
      
      m.esp() = AmountOfRAM - 1;
      m.eip() = 0;

      return m;
    }
  };
}

namespace execute
{
    template <typename machine_t>
    constexpr auto get_next_instruction(machine_t& machine)
    {
        return static_cast<instructions::instruction>(machine.ram[machine.eip()]);
    }

    template <typename machine_t>
    constexpr bool execute_next_instruction(machine_t& machine)
    {
        const auto instruction = get_next_instruction(machine);

        switch(instruction)
        {
            case instructions::instruction::je: // je pointer
            {
                if(machine.zf)
                {
                    const auto new_ip = machine.ram[machine.eip() + 1];
                    machine.eip() = new_ip;
                    return false;
                }
            }break;

            case instructions::instruction::jmp: // jmp pointer
            {
                const auto new_ip = machine.ram[machine.eip() + 1];
                machine.eip() = new_ip;
                return false;
            }break;

            case instructions::instruction::add_reg_mem_ptr_reg_plus_val: // add reg reg2 val
            {
                const auto reg = machine.ram[machine.eip() + 1];
                const auto reg_val = machine.get_reg(reg);
                const auto reg2_val = machine.get_reg(machine.ram[machine.eip() + 2]);
                const auto val = machine.ram[machine.eip() + 3];

                const auto mem_ptr = reg2_val + val;
                const auto val_to_add = machine.ram[mem_ptr];

                const auto new_reg_val = reg_val + val_to_add;
                machine.set_reg(reg, new_reg_val);
            }break;

            case instructions::instruction::sub_reg_val: // sub reg val
            {
                const auto reg = machine.ram[machine.eip() + 1];
                const auto reg_val = machine.get_reg(reg);
                const auto val = machine.ram[machine.eip() + 2];

                const auto new_reg_val = reg_val - val;
                machine.set_reg(reg, new_reg_val);
            }break;

            case instructions::instruction::inc: // inc reg
            {
                const auto reg = machine.ram[machine.eip() + 1];
                const auto reg_val = machine.get_reg(reg);

                machine.set_reg(reg, reg_val + 1);
            }break;

            case instructions::instruction::cmp: // cmp reg val
            {
                const auto reg = machine.ram[machine.eip() + 1];
                const auto reg_val = machine.get_reg(reg);
                const auto val = machine.ram[machine.eip() + 2];

                machine.zf = reg_val == val;
            }break;

            case instructions::instruction::mov_mem_reg_ptr_reg_plus_val: // mov [ reg + val ] , reg2
            {
                const auto reg = machine.ram[machine.eip() + 1];
                const auto reg_val = machine.get_reg(reg);
                const auto val = machine.ram[machine.eip() + 2];
                const auto reg2 = machine.ram[machine.eip() + 3];
                const auto reg2_val = machine.get_reg(reg2);

                const auto mem_ptr = reg_val + val;
                machine.ram[mem_ptr] = reg2_val;
            }break;

            case instructions::instruction::mov_mem_val_ptr_reg_plus_val: // mov [ reg + val ] , val2
            {
                const auto reg = machine.ram[machine.eip() + 1];
                const auto reg_val = machine.get_reg(reg);
                const auto val = machine.ram[machine.eip() + 2];
                const auto val2 = machine.ram[machine.eip() + 3];

                const auto mem_ptr = reg_val + val;
                machine.ram[mem_ptr] = val2;
            }break;

            case instructions::instruction::mov_reg_mem_ptr_reg_plus_val: // mov reg , [ reg2 + val ]
            {
                const auto reg = machine.ram[machine.eip() + 1];
                const auto reg2 = machine.ram[machine.eip() + 2];
                const auto reg2_val = machine.get_reg(reg2);
                const auto val = machine.ram[machine.eip() + 3];

                const auto mem_ptr = reg2_val + val;
                const auto new_reg_val = machine.ram[mem_ptr];
                machine.set_reg(reg, new_reg_val);
            }break;

            case instructions::instruction::mov_reg_reg: // mov reg , reg2
            {
                const auto reg = machine.ram[machine.eip() + 1];
                const auto reg2 = machine.ram[machine.eip() + 2];
                const auto reg2_val = machine.get_reg(reg2);

                machine.set_reg(reg, reg2_val);
            }break;

            case instructions::instruction::mov_reg_val: // mov reg , val
            {
                const auto reg = machine.ram[machine.eip() + 1];
                const auto val = machine.ram[machine.eip() + 2];

                machine.set_reg(reg, val);
            }break;

            default:
            break;
        }

        return true;
    }

    template <typename machine_t>
    constexpr void adjust_eip(machine_t& machine)
    {
        const auto instruction = get_next_instruction(machine);
        const auto eip_change = instructions::get_ip_change(instruction);
        machine.eip() += eip_change;
    }

    template <typename machine_t>
    constexpr auto execute(machine_t machine)
    {
        while(get_next_instruction(machine) != instructions::instruction::exit)
        {
            const auto need_to_change_eip = execute_next_instruction(machine);
            if(need_to_change_eip)
            {
                adjust_eip(machine);
            }
        }

        return machine.eax();
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

int main()
{
  constexpr auto tokens_count = algo::count(asm_code.begin(), asm_code.end(), ' ') + 1;
  constexpr tokenizer<tokens_count> ams_tokenizer;
  constexpr auto tokens = ams_tokenizer.tokenize(asm_code);

  constexpr auto labels_count = algo::count(asm_code.begin(), asm_code.end(), ':');
  constexpr auto labels_metadata = labels::extract_labels<labels_count, decltype(tokens)>(tokens);
  constexpr auto extracted_labels_metadata = labels::extract_labels<tokens_count, decltype(tokens)>(tokens);
  constexpr auto substitued_labels = labels::substitute_labels<decltype(tokens), decltype(extracted_labels_metadata), tokens_count>(tokens, extracted_labels_metadata);

  constexpr assemble::assembler<1024> assembler;
  constexpr auto m = assembler.assemble_tokens(substitued_labels);

  constexpr auto result = execute::execute(m);

  return result;
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

  namespace labels_tests
  {
    constexpr auto text = ":begin "
                          "mov eax , 1 "
                          ":middle "
                          "mov eax , 1 "
                          ":end"_s;
    constexpr auto tokens_count = algo::count(text.begin(), text.end(), ' ') + 1;
    constexpr tokenizer<tokens_count> ams_tokenizer;
    constexpr auto tokens = ams_tokenizer.tokenize(text);
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

  namespace substitute_tests
  {
    constexpr auto text = ":begin "
                          "je .begin "
                          "mov eax , 1 "
                          ":middle "
                          "je .middle "
                          "mov eax , 1 "
                          ":end "
                          "je .end"_s;
    constexpr auto tokens_count = algo::count(text.begin(), text.end(), ' ') + 1;
    constexpr tokenizer<tokens_count> ams_tokenizer;
    constexpr auto tokens = ams_tokenizer.tokenize(text);
    constexpr auto extracted_labels = labels::extract_labels<tokens_count, decltype(tokens)>(tokens);
    constexpr auto substitued_labels = labels::substitute_labels<decltype(tokens), decltype(extracted_labels), tokens_count>(tokens, extracted_labels);

    constexpr auto begin_label_ip_str = "0"_s;
    constexpr auto middle_label_ip_str = "5"_s;
    constexpr auto end_label_ip_str = "10"_s;

    static_assert(substitued_labels[1] == begin_label_ip_str);
    static_assert(substitued_labels[7] == middle_label_ip_str);
    static_assert(substitued_labels[13] == end_label_ip_str);
  }
}
