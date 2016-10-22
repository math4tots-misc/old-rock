const {RockError} = require('./file.js');
const ast = require('./ast.js');

function unrecognized(tokens, kind, node) {
  return new RockError(
      tokens, 'Unrecognized ' + kind + ' Ast: ' + node.constructor.name);
}

/** Instantiates template classes into normal classes.
 *
 * Its constructor accepts an optional argument 'templateTypeCallback'.
 * This callback will get called with the TemplateType Ast node
 * every time we encounter a template type.
 *
 * This is meant to be used by whoever is controlling the instantiation to
 * instantiate every encountered template type.
 */
class Instantiator {
  constructor(templateTypeCallback) {
    this.templateTypeCallback = templateTypeCallback || (node => null);
  }

  instantiateType(node, callTokens, mappings) {
    const tokens = callTokens.concat(node.tokens);
    switch (node.constructor) {
      case ast.Typename:
        return mappings[node.name] ?
            this.copyTypeWithCallTokens(mappings[node.name], callTokens) :
            node;
      case ast.TemplateType: {
        if (mappings[node.name]) {
          throw new RockError(
              tokens,
              'Template arguments cannot be used as template types');
        }
        const args = node.args.map(
            arg => this.instantiateType(arg, tokens, mappings));
        const type = new ast.TemplateType(tokens, node.name, args);
        this.templateTypeCallback(type);
        return type;
      }
      default:
        throw unrecognized(tokens, 'Type', node);
    }
  }

  copyTypeWithCallTokens(node, callTokens) {
    const tokens = callTokens.concat(node.tokens);
    switch (node.constructor) {
      case ast.Typename:
        return new ast.Typename(tokens, node.name);
      case ast.TemplateType:
        return new ast.TemplateType(
            tokens, node.name,
            node.args.map(
                arg => this.copyTypeWithCallTokens(arg, callTokens)));
      default:
        throw unrecognized(tokens, 'Type', node);
    }
  }

  instantiateTemplateClass(node, callTokens, types) {
    const tokens = callTokens.concat(node.tokens);
    if (node.typeargs.length !== types.length) {
      throw new RockError(
          tokens,
          'Expected ' + node.typeargs.length + 'args but got ' +
          types.length + ' args');
    }
    const type = new ast.TemplateType(tokens, node.name, types);
    const mappings = Object.create(null);
    for (let i = 0; i < types.length; i++) {
      mappings[node.typeargs[i]] = types[i];
    }
    const traits = node.traits.map(
        trait => this.instantiateType(trait, callTokens, mappings));
    const fields = node.fields.map(
        field => this.instantiateField(field, callTokens, mappings));
    const methods = node.methods.map(
        method => this.instantiateMethod(method, callTokens, mappings));
    return new ast.Class(
        tokens, node.isExtern, node.isTrait, type, traits, fields, methods);
  }

  instantiateField(node, callTokens, mappings) {
    const type = this.instantiateType(node.type, callTokens, mappings);
    return new ast.Field(type, node.name);
  }

  instantiateMethod(node, callTokens, mappings) {
    const tokens = callTokens.concat(this.tokens);
    const returnType =
        this.instantiateType(returnType, callTokens, mappings);
    const args = this.args.map(
        field => this.instantiateField(field, callTokens, mappings));
    const body = this.instantiateStatement(body, callTokens, mappings);
    return new ast.Method(tokens, returnType, node.name, args, body);
  }

  instantiateStatement(node, callTokens, mappings) {
    const tokens = callTokens.concat(this.tokens);
    switch (node.constructor) {
      case ast.Block:
        return new ast.Block(
            tokens,
            this.statements.map(
                stmt => this.instantiateStatement(
                  stmt, callTokens, mappings)));
      case ast.Return:
        return new ast.Return(
            tokens,
            this.instantiateExpression(
                node.value, callTokens, mappings));
      case ast.Break:
        return new ast.Break(callTokens.concat(this.tokens));
      case ast.Continue:
        return new ast.Continue(callTokens.concat(this.tokens));
      case ast.While:
        return new ast.While(
            tokens,
            this.instantiateExpression(node.condition, callTokens, mappings),
            this.instantiateStatement(node.body, callTokens, mappings));
      default:
        if (node instanceof ast.Expression) {
          return this.instantiateExpression(node, callTokens, mappings);
        }
        throw unrecognized(tokens, 'Statement', node);
    }
  }

  instantiateExpression(node, callTokens, mappings) {
    const tokens = callTokens.concat(this.tokens);
    switch (node.constructor) {
      case ast.Int:
        return new ast.Int(tokens, node.value);
      case ast.Float:
        return new ast.Float(tokens, node.value);
      case ast.Text:
        return new ast.Text(tokens, node.value);
      case ast.Name:
        return new ast.Name(tokens, node.name);
      case ast.MethodCall:
        return new ast.MethodCall(
            tokens,
            this.instantiateExpression(node.owner, callTokens, mappings),
            node.name,
            node.args.map(arg => this.instantiateExpression(
                arg, callTokens, mappings)));
      default:
        throw unrecognized(tokens, 'Expression', node);
    }
  }
}

exports.Instantiator = Instantiator;
