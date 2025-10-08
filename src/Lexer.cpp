#include "Lexer.hh"

#include <cassert>
#include <format>
#include <stdexcept>
#include <unordered_set>

static const std::unordered_set<char> ext_chars{
    '?', '!', '.', '+', '-', '*', '/', '<', '=',
    '>', ':', '$', '%', '^', '&', '_', '~',
};

static const std::unordered_set<char> delimiters{
    ' ',  '\t', '\r', '\n', // white spaces
    '(',  ')',              // parentheses
    '"',                    // double quote
    ';',                    // semi-colon
    // '#',                    // TODO: sharp
    '\'',                   // single quote
    // '`',                    // TODO: backward quote
    // ','                     // TODO: comma
};

static auto isDelimiter(char c) -> bool {
  return delimiters.contains(c);
}

static auto isWhitespace(char c) -> bool {
  return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

static auto isDigit(char c) -> bool {
  return '0' <= c && c <= '9';
}


static auto isDot(std::string_view lexeme) -> bool {
  return lexeme.length() == 1 && lexeme[0] == '.';
};

static auto isBoolean(std::string_view lexeme) -> bool {
  // we only allow #t or #f for now
  return lexeme == "#t" || lexeme == "#f";
};

static auto isNumber(std::string_view lexeme) -> bool {
  bool isNumber = lexeme.length() > 0;
  if (isNumber) {
    std::string_view n = lexeme;
    if (lexeme[0] == '+' || lexeme[0] == '-') {
      n = lexeme.substr(1);
    }
    isNumber = n.length() > 0;
    if (isNumber) {
      int dotCount = 0;
      for (auto c : n) {
        if (!isDigit(c) && c != '.') {
          isNumber = false;
          break;
        }
        if (c == '.') {
          ++dotCount;
        }
      }
      isNumber = isNumber && dotCount <= 1;
    }
  }
  return isNumber;
};

static auto throwError(std::string_view msg, int line, int column) {
  throw std::runtime_error(
      std::format("Error: {} at {}:{}", msg, line, column));
}

Lexer::Lexer(std::string_view source) : _source{source} {}

auto Lexer::next() -> Token {
  if (!hasNext()) {
    throw std::runtime_error("Fetching tokens on an exhausted stream.");
  }

  char thisChar = skipWhitespaceAndComments();

  int begin = position - 1;
  int end = begin;

  Token token;
  token.line = line;
  token.column = column;

  if (thisChar == EOF) {
    token.type = Token::Type::End;
  } else if (isDelimiter(thisChar)) {
    switch (thisChar) {
    case '(':
      token.type = Token::Type::LParen;
      ++end;
      break;
    case ')':
      token.type = Token::Type::RParen;
      ++end;
      break;
    case '\'':
      token.type = Token::Type::Quote;
      ++end;
      break;
    case '"':
      token.type = Token::Type::String;
      end = readString();
      break;
    default:
      // This should never happen
      assert(false);
      break;
    }
  } else {
    // read until the next delimiter
    end = readUntilNextDelimiter();
  }

  token.lexeme = _source.substr(begin, end - begin);

  if (token.type == Token::Type::None) {
    // figure out the type of this token
    token.type = figureTokenType(token.lexeme);
  }

  lastToken = token;
  return token;
}

auto Lexer::peek() -> Token {
  Lexer copy = *this;
  Token token = next();
  *this = copy;
  return token;
}

auto Lexer::hasNext() const -> bool {
  return lastToken.type != Token::Type::End;
}

auto Lexer::getAllTokens() -> std::vector<Token> {
  std::vector<Token> tokens;
  while (hasNext()) {
    tokens.push_back(next());
  }
  return tokens;
}

auto Lexer::peekChar() const -> char {
  return position < _source.length() ? _source[position] : EOF;
}

auto Lexer::peekNextChar() const -> char {
  return position < _source.length() - 1 ? _source[position + 1] : EOF;
}


auto Lexer::consumeChar() -> char {
  if (position >= _source.length()) {
    return EOF;
  }
  char c = _source[position++];
  if (c == '\n') {
    ++line;
    column = 0;
  } else {
    ++column;
  }
  return c;
}

auto Lexer::matchChar(char c) const -> bool {
  return peekChar() == c;
}

auto toString(const Token& token) -> std::string {
  return std::format("type: {}\tlexeme: {}\nline: {}\tcolumn: {}",
                     token.type, token.lexeme, token.line, token.column);
}

auto Lexer::readString() -> int {
  char c = 0;
  while (!matchChar('"') && c != EOF) {
    c = consumeChar();
  }

  if (c == EOF) {
    throwError("String not closed!", line, column);
  } else {
    // we encounter the closing double quote
    consumeChar();
  }

  return position;
}

auto Lexer::readUntilNextDelimiter() -> int {
  char c = peekChar();
  while (!isDelimiter(c) && c != EOF) {
    consumeChar();
    c = peekChar();
  }

  return position;
}

auto Lexer::figureTokenType(std::string_view lexeme) -> Token::Type {
  assert(lexeme.length() > 0);
  Token::Type type = Token::Type::None;
  if (isDot(lexeme)) {
    type = Token::Type::Dot;
  } else if (isBoolean(lexeme)) {
    type = Token::Type::Boolean;
  } else if (isNumber(lexeme)) {
    type = Token::Type::Number;
  } else {
    type = Token::Type::Symbol;
  }
  return type;
}

auto Lexer::skipWhitespaceAndComments() -> char {
  char c = consumeChar();
  while (isWhitespace(c) || c == ';') {
    if (isWhitespace(c)) {
      do {
        c = consumeChar();
      } while (isWhitespace(c));
    } else {
      do {
        c = consumeChar();
      } while (c != '\n');
    }
  }
  return c;
}
