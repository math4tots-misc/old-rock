// jshint esversion: 6

(function() {
'use strict';

const openParenthesis = '(';
const closeParenthesis = ')';
const openBracket = '[';
const closeBracket = ']';
const openBrace = '{';
const closeBrace = '}';

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

const primitive_types = [
  'void', 'bool', 'int', 'float', 'string',
];

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
        const kind = /[A-Z]/.test(value[0]) || primitive_types[value] ?
                     'TYPENAME' : 'NAME';
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

// NOTE: type_tag values are set to null until the annotation
// when they are filled in as types are resolved.

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
  constructor(token, isTrait, name, typeargs, traits, fields, methods) {
    super(token);
    this.isTrait = isTrait;  // Boolean
    this.name = name;  // String
    this.typeargs = typeargs;  // [GenericArgument]
    this.traits = traits;  // [Type]
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
  constructor(token, isStatic, type, name, typeargs, args, body) {
    super(token);
    this.isStatic = isStatic;  // Boolean
    this.type = type;  // Type
    this.name = name;  // String
    this.typeargs = typeargs;  // [GenericArgument]
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
    this.type_tag = null;  // to be filled in during annotation
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

class Return extends Statement {
  constructor(token, value) {
    super(token);
    this.value = value;
  }
}

class Expression extends Statement {
  constructor(token) {
    super(token);
    this.type_tag = null;  // to be filled in during annotation
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

class GetField extends Expression {
  constructor(token, owner, name) {
    super(token);
    this.owner = owner;  // Expression
    this.name = name;  // String
  }
}

class SetField extends Expression {
  constructor(token, owner, name, value) {
    super(token);
    this.owner = owner;  // Expression
    this.name = name;  // String
    this.value = value;  // Expression
  }
}

class New extends Expression {
  constructor(token, owner, args) {
    super(token);
    this.owner = owner;  // Type
    this.args = args;  // [Expression]
  }
}

class StaticMethodCall extends Expression {
  constructor(token, owner, name, args) {
    super(token);
    this.owner = owner;  // Type
    this.name = name;  // String
    this.args = args;  // [Expression]
  }
}

class StaticGetField extends Expression {
  constructor(token, owner, name) {
    super(token);
    this.owner = owner;  // Type
    this.name = name;  // String
  }
}

class StaticSetField extends Expression {
  constructor(token, owner, name, value) {
    super(token);
    this.owner = owner;  // Type
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

class Parser {
  constructor(file) {
    this.file = file;
    this.tokens = new Lexer(file).toArray();
    this.i = 0;
  }
  peek(lookahead) {
    return this.tokens[this.i + (lookahead || 0)];
  }
  next() {
    const token = this.peek();
    this.i++;
    return token;
  }
  at(types, lookahead) {
    const token = this.peek(lookahead);
    return typeof types === 'string' ?
           types === token.type :
           types.indexOf(token.type) !== -1;
  }
  expect(types) {
    if (this.at(types)) {
      return this.next();
    }
    throw new RockError(
        'Expected ' + types + ' but got ' + this.peek().type,
        [this.peek()]);
  }
  consume(types) {
    if (this.at(types)) {
      return this.next();
    }
  }
  parseModule() {
    const token = this.peek();
    const pkg = this.parsePackageDeclaration();
    const imports = this.parseImports();
    const classes = this.parseClasses();
    if (!this.at('EOF')) {
      throw new RockError(
          'Expected class or trait but got "' + this.peek().type + '"',
          [this.peek()]);
    }
    return new Module(token, pkg, imports, classes);
  }
  parsePackageDeclaration() {
    if (this.consume('package')) {
      const pkg = this.consumePackageName();
      this.expect(';');
      return pkg;
    }
    return '__default__';
  }
  consumePackageName() {
    let pkg = this.expect('NAME').value;
    while (this.at('.') && this.at('NAME', 1)) {
      this.expect('.');
      pkg += '.' + this.expect('NAME').value;
    }
    return pkg;
  }
  parseImports() {
    const imports = [];
    while (this.at('import')) {
      const token = this.consume('import');
      const pkg = this.consumePackageName();
      this.expect('.');
      const className = this.expect('TYPENAME').value;
      const alias = this.consume('as') ?
                    this.expect('TYPENAME').value :
                    className;
      this.expect(';');
      imports.push(new Import(token, pkg, className, alias));
    }
    return imports;
  }
  parseClasses() {
    const classes = [];
    while (this.at(['class', 'trait'])) {
      classes.push(this.parseClass());
    }
    return classes;
  }
  parseClass() {
    const token = this.peek();
    const isTrait = !!this.consume('trait');
    if (!isTrait) {
      this.expect('class');
    }
    const name = this.expect('TYPENAME').value;
    const typeargs = this.parseGenericArguments();
    const traits = this.parseWithDeclaration();
    const fields = [];
    const methods = [];
    this.expect(openBrace);
    while (!this.consume(closeBrace)) {
      const token = this.peek();
      const isStatic = !!this.consume('static');
      const type = this.parseType();
      const name = this.expect('NAME').value;
      if (this.consume(';')) {
        fields.push(new Field(token, isStatic, type, name));
      } else {
        methods.push(this.parseMethod(token, isStatic, type, name));
      }
    }
    return new Class(token, isTrait, name, typeargs, traits, fields, methods);
  }
  parseGenericArguments() {
    if (!this.at(openBracket)) {
      return null;
    }
    this.expect(openBracket);
    const args = [];
    while (!this.consume(closeBracket)) {
      args.push(this.parseGenericArgument());
      if (!this.consume(',')) {
        this.expect(closeBracket);
        break;
      }
    }
    return args;
  }
  parseGenericArgument() {
    const token = this.peek();
    const variance = this.consume('+') ? 'covariant' :
                     this.consume('-') ? 'contravariant' :
                     'invariant';
    const name = this.expect('TYPENAME').value;
    const base = this.consume('extends') ? this.parseType() : null;
    return new GenericArgument(token, variance, name, base);
  }
  parseWithDeclaration() {
    if (!this.consume('with')) {
      return [];
    }
    const traits = [this.parseType()];
    if (this.consume(',')) {
      while (this.at('TYPENAME')) {
        traits.push(this.parseType());
        this.consume(',');
      }
    }
    return traits;
  }
  parseType() {
    const token = this.peek();
    const name = this.expect('TYPENAME').value;
    if (this.consume(openBracket)) {
      const args = [];
      while (!this.consume(closeBracket)) {
        args.push(this.parseType());
      }
      return new GenericType(token, name, args);
    } else {
      return new Typename(token, name);
    }
  }
  parseMethod(token, isStatic, type, name) {
    const typeargs = this.parseGenericArguments();
    const args = this.parseArguments();
    const body = this.parseBlock();
    return new Method(token, isStatic, type, name, typeargs, args, body);
  }
  parseArguments() {
    const args = [];
    this.expect(openParenthesis);
    while (!this.consume(closeParenthesis)) {
      const token = this.peek();
      const type = this.parseType();
      const name = this.expect('NAME');
      args.push(new Argument(token, type, name));
      if (!this.consume(',')) {
        this.expect(closeParenthesis);
        break;
      }
    }
    return args;
  }
  parseBlock() {
    const token = this.peek();
    this.expect(openBrace);
    const statements = [];
    while (!this.consume(closeBrace)) {
      statements.push(this.parseStatement());
    }
    return new Block(token, statements);
  }
  parseStatement() {
    const token = this.peek();
    if (this.atDeclaration()) {
      const isFinal = !!this.consume('final');
      const type = this.at('TYPENAME') ? this.parseType() : null;
      const name = this.expect('NAME').value;
      const value = this.consume('=') ? this.parseExpression() : null;
      this.expect(';');
      return new Declaration(token, isFinal, type, name, value);
    } else if (this.consume('break')) {
      this.expect(';');
      return new Break(token);
    } else if (this.consume('continue')) {
      this.expect(';');
      return new Continue(token);
    } else if (this.consume('return')) {
      const expression = this.at(';') ? null : this.parseExpression();
      this.expect(';');
      return new Return(token, expression);
    } else {
      const expression = this.parseExpression();
      this.expect(';');
      return expression;
    }
  }
  atDeclaration() {
    if (this.at('final')) {
      return true;
    }
    if (!this.at('TYPENAME')) {
      return false;
    }
    const savedLocation = this.i;
    try {
      this.parseType();
      this.expect('NAME');
      return true;
    } finally {
      this.i = savedLocation;
    }
    return false;
  }
  parseExpression() {
    return this.parsePostfixExpression();
  }
  parsePostfixExpression() {
    let expression = this.parsePrimaryExpression();
    while (true) {
      const token = this.peek();
      if (this.consume('.')) {
        const name = this.expect('NAME').value;
        if (this.at(openParenthesis)) {
          const args =
              this.parseExpressionList(openParenthesis, closeParenthesis);
          expression = new MethodCall(token, expression, name, args);
        } else if (this.consume('=')) {
          const value = this.parseExpression();
          expression = new SetField(token, expression, name, value);
        } else {
          expression = new GetField(token, expression, name);
        }
      } else if (this.consume('as')) {
        const type = this.parseType();
        expression = new Cast(token, expression, type);
      } else {
        break;
      }
    }
    return expression;
  }
  parseExpressionList(open, close) {
    this.expect(open);
    const exprs = [];
    while (!this.consume(close)) {
      exprs.push(this.parseExpression());
      if (!this.consume(',')) {
        this.expect(close);
        break;
      }
    }
    return exprs;
  }
  parsePrimaryExpression() {
    const token = this.peek();
    if (this.consume(openParenthesis)) {
      const expression = this.parseExpression();
      this.expect(closeParenthesis);
      return expression;
    }
    if (this.consume('INT')) {
      return new IntLiteral(token, token.value);
    }
    if (this.consume('NAME')) {
      const name = token.value;
      return new Name(token, name);
    }
    throw new RockError('Expected expression', [token]);
  }
}

// NOTE: For now, to simplify implementation, assume no generic types.
// This applies to TypeTag, Analyzer and Annotator.
// TODO: Implement generic types.

class TypeTag {
  constructor(pkg, name) {
    this.pkg = pkg;
    this.name = name;
  }
  equals(other) {
    return this.toString() === other.toString();
  }
  toString() {
    return this.pkg + '.' + this.name;
  }
}

class Analyzer {
  constructor(modules) {
    this.name_to_ast = Object.create(null);
    for (const mod of modules) {
      this._readModule(mod);
    }
  }
  _readModule(mod) {
    const pkg = mod.pkg;
    for (const cls of mod.classes) {
      const name = pkg + '.' + cls.name;
      if (this.name_to_ast[name]) {
        throw new RockError(
            'Duplicate definition of ' + name,
            [this.name_to_ast[name].token, cls.token]);
      }
      this.name_to_ast[name] = cls;
    }
  }
  getMethodReturnType(ownerTypeTag, name, argtypes) {
  }
  getFieldType(ownerTypeTag, name) {
  }
  getStaticMethodReturnType(ownerTypeTag, name, argtypes) {
  }
  getStaticFieldType(ownerTypeTag, name) {
  }
}

const GLOBAL_ALIASES = [
  ['Self', new TypeTag('rock.lang', 'Self')],
  ['Bool', new TypeTag('rock.lang', 'Bool')],
  ['Int', new TypeTag('rock.lang', 'Int')],
  ['Float', new TypeTag('rock.lang', 'Float')],
  ['String', new TypeTag('rock.lang', 'String')],
];

class Annotator {
  constructor(analyzer) {
    this.analyzer = analyzer;
  }
  annotateModule(mod) {
    const pkg = mod.pkg;
    const aliases = Object.create(null);
    const alias_to_ast = Object.create(null);
    for (const imp of mod.imports) {
      if (aliases[imp.alias]) {
        throw new RockError(
            'Name conflict',
            [aliases_to_ast[imp.alias].token, imp.token]);
      }
      aliases[imp.alias] = new TypeTag(imp.pkg, imp.name);
      alias_to_ast[imp.alias] = imp;
    }
    for (const cls of mod.classes) {
      if (aliases[cls.name]) {
        throw new RockError(
            'Name conflict',
            [aliases_to_ast[cls.name].token, cls.token]);
      }
      aliases[cls.name] = new TypeTag(pkg, cls.name);
      alias_to_ast[cls.name] = cls;
    }
    for (const [alias, tag] of GLOBAL_ALIASES) {
      if (aliases[alias]) {
        throw new RockError(
            'This declaration shadows a global builtin name ' +
                '(' + tag.toString() + ')',
            [aliases_to_ast[alias].token]);
      }
      aliases[alias] = tag;
    }
    for (const cls of mod.classes) {
      this.annotateClass(aliases, cls);
    }
  }
  annotateClass(aliases, cls) {
    for (const trait of cls.traits) {
      this.annotateType(aliases, trait);
    }
    for (const field of cls.fields) {
      this.annotateField(aliases, cls, field);
    }
    for (const method of cls.methods) {
      this.annotateMethod(aliases, cls, method);
    }
  }
  annotateType(aliases, type) {
    if (!alises[type.name]) {
      throw new RockError('Undeclared type: ' + type.name, [type.token]);
    }
    type.type_tag = aliases[type.name];
  }
  annotateField(alises, field) {
    this.annotateType(field.type);
  }
  annotateMethod(alises, method) {
    this.annotateType(method.type);
    for (const arg of method.args) {
      this.annotateArgument(aliases, args);
    }
    if (method.body) {
      this.annotateStatement(method.body);
    }
  }
}


const exports =
    typeof module === 'undefined' ? Object.create(null) : module.exports;
exports.File = File;
exports.Token = Token;
exports.RockError = RockError;
exports.Lexer = Lexer;
exports.Parser = Parser;
exports.TypeTag = TypeTag;

exports.Import = Import;
exports.Class = Class;
exports.GenericArgument = GenericArgument;
exports.Statement = Statement;
exports.Return =  Return;
exports.Declaration =  Declaration;
exports.Name = Name;
exports.IntLiteral = IntLiteral;
exports.Typename = Typename;
exports.GenericType = GenericType;
exports.Cast = Cast;
exports.GetField = GetField;
exports.SetField = SetField;
exports.MethodCall = MethodCall;

})();
