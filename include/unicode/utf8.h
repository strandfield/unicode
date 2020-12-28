// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'unicode' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef UNICODE_UTF8_H
#define UNICODE_UTF8_H

#include "unicode.h"

#include <array>
#include <cassert>
#include <string>

namespace unicode
{

inline size_t utf8_codepoint_length(const char* str)
{
  auto shifted = [](char c, int n) -> int
  {
    return static_cast<unsigned char>(c) >> n;
  };

  if (!shifted(*str, 7))
    return 1;
  else if (shifted(*str, 5) == 0x6)
    return 2;
  else if (shifted(*str, 4) == 0xE)
    return 3;
  else if (shifted(*str, 3) == 0x1E)
    return 4;
  else
    return 0;
}

inline bool read_utf8_char(std::string::const_iterator begin, std::string::const_iterator end, std::string::const_iterator& output)
{
  auto shifted = [](std::string::const_iterator it, int n) -> int
  {
    return static_cast<unsigned char>(*it) >> n;
  };

  if (!shifted(begin, 7))
  {
    output = begin + 1;
    return true;
  }
  else if (shifted(begin, 5) == 0x6)
  {
    if (std::distance(begin, end) < 2)
      return false;

    ++begin;

    if (shifted(begin, 6) == 0x2)
    {
      output = begin + 1;
      return true;
    }
  }
  else if (shifted(begin, 4) == 0xE)
  {
    if (std::distance(begin, end) < 3)
      return false;

    ++begin;

    if (shifted(begin, 6) == 0x2 && shifted(begin+1, 6) == 0x2)
    {
      output = begin + 2;
      return true;
    }
  }
  else if (shifted(begin, 3) == 0x1E)
  {
    if (std::distance(begin, end) < 4)
      return false;

    ++begin;

    if (shifted(begin, 6) == 0x2 && shifted(begin+1, 6) == 0x2 && shifted(begin+2, 6) == 0x2)
    {
      output = begin + 3;
      return true;
    }
  }

  return false;
}

template<typename T>
inline Character read_utf8_char(T& begin)
{
  auto shifted = [](T it, int n) -> int
  {
    return static_cast<unsigned char>(*it) >> n;
  };

  if (!shifted(begin, 7))
  {
    return *(begin++);
  }
  else if (shifted(begin, 5) == 0x6)
  {
    Character ret = *(begin++) & 0x1F;
    assert(shifted(begin, 6) == 0x2);
    ret = (ret << 6) | (*(begin++) & 0x3F);
    return ret;
  }
  else if (shifted(begin, 4) == 0xE)
  {
    Character ret = *(begin++) & 0xF;
    assert(shifted(begin, 6) == 0x2);
    ret = (ret << 6) | (*(begin++) & 0x3F);
    assert(shifted(begin, 6) == 0x2);
    ret = (ret << 6) | (*(begin++) & 0x3F);
    return ret;
  }
  else if (shifted(begin, 3) == 0x1E)
  {
    Character ret = *(begin++) & 0x7;
    assert((*begin >> 6) == 0x2);
    ret = (ret << 6) | (*(begin++) & 0x3F);
    assert((*begin >> 6) == 0x2);
    ret = (ret << 6) | (*(begin++) & 0x3F);
    assert((*begin >> 6) == 0x2);
    ret = (ret << 6) | (*(begin++) & 0x3F);
    return ret;
  }
  else
  {
    assert(false);
    return 0;
  }
}

inline bool is_utf8_char(const std::string& str)
{
  std::string::const_iterator it = str.end();
  return read_utf8_char(str.begin(), str.end(), it) && it == str.end();
}

inline bool is_utf8_string(const std::string& str)
{
  std::string::const_iterator it = str.begin();

  while (it != str.end())
  {
    if (!read_utf8_char(it, str.end(), it))
    {
      return false;
    }
  }

  return true;
}

class Utf8Char
{
private:
  std::array<char, 5> m_chars;

public:
  explicit Utf8Char(Character c)
  {
    if (c <= 0x007F)
    {
      m_chars[0] = c;
      m_chars[1] = 0;
      m_chars[2] = 0;
      m_chars[3] = 0;
      m_chars[4] = 3;
    }
    else if (c <= 0x07FF)
    {
      m_chars[0] = (3 << 6) | ((c >> 6) & 0x3F);
      m_chars[1] = (1 << 7) | (c & 0x3F);
      m_chars[2] = 0;
      m_chars[3] = 0;
      m_chars[4] = 2;
    }
    else if (c <= 0xFFFF)
    {
      m_chars[0] = (7 << 5) | ((c >> 12) & 0x3F);
      m_chars[1] = (1 << 7) | ((c >> 6) & 0x3F);
      m_chars[2] = (1 << 7) | (c & 0x3F);
      m_chars[3] = 0;
      m_chars[4] = 1;
    }
    else
    {
      m_chars[0] = (15 << 4) | ((c >> 18) & 0x3F);
      m_chars[1] = (1 << 7) | ((c >> 12) & 0x3F);
      m_chars[2] = (1 << 7) | ((c >> 6) & 0x3F);
      m_chars[3] = (1 << 7) | (c & 0x3F);
      m_chars[4] = 0;
    }
  }

  const char* data() const { return m_chars.data(); }
  size_t size() const { return 4 - m_chars[4]; }
};

class Utf8Iterator
{
private:
  const char* m_str;

public:
  Utf8Iterator(const char* str) : m_str(str) { }
  Utf8Iterator(const Utf8Iterator&) = default;
  ~Utf8Iterator() = default;

  const char* str() const { return m_str; }

  Character operator*() const
  {
    const char* s = m_str;
    return read_utf8_char<const char*>(s);
  }

  Utf8Iterator& operator++()
  {
    read_utf8_char<const char*>(m_str);
    return *(this);
  }

  Utf8Iterator operator++(int)
  {
    Utf8Iterator copy{ *this };
    read_utf8_char<const char*>(m_str);
    return copy;
  }

  Utf8Iterator& operator--()
  {
    auto is_middle_byte = [](char c) -> bool {
      return (static_cast<unsigned char>(c) >> 6) == 2;
    };

    while (is_middle_byte(*(--m_str)));

    return *(this);
  }

  Utf8Iterator operator--(int)
  {
    Utf8Iterator copy{ *this };
    --(*this);
    return copy;
  }
};

inline bool operator==(const Utf8Iterator& lhs, const Utf8Iterator& rhs)
{
  return lhs.str() == rhs.str();
}

inline bool operator!=(const Utf8Iterator& lhs, const Utf8Iterator& rhs)
{
  return lhs.str() != rhs.str();
}

namespace utf8
{

inline size_t codepoint_length(const char* str)
{
  return unicode::utf8_codepoint_length(str);
}

inline Utf8Iterator begin(const char* str)
{
  return Utf8Iterator(str);
}

inline Utf8Iterator end(const char* str)
{
  return Utf8Iterator(str);
}

inline Utf8Iterator begin(const std::string& str)
{
  return Utf8Iterator(str.c_str());
}

inline Utf8Iterator end(const std::string& str)
{
  return Utf8Iterator(str.c_str() + str.size());
}

inline size_t length(const std::string& str)
{
  Utf8Iterator it = utf8::begin(str);
  Utf8Iterator end = utf8::end(str);

  size_t result = 0;

  while (it != end)
    ++result, ++it;

  return result;
}

} // namespace utf8

} // namespace unicode

#endif // UNICODE_UTF8_H
