#ifndef rock_lexer_hh
#define rock_lexer_hh

#include <map>
#include <set>
#include <string>
#include <vector>

namespace rock {

struct Lexer final {
  File *const file;
  long position;
  Token *last;
  bool doneFlag;
  std::vector<std::string> parenthesisStack;
  Lexer(File *f):
      file(f), position(0), last(nullptr), doneFlag(false) {
    last = extract();
  }

  Token *peek() const {
    return last;
  }

  Token *next() {
    if (last->type == "ERR" || last->type == "EOF") {
      doneFlag = true;
      return last;
    } else {
      Token *token = last;
      last = extract();
      return token;
    }
  }

  bool done() const {
    return doneFlag;
  }

private:
  static bool isword(char c) {
    return c == '_' || std::isalnum(c);
  }
  std::string slice(long start, long end) const {
    return file->contents.substr(start, end - start);
  }
  char ch() const {
    return ch(0);
  }
  char ch(long diff) const {
    long p = position + diff;
    return (p < 0 || p >= file->contents.size()) ? '\0' : file->contents[p];
  }
  bool startsWith(const std::string& prefix) const {
    return startsWith(prefix, 0);
  }
  bool startsWith(const std::string& prefix, long diff) const {
    for (char c: prefix) {
      if (ch(diff++) != c) {
        return false;
      }
    }
    return true;
  }
  // The rule for newline tokens:
  // If the innermost grouping is '()' or '[]', newlines are ignored.
  // If you are at the top level, or the innermost grouping is '{}',
  // newline tokens are generated.
  bool isSpace(char c) const {
    return c == '\n' ?
           !parenthesisStack.empty() && (
              parenthesisStack.back() == "(" ||  // )
              parenthesisStack.back() == "[") :  // ]
           std::isspace(c);
  }
  void skipWhitespace() {
    while (isSpace(ch())) {
      position++;
    }
  }
  Token *extract() {
    std::set<std::string> keywords({
      "class", "for", "return",
    });
    std::vector<std::string> symbols({
      "(", ")", "[", "]", "{", "}", ";", "\\", ".",
    });
    std::map<std::string, std::string> matches({
      {"(", ")"},
      {"[", "]"},
      {"{", "}"},
    });
    std::set<std::string> openers({"(", "[", "{"});
    std::set<std::string> closers({")", "]", "}"});
    std::sort(symbols.begin(), symbols.end());
    std::reverse(symbols.begin(), symbols.end());

    skipWhitespace();

    if (ch() == '\0') {
      if (!parenthesisStack.empty()) {
        return new Token(
            file, position, "ERR",
            std::string("Mismatched parenthesis: ") +
                parenthesisStack.back());
      }
      if (last && last->type == "NEWLINE") {
        return new Token(file, position, "EOF");
      } else {
        return new Token(file, position, "NEWLINE");
      }
    }

    const long p = position;

    // NEWLINE
    if (ch() == '\n') {
      position++;
      return new Token(file, p, "NEWLINE");
    }

    // STRING
    if (startsWith("r'") || startsWith("'") ||
        startsWith("r\"") || startsWith("\"")) {
      bool raw = false;
      if (startsWith("r")) {
        raw = true;
        position++;
      }
      std::string quote = slice(position, position+1);
      if (startsWith(quote + quote + quote)) {
        quote = slice(position, position + 3);
      }
      position += quote.size();
      std::string value;
      while (!startsWith(quote)) {
        if (ch() == '\0') {
          return new Token(file, position, "ERR", "Unfinished string");
        }
        if (!raw && ch() == '\\') {
          position++;
          switch (ch()) {
          case 'n': value += '\n'; break;
          case 't': value += '\t'; break;
          case '\'': value += '\''; break;
          case '\"': value += '\"'; break;
          default:
            return new Token(file, position, "ERR", "Invalid escape");
          }
          position++;
        } else {
          value += ch();
          position++;
        }
      }
      position += quote.size();
      return new Token(file, p, "STRING", value);
    }

    // SYMBOL
    for (const auto& symbol: symbols) {
      if (startsWith(symbol)) {
        if (openers.find(symbol) != openers.end()) {
          parenthesisStack.push_back(symbol);
        } else if (closers.find(symbol) != closers.end()) {
          if (matches[parenthesisStack.back()] != symbol) {
            return new Token(file, p, "ERR", "Mismatched parenthesis");
          } else {
            parenthesisStack.pop_back();
          }
        }
        position += symbol.size();
        return new Token(file, p, symbol);
      }
    }

    // NUMBER
    if (std::isdigit(ch()) || (ch() == '.' && std::isdigit(ch(1)))) {
      while (std::isdigit(ch())) {
        position++;
      }
      if (ch() == '.') {
        position++;
        while (std::isdigit(ch())) {
          position++;
        }
      }
      return new Token(file, p, "NUMBER", slice(p, position));
    }

    // NAME/keywords
    while (isword(ch())) {
      position++;
    }
    if (p != position) {
      auto word = slice(p, position);
      if (keywords.find(word) == keywords.end()) {
        return new Token(file, p, "NAME", word);
      } else {
        return new Token(file, p, word);
      }
    }

    // Unrecognized token
    return new Token(file, position, "ERR", "Unrecognized token");
  }
};

inline std::vector<Token*> lex(File *f) {
  std::vector<Token*> tokens;
  Lexer lexer(f);
  while (!lexer.done()) {
    tokens.push_back(lexer.next());
  }
  return tokens;
}

}

#endif//rock_lexer_hh
