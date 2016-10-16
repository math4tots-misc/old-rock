const err = require('./err.js');

const keywords = [
  'package', 'import', 'as', 'class', 'trait', 'with', 'static', 'final',
  'if', 'else', 'for', 'while', 'break', 'continue', 'return',
  'true', 'false', 'and', 'or',

  // unused but reserved
  'def', 'interface', 'do', 'null', 'goto', 'new', 'const', 'let', 'var',
  'public', 'private', 'extends', 'extend', 'implement', 'implements',
];

const symbols = [
  '(', ')', '[', ']', '{', '}', '=',
  ';', ',', '.', '->',
  '+', '-',
].sort().reverse();

function _atStringStart(s, i) {
  if (s[i] === '"' || s[i] === "'") {
    return true;
  }
  if (s[i] === 'r') {
    if (i+1 < s.length && (s[i+1] === '"' || s[i+1] === "'")) {
      return true;
    }
  }
  return false;
}

class Lexer {
  constructor(file) {
    this.file = file;
    this.uri = file.uri;
    this.s = file.contents;
    this.i = 0;
    this.peek = this._extract();
  }
  _skipWhitespace() {
    while (this.i < this.s.length && /\s/.test(this.s[this.i])) {
      this.i++;
    }
  }
  _extract() {
    this._skipWhitespace();
    if (this.i >= this.s.length) {
      return new err.Token('EOF', null, this.file, this.i);
    }
    const j = this.i;
    if (/\d/.test(this.s[this.i])) {  // INT/FLOAT
      this.i++;
      while (this.i < this.s.length && /\d/.test(this.s[this.i])) {
        this.i++;
      }
      if (this.s[this.i] === '.') {
        this.i++;
        while (/\d/.test(this.s[this.i])) {
          this.i++;
        }
        const value = parseFloat(this.s.slice(j, this.i));
        return new err.Token('FLOAT', value, this.file, j);
      } else {
        const value = parseInt(this.s.slice(j, this.i));
        return new err.Token('INT', value, this.file, j);
      }
    }
    if (/\w/.test(this.s[this.i])) {  // NAME/TYPENAME/keyword
      while (this.i < this.s.length && /\w/.test(this.s[this.i])) {
        this.i++;
      }
      const value = this.s.slice(j, this.i);
      if (keywords.indexOf(value) === -1) {
        const kind = /[A-Z]/.test(value[0]) ? 'TYPENAME' : 'NAME';
        return new err.Token(kind, value, this.file, j);
      } else {
        return new err.Token(value, null, this.file, j);
      }
    }
    if (_atStringStart(this.s, this.i)) {  // STRING
      let raw = this.s[this.i] === 'r';
      if (raw) {
        this.i++;
      }
      let quote = this.s[this.i];
      if (this.s.startsWith(quote.repeat(3), this.i)) {
        quote = quote.repeat(3);
      }
      this.i += quote.length;
      let value = '';
      while (this.i < this.s.length && !this.s.startsWith(quote, this.i)) {
        if (!raw && this.s[this.i] === '\\') {
          this.i++;
          if (this.i >= this.s.length) {
            throw new err.RockError(
                'Invalid escape',
                [new err.Token('ERR', null, this.file, this.i)]);
          }
          switch (this.s[this.i]) {
          case 'n': value += '\n'; break;
          case 't': value += '\t'; break;
          case '\\': value += '\\'; break;
          case '"': value += '\"'; break;
          case "'": value += '\''; break;
          default:
            throw new err.RockError(
                'Invalid escape',
                [new err.Token('ERR', null, this.file, this.i)]);
          }
          this.i++;
        } else {
          value += this.s[this.i];
          this.i++;
        }
      }
      if (this.i >= this.s.length) {
        throw new err.RockError(
            'Unfinished string literal',
            [new err.Token('ERR', null, this.file, this.i)]);
      }
      this.i += quote.length;
      return new err.Token('STRING', value, this.file, j);
    }
    for (const symbol of symbols) {  // symbol
      if (this.s.startsWith(symbol, j)) {
        this.i += symbol.length;
        return new err.Token(symbol, null, this.file, j);
      }
    }
    throw new err.RockError(
        'Unrecognized token', [new err.Token('ERR', null, this.file, j)]);
  }
  next() {
    const token = this.peek;
    this.peek = this._extract();
    return token;
  }
  toArray() {
    const tokens = [];
    while (this.peek.type !== 'EOF') {
      tokens.push(this.next());
    }
    tokens.push(this.peek);
    return tokens;
  }
}

exports.Lexer = Lexer;
