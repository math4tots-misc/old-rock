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
  parseModule(callTokens) {
    const includes = [];
    while (this.at('#')) {
      const token = this.expect('#');
      if (this.expect('NAME').value !== 'include') {
        throw new RockError(
            callTokens.concat([token]), 'Invalid include directive');
      }
      const uri = this.expect('STRING').value;
      includes.push(uri);
    }
    const classes = [];
    while (this.at('class') || this.at('trait') || this.at('extern')) {
      classes.push(this.parseClass(callTokens));
    }
    const tokens = callTokens.concat(this.tokens);
    return new ast.Module(tokens, includes, classes);
  }
}

exports.Parser = Parser;
