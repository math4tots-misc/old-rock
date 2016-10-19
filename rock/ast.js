const {RockError} = require('./file.js');

class Program {
  constructor(classes) {
    this.classes = classes;  // [Class]
  }
}

class Ast {
  constructor(tokens) {
    this.tokens = tokens;
  }
}

class Module extends Ast {
  constructor(tokens, includes, classes) {
    super(tokens);
    this.includes = includes;  // [string]  # [URI]
    this.classes = classes;  // [Class]
  }
}

class TemplateClass extends Ast {
  constructor(tokens, isExtern, isTrait, name,
              typeargs, traits, fields, methods) {
    super(tokens);
    this.isExtern = isExtern;  // boolean
    this.isTrait = isTrait;  // boolean
    this.name = name;  // string
    this.typeargs = typeargs;  // [string]
    this.traits = traits;  // [Type]
    this.fields = fields;  // [Field]
    this.methods = methods;  // [Method]
  }
  instantiateTemplateClass(callTokens, types) {
    const tokens = callTokens.concat(this.tokens);
    if (this.typeargs.length !== types.length) {
      throw new RockError(
          tokens,
          'Expected ' + this.typeargs.length + 'args but got ' +
          types.length + ' args');
    }
    const instantiatedType = new TemplateType(tokens, this.name, types);
    const name = instantiatedType.toTypeString();
    const mappings = Object.create(null);
    for (let i = 0; i < types.length; i++) {
      mappings[this.typeargs[i]] = types[i];
    }
    const traits = this.traits.map(
        trait => trait.instantiateType(callTokens, mappings));
    const fields = this.fields.map(
        field => field.instantiateField(callTokens, mappings));
    const methods = this.methods.map(
        method => method.instantiateMethod(callTokens, mappings));
    return new Class(
        tokens, this.isExtern, this.isTrait, name, traits, fields, methods);
  }
}

class Class extends Ast {
  constructor(tokens, isExtern, isTrait, name, traits, fields, methods) {
    super(tokens);
    this.isExtern = isExtern;  // boolean
    this.isTrait = isTrait;  // boolean
    this.name = name;  // string
    this.traits = traits;  // [Type]
    this.fields = fields;  // [Field]
    this.methods = methods;  // [Method]
  }
}

class Type extends Ast {}

class Typename extends Type {
  constructor(tokens, name) {
    super(tokens);
    this.name = name;  // string
  }
  copyTypeWithCallTokens(callTokens) {
    return new Typename(callTokens.concat(this.tokens), this.name);
  }
  instantiateType(callTokens, mappings) {
    return mappings[this.name] ?
        mappings[this.name].copyTypeWithCallTokens(callTokens) :
        this;
  }
  toTypeString() {
    return this.name;
  }
  toString() {
    return this.name;
  }
}

class TemplateType extends Type {
  constructor(tokens, name, args) {
    super(tokens);
    this.name = name;  // string
    this.args = args;  // [Type]
  }
  copyTypeWithCallTokens(callTokens) {
    return new TemplateType(
        callTokens.concat(this.tokens),
        this.args.map(arg => arg.copyTypeWithCallTokens(callTokens)));
  }
  instantiateType(callTokens, mappings) {
    const tokens = callTokens.concat(this.tokens);
    if (mappings[this.name]) {
      throw new RockError(
          tokens, 'Template arguments cannote be used as template types');
    }
    return new TemplateType(
        tokens, this.name,
        this.args.map(arg => arg.instantiateType(tokens, mappings)));
  }
  toTypeString() {
    return 'T' + this.args.length + '__' + this.name + '__' +
           this.args.map(arg => arg.toTypeString()).join('__');
  }
  toString() {
    return this.name +
           '[' + this.args.map(arg => arg.toString()).join(', ') + ']';
  }
}

class Field extends Ast {
  constructor(tokens, type, name) {
    super(tokens);
    this.type = type;  // Type
    this.name = name;  // string
  }
  instantiateField(callTokens, mappings) {
    const type = this.type.instantiateType(callTokens, mappings);
    return new Field(type, this.name);
  }
}

