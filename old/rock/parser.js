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
  /**
   * file:
   *   File object containing the path (URI) to the file and
   *   text to be parsed.
   * callTokens (optional):
   *   Array of tokens that represents the context when this parse
   *   was invoked. Defaults to an empty array.
   *   This is useful when trying to figure out what file included
   *   other this file that it was caused to be processed.
   */
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
  skipType() {
    if (!this.at('TYPENAME')) {
      return false;
    }
    while (this.at(['TYPENAME', ',', openBracket, closeBracket])) {
      this.i++;
    }
    return true;
  }
  atDeclaration() {
    const save = this.i;
    if (this.at('final')) {
      return true;
    }
    if (!this.skipType()) {
      return false;
    }
    if (!this.consume('NAME')) {
      this.i = save;
      return false;
    }
    if (!this.at(';') && !this.at('=')) {
      this.i = save;
      return false;
    }
    this.i = save;
    return true;
  }
  /** Returns an array of tokens representing the current context.
   * If there is a compile time error, this should contain the full
   * "compile time stack trace" needed for a user to debug the program.
   */
  getTokensHere() {
    return this.callTokens.concat([this.peek()]);
  }
  parseModule() {
    const tokens = this.getTokensHere();
    const includes = [];
    while (this.at('#')) {
      const tokens = this.getTokensHere();
      const token = this.expect('#');
      if (this.expect('NAME').value !== 'include') {
        throw new RockError(tokens, 'Invalid include directive');
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
    while (this.atDeclaration()) {
      fields.push(this.parseField());
    }
    while (!this.consume(closeBrace)) {
      methods.push(this.parseMethod());
    }
    return [fields, methods];
  }
  parseClassTemplate() {
    const tokens = this.getTokensHere();
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
    const tokens = this.getTokensHere();
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
    const tokens = this.getTokensHere();
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
  parseField() {
    const tokens = this.getTokensHere();
    const type = this.parseType();
    const name = this.expect('NAME').value;
    this.expect(';');
    return new ast.Field(tokens, type, name);
  }
  parseMethod() {
    const tokens = this.getTokensHere();
    const type = this.parseType();
    const name = this.expect('NAME').value;
    const args = this.parseArguments();
    const body = this.consume(';') ? null : this.parseBlock();
    return new ast.Method(tokens, type, name, args, body);
  }
  parseArguments() {
    this.expect(openParenthesis);
    const args = [];
    while (!this.consume(closeParenthesis)) {
      const tokens = this.getTokensHere();
      const type = this.parseType();
      const name = this.expect('NAME').value;
      args.push(new ast.Field(tokens, type, name));
      if (!this.consume(',')) {
        this.expect(closeParenthesis);
        break;
      }
    }
    return args;
  }
  parseBlock() {
    const tokens = this.getTokensHere();
    this.expect(openBrace);
    const statements = [];
    while (!this.consume(closeBrace)) {
      statements.push(this.parseStatement());
    }
    return new ast.Block(tokens, statements);
  }
  parseStatement() {
    const tokens = this.getTokensHere();
    if (this.at(openBrace)) {
      return this.parseBlock();
    } else if (this.atDeclaration()) {
      const isFinal = !!this.consume('final');
      const type = this.at('TYPENAME') ? this.parseType() : null;
      const name = this.expect('NAME').value;
      const value = this.consume('=') ? this.parseExpression() : null;
      this.expect(';');
      return new ast.Declaration(tokens, isFinal, type, name, value);
    } else if (this.consume('while')) {
      const condition = this.parseExpression();
      const body = this.parseBlock();
      return new ast.While(tokens, condition, body);
    } else if (this.consume('continue')) {
      return new ast.Continue(tokens);
    } else if (this.consume('break')) {
      return new ast.Break(tokens);
    } else if (this.consume('if')) {
      const condition = this.parseExpression();
      const body = this.parseBlock();
      const other =
          this.consume('else') ?
          (this.at('if') ? this.parseStatement() : this.parseBlock()) :
          null;
      return new ast.If(tokens, condition, body, other);
    } else {
      const expression = this.parseExpression();
      this.expect(';');
      return expression;
    }
  }
  parseExpression() {
    return this.parsePostfixExpression();
  }
  parsePostfixExpression() {
    let expression = this.parsePrimaryExpression();
    while (true) {
      const tokens = this.getTokensHere();
      if (this.consume('.')) {
        const name = this.expect('NAME').value;
        if (this.at(openParenthesis)) {
          const args = this.parseExpressionList('(', ')');
          expression = new ast.MethodCall(tokens, expression, name, args);
        } else if (this.consume('=')) {
          const value = this.parseExpression();
          expression = new ast.SetField(tokens, expression, name, value);
        } else {
          expression = new ast.GetField(tokens, expression, name);
        }
      }
      break;
    }
    return expression;
  }
  parsePrimaryExpression() {
    if (this.consume(openParenthesis)) {
      const expression = this.parseExpression();
      this.expect(closeParenthesis);
      return expression;
    }
    const tokens = this.getTokensHere();
    if (this.consume('this')) {
      return new ast.This(tokens);
    }
    if (this.at('INT')) {
      return new ast.Int(tokens, this.expect('INT').value);
    }
    if (this.at('FLOAT')) {
      return new ast.Float(tokens, this.expect('FLOAT').value);
    }
    if (this.at('STRING')) {
      return new ast.Str(tokens, this.expect('STRING').value);
    }
    if (this.at('NAME')) {
      const name = this.expect('NAME').value;
      if (this.consume('=')) {
        const value = this.parseExpression();
        return new ast.Assign(tokens, name, value);
      } else if (this.at(openParenthesis)) {
        const args = this.parseExpressionList('(', ')');
        return new ast.MethodCall(tokens, new ast.This(tokens), name, args);
      } else {
        return new ast.Name(tokens, name);
      }
    }
    throw new RockError(tokens, 'Expected expression');
  }
  parseExpressionList(open, close) {
    const expressions = [];
    this.expect(open);
    while (!this.consume(close)) {
      expressions.push(this.parseExpression());
      if (!this.consume(',')) {
        this.expect(close);
        break;
      }
    }
    return expressions;
  }
}

exports.Parser = Parser;
