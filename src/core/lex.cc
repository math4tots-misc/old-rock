#include "rock/core/all.hh"

#include <iostream>

#include <map>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

namespace rock {

std::vector<Token> lex(const File &f) {
  std::vector<Token> tokens;

  const std::set<std::string> SYMBOLS({
      "{", "}", "[", "]", "(", ")", "\n",
      "+", "-", "*", "/", "%",
      ".", ",", "\\", ":",
      "=",
      "==", "!=", "<", "<=", ">", ">=",
  });
  const std::unordered_set<std::string> KEYWORDS({
      "def", "fn", "return", "class", "var", "final", "super",
      "if", "then", "else", "switch", "case", "default",
      "while", "break", "continue",
      "for", "in",
      "is", "not", "and", "or",
  });
  const std::map<std::string, std::string> MATCHES({
    {"{", "}"},
    {"(", ")"},
    {"[", "]"},
  });
  const std::set<std::string> OPENERS({ "{", "(", "[" });
  const std::set<std::string> CLOSERS({ "}", ")", "]" });
  const std::string openBrace = "{";
  const std::string closeBrace = "}";

  int i = 0;
  std::vector<std::string> stack;
  const std::string &s(f.contents);

  for (;;) {
    while (i < static_cast<int>(s.size()) &&
           (s[i] == '#' ||
            (s[i] == '\n' && !(stack.empty() || stack.back() == openBrace)) ||
            (s[i] != '\n' && isspace(s[i])))) {
      if (s[i] == '#') {
        while (i < static_cast<int>(s.size()) && s[i] != '\n') {
          i++;
        }
      } else {
        i++;
      }
    }
    if (i >= static_cast<int>(s.size())) {
      break;
    }

    int j = i;

    // NEWLINE
    if (s[i] == '\n') {
      tokens.push_back(Token(f, i++, "NEWLINE", ""));
      continue;
    }

    // string
    {
      bool raw = false;
      if (j < static_cast<int>(s.size()) && s[j] == 'r') {
        raw = true;
        j++;
      }
      if (j < static_cast<int>(s.size()) && (s[j] == '"' || s[j] == '\'')) {
        std::string q(1, s[j]);
        if (s.substr(j, 3) == "\"\"\"" || s.substr(j, 3) == "'''") {
          q = s.substr(j, 3);
        }
        j += q.size();
        std::string buf;
        while (j < static_cast<int>(s.size()) && s.substr(j, q.size()) != q) {
          if (!raw && s[j] == '\\') {
            j++;
            if (j >= static_cast<int>(s.size())) {
              tokens.push_back(Token(
                  f, j, "ERROR",
                  "EOF on incomplete string escape"));
              return tokens;
            }
            switch (s[j]) {
            case 'n': buf.push_back('\n'); break;
            case 't': buf.push_back('\t'); break;
            case '\'': buf.push_back('\''); break;
            case '"': buf.push_back('"'); break;
            case '\\': buf.push_back('\\'); break;
            default:
              tokens.push_back(Token(
                  f, j, "ERROR",
                  "Invalid string escape: " + std::string(1, s[j])));
              return tokens;
            }
            j++;
          } else {
            buf.push_back(s[j]);
            j++;
          }
        }
        if (j >= static_cast<int>(s.size())) {
          tokens.push_back(Token(
              f, i, "ERROR",
              "EOF on incomplete string literal"));
          return tokens;
        }
        j += q.size();
        tokens.push_back(Token(f, i, "STR", buf));
        i = j;
        continue;
      }
      j = i;
    }

    // number
    {
      bool seen_digit = false;
      while (j < static_cast<int>(s.size()) && isdigit(s[j])) {
        j++;
        seen_digit = true;
      }
      if (j < static_cast<int>(s.size()) && s[j] == '.') {
        j++;
      }
      while (j < static_cast<int>(s.size()) && isdigit(s[j])) {
        j++;
        seen_digit = true;
      }
      if (seen_digit) {
        tokens.push_back(Token(f, i, "NUM", s.substr(i, j-i)));
        i = j;
        continue;
      }
      j = i;
    }

    // symbol -- depends on SYMBOLS being ordered.
    std::string symbol;
    for (auto &sym: SYMBOLS) {
      if (sym == s.substr(i, sym.size())) {
        symbol = sym;
      }
    }
    if (!symbol.empty()) {
      if (OPENERS.find(symbol) != OPENERS.end()) {
        stack.push_back(symbol);
      } else if (CLOSERS.find(symbol) != CLOSERS.end()) {
        if (stack.empty() || symbol != MATCHES.find(stack.back())->second) {
          std::string message = "Mismatched parenthesis";
          if (!stack.empty()) {
            message += ", expected: " + MATCHES.find(stack.back())->second;
          }
          tokens.push_back(Token(f, i, "ERROR", message));
          return tokens;
        }
        stack.pop_back();
      }
      tokens.push_back(Token(f, i, symbol, ""));
      i += symbol.size();
      continue;
    }

    // id and keywords
    while (j < static_cast<int>(s.size()) && (isalnum(s[j]) || s[j] == '_')) {
      j++;
    }
    if (i != j) {
      std::string name = s.substr(i, j-i);
      if (KEYWORDS.find(name) == KEYWORDS.end()) {
        tokens.push_back(Token(f, i, "ID", name));
      } else {
        tokens.push_back(Token(f, i, name, ""));
      }
      i = j;
      continue;
    }

    // err
    while (j < static_cast<int>(s.size()) && !isspace(s[j])) {
      j++;
    }
    tokens.push_back(Token(
        f, i, "ERROR",
        "Invalid token: " + s.substr(i, j-i)));
    return tokens;
  }

  tokens.push_back(Token(f, i, "EOF", ""));
  return tokens;
}

}
