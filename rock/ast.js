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

class ClassTemplate extends Ast {
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
}

class Class extends Ast {
  constructor(tokens, isExtern, isTrait, type, traits, fields, methods) {
    super(tokens);
    this.isExtern = isExtern;  // boolean
    this.isTrait = isTrait;  // boolean
    this.type = type;  // Type:
                       //   Typename if normal class,
                       //   TemplateType if instantiated from ClassTemplate
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
  toTypeString() {
    return 'T' + '__' + this.args.length + '__' + this.name + '__' +
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
}

class Method extends Ast {
  constructor(tokens, returnType, name, args, body) {
    super(tokens);
    this.returnType = returnType;  // Type
    this.name = name;  // string
    this.args = args;  // [Field]  # HACK: Don't reuse Field for Argument.
    this.body = body;  // Block
  }
}

class Statement extends Ast {}

class Block extends Statement {
  constructor(tokens, statements) {
    super(tokens);
    this.statements = statements;  // [Statement]
  }
}

class Return extends Statement {
  constructor(tokens, value) {
    super(tokens);
    this.value = value;  // Expression
  }
}

class Break extends Statement {}

class Continue extends Statement {}

class While extends Statement {
  constructor(tokens, condition, body) {
    super(tokens);
    this.condition = condition;  // Expression
    this.body = body;  // Block
  }
}

class If extends Statement {
  constructor(tokens, condition, body, other) {
    super(tokens);
    this.condition = condition;  // Expression
    this.body = body;  // Block
    this.other = other;  // Block|If|null
  }
}

class Expression extends Statement {}

class Int extends Expression {
  constructor(tokens, value) {
    super(tokens);
    this.value = value;
  }
}

class Float extends Expression {
  constructor(tokens, value) {
    super(tokens);
    this.value = value;
  }
}

class Str extends Expression {
  constructor(tokens, value) {
    super(tokens);
    this.value = value;
  }
}

class This extends Expression {}

class Name extends Expression {
  constructor(tokens, name) {
    super(tokens);
    this.name = name;  // string
  }
}

class MethodCall extends Expression {
  constructor(tokens, owner, name, args) {
    super(tokens);
    this.owner = owner;  // Expression
    this.name = name;  // string
    this.args = args;  // [Expression]
  }
}

class GetField extends Expression {
  constructor(tokens, owner, name) {
    super(tokens);
    this.owner = owner;  // Expression
    this.name = name;  // string
  }
}

class SetField extends Expression {
  constructor(tokens, owner, name, value) {
    super(tokens);
    this.owner = owner;  // Expression
    this.name = name;  // string
    this.value = value;  // Expression
  }
}

exports.Program = Program;
exports.Ast = Ast;
exports.Module = Module;
exports.ClassTemplate = ClassTemplate;
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
exports.If = If;
exports.Expression = Expression;
exports.Int = Int;
exports.Float = Float;
exports.Str = Str;
exports.This = This;
exports.Name = Name;
exports.GetField = GetField;
exports.SetField = SetField;
exports.MethodCall = MethodCall;
