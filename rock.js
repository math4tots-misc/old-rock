// jshint esversion: 6

(function() {
'use strict';

class File {
  constructor(uri, contents) {
    this.uri = uri;
    this.contents = contents;
  }
}

class Token {
  constructor(type, value, file, i) {
    this.type = type;
    this.value = value;
    this.file = file;
    this.i = i;
  }

  getLineNumber() {
    let lineNumber = 1;
    for (let i = 0; i < this.i; i++) {
      if (this.file.contents[i] === '\n') {
        lineNumber++;
      }
    }
    return lineNumber;
  }

  getColumnNumber() {
    let columnNumber = 1;
    for (let i = this.i; i > 0 && this.file.contents[i - 1] !== '\n'; i--) {
      columnNumber++;
    }
    return columnNumber;
  }

  getLine() {
    const start = this.i - this.getColumnNumber() + 1;
    let end = this.i;
    while (end < this.file.contents.length &&
           this.file.contents[end] !== '\n') {
      end++;
    }
    return this.file.contents.slice(start, end);
  }

  getLocationMessage() {
    return '\nin file "' + this.file.uri + '" on line ' +
           this.getLineNumber() +
           '\n' + this.getLine() +
           '\n' + ' '.repeat(this.getColumnNumber() - 1) + '*';
  }
}

class RockError extends Error {
  constructor(message, tokens) {
    super(message +
          tokens.map(token => token.getLocationMessage()).join('') +
          '\n');
  }
}

const keywords = [
  'class', 'trait', 'final',
  'if', 'else', 'for', 'while', 'break', 'continue',
  'true', 'false', 'and', 'or',

  // unused but reserved
  'def', 'interface', 'do', 'null', 'goto', 'new', 'const', 'let', 'var',
  'public', 'private',
];

const symbols = [
  '(', ')', '[', ']', '{', '}',
  ';', ',', '.', '->',
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
      return new Token('EOF', null, this.file, this.i);
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
        return new Token('FLOAT', value, this.file, j);
      } else {
        const value = parseInt(this.s.slice(j, this.i));
        return new Token('INT', value, this.file, j);
      }
    }
    if (/\w/.test(this.s[this.i])) {  // NAME/TYPENAME/keyword
      while (this.i < this.s.length && /\w/.test(this.s[this.i])) {
        this.i++;
      }
      const value = this.s.slice(j, this.i);
      if (keywords.indexOf(value) === -1) {
        const kind = /[A-Z]/.test(value[0]) ? 'TYPENAME' : 'NAME';
        return new Token(kind, value, this.file, j);
      } else {
        return new Token(value, null, this.file, j);
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
            throw new RockError(
                'Invalid escape',
                [new Token('ERR', null, this.file, this.i)]);
          }
          switch (this.s[this.i]) {
          case 'n': value += '\n'; break;
          case 't': value += '\t'; break;
          case '\\': value += '\\'; break;
          case '"': value += '\"'; break;
          case "'": value += '\''; break;
          default:
            throw new RockError(
                'Invalid escape',
                [new Token('ERR', null, this.file, this.i)]);
          }
          this.i++;
        } else {
          value += this.s[this.i];
          this.i++;
        }
      }
      if (this.i >= this.s.length) {
        throw new RockError(
            'Unfinished string literal',
            [new Token('ERR', null, this.file, this.i)]);
      }
      this.i += quote.length;
      return new Token('STRING', value, this.file, j);
    }
    for (const symbol of symbols) {  // symbol
      if (this.s.startsWith(symbol, j)) {
        this.i += symbol.length;
        return new Token(symbol, null, this.file, j);
      }
    }
    throw new RockError(
        'Unrecognized token', [new Token('ERR', null, this.file, j)]);
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

// NOTE: qualified_typename fields are always a string.
// They represent either concrete types like rock.lang.Integer,
// or generic types like rock.lang.List.$Item
// qualified_typename values are set to null until the annotation
// when they are filled in after types are resolved.

class Ast {
  constructor(token) {
    this.token = token;
  }

  notImplementedError() {
    return new RockError(
        'Not implemented for ' + this.constructor.name,
        [this.token]);
  }
}

class Module extends Ast {
  constructor(token, pkg, imports, classes) {
    super(token);
    this.pkg = pkg;  // String
    this.imports = imports;  // [Import]
    this.classes = classes;  // [Class]
  }
}

class Import extends Ast {
  constructor(token, pkg, name, alias) {
    super(token);
    this.pkg = pkg;  // String
    this.name = name;  // String
    this.alias = alias || name;  // String
  }
}

