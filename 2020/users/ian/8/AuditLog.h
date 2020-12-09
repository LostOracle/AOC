#pragma once

#include "State.h"
class AuditLog
{
  public:
  std::vector<State> log;
  void audit(State s)
  {
    log.push_back(s);
  }

  void printLog()
  {
    for(const auto& record : log)
    {
      printf("pc: %d\n", record.pc);
      printf("acc: %d\n",record.acc);
    }
  }
};
