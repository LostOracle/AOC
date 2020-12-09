#pragma once

class State
{
  public:
  unsigned pc;
  int acc;
  int return_code;
  State() : pc(0),acc(0),return_code(0){};
};
