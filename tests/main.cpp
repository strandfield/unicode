// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'unicode' library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "unicode/characters.h"
#include "unicode/utf8.h"

#include <iostream>
#include <stdexcept>
#include <string>

void raise(const std::string& str, int line)
{
  std::cerr << __FILE__ << ":" << line << ": " << str << std::endl;
  throw std::runtime_error{ "Assertion failure" };
}

#define REQUIRE(x) do { if(!(x)) raise(#x, __LINE__); }while(false)

int main(int argc, char* argv[])
{
  using namespace unicode;

  std::string str = "a";
  str += Utf8Char{ chars::GREEK_CAPITAL_LETTER_PI }.data();

  REQUIRE(utf8::length(str) == 2);

  auto begin = utf8::begin(str);
  auto end = utf8::end(str);
  
  auto it = begin;
  REQUIRE(*it == 'a');
  ++it;
  REQUIRE(*it == chars::GREEK_CAPITAL_LETTER_PI);
  ++it;
  REQUIRE(it == end);

  it = end;
  --it;
  REQUIRE(*it == chars::GREEK_CAPITAL_LETTER_PI);
  --it;
  REQUIRE(*it == 'a');
  REQUIRE(it == begin);
}
