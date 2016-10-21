const {Token, RockError} = require('./file.js');
const {Lexer} = require('./lexer.js');
const ast = require('./ast.js');

const openParenthesis = '(';
const closeParenthesis = ')';
const openBracket = '[';
const closeBracket = ']';
const openBrace = '{';
const closeBrace = '}';

class Parser {
  constructor(file, callTokens) {
    this.file = file;
    this.tokens = new Lexer(file).toArray();
    this.callTokens = callTokens || [];
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
  atClassTemplate() {
    return this.at(openBracket, 2 + !!this.at('extern'));
  }
  atField() {
    const save = this.i;
    try {
      this.parseType();
      this.expect('NAME');
      this.expect(';');
      return true;
    } catch(e) {
      return false;
    } finally {
      this.i = save;
    }
  }
  parseModule() {
    const includes = [];
    while (this.at('#')) {
      const token = this.expect('#');
      if (this.expect('NAME').value !== 'include') {
        throw new RockError(
            this.callTokens.concat([token]), 'Invalid include directive');
      }
      const uri = this.expect('STRING').value;
      includes.push(uri);
    }
    const classes = [];
    const class_templates = [];
    while (this.at('class') || this.at('trait') || this.at('extern')) {
      if (this.atClassTemplate()) {
        class_templates.push(this.parseClassTemplate());
      } else {
        classes.push(this.parseClass());
      }
    }
    const tokens = this.callTokens.concat(this.tokens);
    return new ast.Module(tokens, includes, classes);
  }
  parseWithClause() {
    const traits = [];
    if (this.consume('with')) {
      traits.push(this.parseType());
      while (this.consume(',')) {
        traits.push(this.parseType());
      }
    }
    return traits;
  }
  parseClassBody() {
    const fields = [];
    const methods = [];
    this.expect(openBrace);
    while (this.atField()) {
      fields.push(this.parseField());
    }
    while (!this.consume(closeBrace)) {
      methods.push(this.parseMethod());
    }
    return [fields, methods];
  }
  parseClassTemplate() {
    const tokens = this.callTokens.concat([this.peek()]);
    const isExtern = !!this.consume('extern');
    const isTrait = !!this.consume('trait');
    if (!isTrait) {
      this.expect('class');
    }
    const name = this.expect('TYPENAME').value;
    const typeargs = [];
    this.expect(openBracket);
    while (!this.consume(closeBracket)) {
      typeargs.push(this.expect('TYPENAME').value);
      if (!this.consume(',')) {
        this.expect(closeBracket);
        break;
      }
    }
    const traits = this.parseWithClause();
    const [fields, methods] = this.parseClassBody();
    return new ast.ClassTemplate(
        tokens, isExtern, isTrait, name, typeargs, traits, fields, methods);
  }
  parseClass() {
    const tokens = this.callTokens.concat([this.peek()]);
    const isExtern = !!this.consume('extern');
    const isTrait = !!this.consume('trait');
    if (!isTrait) {
      this.expect('class');
    }
    const type = new ast.Typename(tokens, this.expect('TYPENAME').value);
    const traits = this.parseWithClause();
    const [fields, methods] = this.parseClassBody();
    return new ast.Class(
        tokens, isExtern, isTrait, type, traits, fields, methods);
  }
  parseType() {
    const tokens = this.callTokens.concat([this.peek()]);
    const name = this.expect('TYPENAME').value;
    if (this.consume(openBracket)) {
      const args = [];
      while (!this.consume(closeBracket)) {
        args.push(this.parseType());
        if (!this.consume(',')) {
          this.expect(closeBracket);
          break;
        }
      }
      return new ast.TemplateType(tokens, name, args);
    }
    return new ast.Typename(tokens, name);
  }
}

exports.Parser = Parser;