class Class extends Ast {
  constructor(token, name, args, fields, methods) {
    super(token);
    this.name = name;  // String
    this.args = args;  // [GenericArgument]
    this.fields = fields;  // [Field]
    this.methods = methods;  // [Method]
  }
}

class GenericArgument extends Ast {
  constructor(token, variance, name, base) {
    super(token);
    this.variance = variance;  // 'covariant'/'contravariant'/'invariant'
    this.name = name;  // String
    this.base = base;  // null|Type
  }
}

class Field extends Ast {
  constructor(token, isStatic, type, name) {
    super(token);
    this.isStatic = isStatic;  // Boolean
    this.type = type;  // Type
    this.name = name;  // String
  }
}

class Method extends Ast {
  constructor(token, isStatic, type, name, args, body) {
    super(token);
    this.isStatic = isStatic;  // Boolean
    this.type = type;  // Type
    this.name = name;  // String
    this.args = args;  // [Argument]
    this.body = body;  // null|Block
  }
}

class Argument extends Ast {
  constructor(token, type, name) {
    super(token);
    this.type = type;  // Type
    this.name = name;  // String
  }
}

class Type extends Ast {
  constructor(token) {
    super(token);
    this.qualified_typename = null;  // to be filled in during annotation
  }
}

class Typename extends Type {
  constructor(token, name) {
    super(token);
    this.name = name;  // String
  }
}

class GenericType extends Type {
  constructor(token, name, args) {
    super(token);
    this.name = name;  // String
    this.args = args;  // [Type]
  }
}

class Statement extends Ast {}

class Declaration extends Statement {
  constructor(token, isFinal, type, name, value) {
    super(token);
    this.isFinal = isFinal;  // Boolean
    this.type = type;  // Type
    this.name = name;  // String
    this.value = value;  // Expression
  }
}

class Block extends Statement {
  constructor(token, statements) {
    super(token);
    this.statements = statements;  // [Statement]
  }
}

class If extends Statement {
  constructor(token, condition, body, other) {
    super(token);
    this.condition = condition;  // Expression
    this.body = body;  // Block
    this.other = other || new Block(token, []);  // Block
  }
}

class While extends Statement {
  constructor(token, condition, body) {
    super(token);
    this.condition = condition;  // Expression
    this.body = body;  // Block
  }
}

class Break extends Statement {}

class Continue extends Statement {}

class Expression extends Statement {
  constructor(token) {
    super(token);
    this.qualified_typename = null;  // to be filled in during annotation
  }
}

class Name extends Expression {
  constructor(token, name) {
    super(token);
    this.name = name;  // String
  }
}

class IntLiteral extends Expression {
  constructor(token, value) {
    super(token);
    this.value = value;  // Int
  }
}

class FloatLiteral extends Expression {
  constructor(token, value) {
    super(token);
    this.value = value;  // Float
  }
}

class StringLiteral extends Expression {
  constructor(token, value) {
    super(token);
    this.value = value;  // String
  }
}

class MethodCall extends Expression {
  constructor(token, owner, name, args) {
    super(token);
    this.owner = owner;  // Expression
    this.name = name;  // String
    this.args = args;  // [Expression]
  }
}

class GetAttribute extends Expression {
  constructor(token, owner, name) {
    super(token);
    this.owner = owner;  // Expression
    this.name = name;  // String
  }
}

class SetAttribute extends Expression {
  constructor(token, owner, name, value) {
    super(token);
    this.owner = owner;  // Expression
    this.name = name;  // String
    this.value = value;  // Expression
  }
}

class Cast extends Expression {
  constructor(token, value, type) {
    super(token);
    this.value = value;  // Expression
    this.type = type;  // Type
  }
}

class Assign extends Expression {
  constructor(token, name, value) {
    super(token);
    this.name = name;  // String
    this.value = value;  // Expression
  }
}

class LogicalOperation extends Expression {
  constructor(token, left, operand, right) {
    super(token);
    this.left = left;  // Expression
    this.operand = operand;  // 'and'|'or'
    this.right = right;  // Expression
  }
}

class Ternary extends Expression {
  constructor(token, condition, left, right) {
    super(token);
    this.condition = condition;  // Expression
    this.left = left;  // Expression
    this.right = right;  // Expression
  }
}

const exports =
    typeof module === 'undefined' ? Object.create(null) : module.exports;
exports.File = File;
exports.Token = Token;
exports.RockError = RockError;
exports.Lexer = Lexer;

})();
