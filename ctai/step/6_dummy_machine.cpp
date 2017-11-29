enum reg
{
  eax
};

class machine
{
public:
  int& eax{ m_regs[reg::eax] };

private:
  int m_regs[1]{};
};

constexpr auto execute(machine m)
{
  m.eax = 22;
  return m.eax;
}

int main()
{
  constexpr machine m{};

  //constexpr auto result = execute(m);

  return 0;
}