class Method extends Ast {
  constructor(tokens, returnType, name, args, body) {
    super(tokens);
    this.returnType = returnType;  // Type
    this.name = name;  // string
    this.args = args;  // [Field]  # HACK: Don't reuse Field for Argument.
    this.body = body;  // Block
  }
  instantiateMethod(callTokens, mappings) {
    const tokens = callTokens.concat(this.tokens);
    const returnType = this.returnType.instantiateType(callTokens, mappings);
    const args = this.args.map(
        field => field.instantiateField(callTokens, mappings));
    const body = this.body.instantiateStatement(callTokens, mappings);
    return new Method(tokens, returnType, this.name, args, body);
  }
}

class Statement extends Ast {}

class Block extends Statement {
  constructor(tokens, statements) {
    super(tokens);
    this.statements = statements;  // [Statement]
  }
  instantiateStatement(callTokens, mappings) {
    const tokens = callTokens.concat(this.tokens);
    const stmts = this.statements.map(
        stmt => stmt.instantiateStatement(callTokens, mappings));
    return new Block(tokens, stmts);
  }
}

class Return extends Statement {
  constructor(tokens, value) {
    super(tokens);
    this.value = value;  // Expression
  }
  instantiateStatement(callTokens, mappings) {
    const tokens = callTokens.concat(this.tokens);
    const value = this.value.instantiateExpression(callTokens, mappings);
    return new Return(tokens, value);
  }
}

class Break extends Statement {
  instantiateStatement(callTokens, mappings) {
    return new Break(callTokens.concat(this.tokens));
  }
}

class Continue extends Statement {
  instantiateStatement(callTokens, mappings) {
    return new Continue(callTokens.concat(this.tokens));
  }
}

class While extends Statement {
  constructor(tokens, condition, body) {
    super(tokens);
    this.condition = condition;  // Expression
    this.body = body;  // Block
  }
  instantiateStatement(callTokens, mappings) {
    const tokens = callTokens.concat(this.tokens);
    const condition = this.condition.instantiateExpression(
        callTokens, mappings);
    const body = this.body.instantiateStatement(callTokens, mappings);
    return new While(tokens, condition, body);
  }
}

class Expression extends Statement {
  instantiateStatement(callTokens, mappings) {
    return this.instantiateExpression(callTokens, mappings);
  }
}

class Int extends Expression {
  constructor(tokens, value) {
    super(tokens);
    this.value = value;
  }
  instantiateExpression(callTokens, mappings) {
    return new Int(callTokens.concat(this.tokens), this.value);
  }
}

class Float extends Expression {
  constructor(tokens, value) {
    super(tokens);
    this.value = value;
  }
  instantiateExpression(callTokens, mappings) {
    return new Float(callTokens.concat(this.tokens), this.value);
  }
}

class Text extends Expression {
  constructor(tokens, value) {
    super(tokens);
    this.value = value;
  }
  instantiateExpression(callTokens, mappings) {
    return new Text(callTokens.concat(this.tokens), this.value);
  }
}

class Name extends Expression {
  constructor(tokens, name) {
    super(tokens);
    this.name = name;  // string
  }
  instantiateExpression(callTokens, mappings) {
    return new Name(callTokens.concat(this.tokens), this.name);
  }
}

class MethodCall extends Expression {
  constructor(tokens, owner, name, args) {
    super(tokens);
    this.owner = owner;  // Expression
    this.name = name;  // string
    this.args = args;  // [Expression]
  }
  instantiateExpression(callTokens, mappings) {
    const tokens = callTokens.concat(this.tokens);
    const owner = this.owner.instantiateExpression(callTokens, mappings);
    const args = this.args.map(
        arg => arg.instantiateExpression(callTokens, mappings));
    return new MethodCall(tokens, owner, this.name, args);
  }
}

exports.Program = Program;
exports.Ast = Ast;
exports.Module = Module;
exports.TemplateClass = TemplateClass;
exports.Class = Class;
exports.Type = Type;
exports.Typename = Typename;
exports.TemplateType = TemplateType;
exports.Field = Field;
exports.Method = Method;
exports.Statement = Statement;
exports.Block = Block;
exports.Return = Return;
exports.Break = Break;
exports.Continue = Continue;
exports.While = While;
exports.Expression = Expression;
exports.Int = Int;
exports.Float = Float;
exports.Text = Text;
exports.Name = Name;
exports.MethodCall = MethodCall;
