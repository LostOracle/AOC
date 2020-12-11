#include "regex_error.h"

void print_regex_error(const std::regex_error& e)
{
  std::string err;
  if(e.code() == std::regex_constants::error_collate)
  {
    err = "The expression contained an invalid collating element name.";
  }
  else if(e.code() == std::regex_constants::error_ctype)
  {
    err = "The expression contained an invalid character class name.";
  }
  else if(e.code() == std::regex_constants::error_escape)
  {
    err = "The expression contained an invalid escaped character, or a trailing escape.";
  }
  else if(e.code() == std::regex_constants::error_backref)
  {
    err = "The expression contained an invalid back reference.";
  }
  else if(e.code() == std::regex_constants::error_brack)
  {
    err = "The expression contained mismatched brackets ([ and ]).";
  }
  else if(e.code() == std::regex_constants::error_paren)
  {
    err = " The expression contained mismatched parentheses (( and )).";
  }
  else if(e.code() == std::regex_constants::error_brace)
  {
    err = "The expression contained mismatched braces ({ and }).";
  }
  else if(e.code() == std::regex_constants::error_badbrace)
  {
    err = "The expression contained an invalid range between braces ({ and }).";
  }
  else if(e.code() == std::regex_constants::error_range)
  {
    err = "The expression contained an invalid character range.";
  }
  else if(e.code() == std::regex_constants::error_space)
  {
    err = "There was insufficient memory to convert the expression into a finite state machine.";
  }
  else if(e.code() == std::regex_constants::error_badrepeat)
  {
    err = "The expression contained a repeat specifier (one of *?+{) that was not preceded by a valid regular expression.";
  }
  else if(e.code() == std::regex_constants::error_complexity)
  {
    err = "The complexity of an attempted match against a regular expression exceeded a pre-set level.";
  }
  else if(e.code() == std::regex_constants::error_stack)
  {
    err = "There was insufficient memory to determine whether the regular expression could match the specified character sequence.";
  }
  else
  {
    err = "Other";
  }
  printf("regex_error: %s\n",err.c_str());
}
