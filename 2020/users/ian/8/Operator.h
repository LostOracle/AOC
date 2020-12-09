#pragma once

#include "State.h"

class Operator
{
  public:
  virtual State compute(const State& before, int argument) = 0;
};

class NOP: public Operator
{
  public:
  State compute(const State& before, int /*argument*/) override
  {
    State after = before;
    after.pc += 1;
    return after;
  }
};

class ACC: public Operator
{
  public:
  State compute(const State& before, int argument) override
  {
    State after = before;
    after.acc += argument;
    after.pc += 1;
    return after;
  }
};

class JMP: public Operator
{
  public:
  State compute(const State& before, int argument) override
  {
    State after = before;
    after.pc += argument;
    return after;
  }
};
