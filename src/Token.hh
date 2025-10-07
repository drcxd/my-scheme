#pragma once

#include <string>
#include <string_view>
#include <format>

struct Token {
  enum class Type {
    None, // default value
    LParen, // (
    RParen, // )
    Dot, // .
    Quote, // '
    Boolean, // #t and #f
    Number, // 42, 3.14
    String, // "abc\n"
    Symbol, // xyz, +, -, *, /, ->string
    End, // end of file
  };

  Type type = Type::None;
  std::string_view lexeme;
  int line = 0;
  int column = 0;
};

template <>
struct std::formatter<Token::Type> : std::formatter<std::string> {
  auto format(Token::Type type, std::format_context& ctx) const {
    std::string str;
    switch (type) {
    case Token::Type::None: str = "None"; break;
    case Token::Type::LParen: str = "LParen"; break;
    case Token::Type::RParen: str = "RParen"; break;
    case Token::Type::Dot: str = "Dot"; break;
    case Token::Type::Quote: str = "Quote"; break;
    case Token::Type::Boolean: str = "Boolean"; break;
    case Token::Type::Number: str = "Number"; break;
    case Token::Type::String: str = "String"; break;
    case Token::Type::Symbol: str = "Symbol"; break;
    case Token::Type::End: str = "EOF"; break;
    }
    return std::formatter<std::string>::format(str, ctx);
  }
};

auto toString(const Token& token) -> std::string;
