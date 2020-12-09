#pragma once

#include "State.h"

class Operator
{
  public:
  virtual State compute(const State& before, int argument) = 0;
  auto clone() const { return std::unique_ptr<Operator>(clone_impl()); }
  virtual Operator* clone_impl() const = 0;
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
  NOP* clone_impl() const override { return new NOP(*this); }
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
  ACC* clone_impl() const override { return new ACC(*this); }
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
  JMP* clone_impl() const override { return new JMP(*this); }
};

class ADD: public Operator
{
  public:
  State compute(const State& before, int argument) override
  {
    State after = before;
    after.pc += argument;
    return after;
  }
  JMP* clone_impl() const override { return new JMP(*this); }
};
