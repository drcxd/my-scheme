#pragma once

#include <string_view>
#include <vector>

#include "Token.hh"

class Lexer {

  std::string_view _source;

  // line starts from 1 while column from 0, Emacs convention
  int line = 1;
  int column = 0;

  // position in the source
  int position = 0;

  // Last token we have returned
  Token lastToken;

  auto peekChar() const -> char;
  auto peekNextChar() const -> char;
  auto consumeChar() -> char;
  auto matchChar(char c) const -> bool;

  // Consume a string
  auto readString() -> int;

  auto skipWhitespaceAndComments() -> char;

  // Read to next delimiter
  auto readUntilNextDelimiter() -> int;

  auto figureTokenType(std::string_view lexeme) -> Token::Type;

public:
  Lexer(std::string_view source);

  auto next() -> Token;
  auto peek() -> Token;
  auto hasNext() const -> bool;

  auto getAllTokens() -> std::vector<Token>;
};
