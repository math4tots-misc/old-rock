const chai = require('chai');
chai.use(require('chai-subset'));
const {expect} = chai;
const {File, Token} = require('../rock/file.js');
const ast = require('../rock/ast.js');
const {Parser} = require('../rock/parser.js');


describe('Parser', () => {
  function parse(kind, text) {
    return new Parser(new File('<test>', text))['parse' + kind]([]);
  }
  describe('parseType', () => {
    it('should parse a Typename', () => {
      const text = 'Foo';
      const node = parse('Type', text);
      expect(node.constructor).to.equal(ast.Typename);
      expect(node.name).to.equal('Foo');
    });
    it('should parse a TemplateType', () => {
      const text = 'Foo[Bar]';
      const node = parse('Type', text);
      expect(node.constructor).to.equal(ast.TemplateType);
      expect(node.name).to.equal('Foo');
      expect(node.args.length).to.equal(1);
      expect(node.args[0].constructor).to.equal(ast.Typename);
      expect(node.args[0].name).to.equal('Bar');
    });
  });
  describe('parseClassTemplate', () => {
    it('should parse simple class template', () => {
      const text = 'class Foo[Bar, Baz] {}';
      const node = parse('ClassTemplate', text);
      expect(node.constructor).to.equal(ast.ClassTemplate);
      expect(node.isExtern).to.equal(false);
      expect(node.isTrait).to.equal(false);
      expect(node.name).to.equal('Foo');
      expect(node.typeargs.length).to.equal(2);
      expect(node.typeargs[0]).to.equal('Bar');
      expect(node.typeargs[1]).to.equal('Baz');
    });
  });
  describe('parseClass', () => {
    it('should parse an empty class', () => {
      const text = 'class Foo with Bar, Baz {}';
      const node = parse('Class', text);
      expect(node.constructor).to.equal(ast.Class);
      expect(node.type.constructor).to.equal(ast.Typename);
      expect(node.type.name).to.equal('Foo');
      expect(node.traits.length).to.equal(2);
      expect(node.traits[0].constructor).to.equal(ast.Typename);
      expect(node.traits[0].name).to.equal('Bar');
      expect(node.traits[1].constructor).to.equal(ast.Typename);
      expect(node.traits[1].name).to.equal('Baz');
    });
    it('should parse a class with a field', () => {
      const node = parse('Class', 'class Foo { String text; }');
      expect(node.constructor).to.equal(ast.Class);
      expect(node.type.constructor).to.equal(ast.Typename);
      expect(node.type.name).to.equal('Foo');
      expect(node.fields.length).to.equal(1);
      expect(node.fields[0].name).to.equal('text');
    });
  });
  describe('parseField', () => {
    it('should parse a field', () => {
      const node = parse('Field', 'Int foo;');
      expect(node).to.containSubset({
        'constructor': ast.Field,
        'name': 'foo',
        'type': {'constructor': ast.Typename, 'name': 'Int'},
      });
    });
  });
  describe('parseMethod', () => {
    it('should parse an empty method', () => {
      const node = parse('Method', 'Void foo(String bar, Float baz) {}');
      expect(node).to.containSubset({
        'constructor': ast.Method,
        'name': 'foo',
        'returnType': {'constructor': ast.Typename, 'name': 'Void'},
        'args': [
          {
            'constructor': ast.Field,
            'type': {'constructor': ast.Typename, 'name': 'String'},
            'name': 'bar',
          },
          {
            'constructor': ast.Field,
            'type': {'constructor': ast.Typename, 'name': 'Float'},
            'name': 'baz',
          },
        ],
        'body': {
          'constructor': ast.Block,
          'statements': [],
        },
      });
    });
  });
  describe('parseExpression', () => {
    it('should parse a string literal', () => {
      const node = parse('Expression', '"hello world!"');
      expect(node).to.containSubset({
        'constructor': ast.Str,
        'value': 'hello world!',
      });
    });
    it('should parse an int literal', () => {
      const node = parse('Expression', '1251');
      expect(node).to.containSubset({
        'constructor': ast.Int,
        'value': 1251,
      });
    });
    it('should parse a float literal', () => {
      const node = parse('Expression', '52.5');
      expect(node).to.containSubset({
        'constructor': ast.Float,
        'value': 52.5,
      });
    });
    it('should parse this', () => {
      const node = parse('Expression', 'this');
      expect(node).to.containSubset({
        'constructor': ast.This,
      });
    });
    it('should parse a name', () => {
      const node = parse('Expression', 'x');
      expect(node).to.containSubset({
        'constructor': ast.Name,
        'name': 'x',
      });
    });
    it('should parse a method call', () => {
      const node = parse('Expression', 'a.b()');
      expect(node).to.containSubset({
        'constructor': ast.MethodCall,
        'owner': {'constructor': ast.Name, 'name': 'a'},
        'name': 'b',
        'args': [],
      });
    });
    it('should parse getting a field', () => {
      const node = parse('Expression', 'a.b');
      expect(node).to.containSubset({
        'constructor': ast.GetField,
        'owner': {'constructor': ast.Name, 'name': 'a'},
        'name': 'b',
      });
    });
    it('should parse setting a field', () => {
      const node = parse('Expression', 'a.b = c');
      expect(node).to.containSubset({
        'constructor': ast.SetField,
        'owner': {'constructor': ast.Name, 'name': 'a'},
        'name': 'b',
        'value': {'constructor': ast.Name, 'name': 'c'},
      });
    });
  });
  describe('parseStatement', () => {
    it('should parse a block statement', () => {
      const node = parse('Statement', '{ x; }');
      expect(node).to.containSubset({
        'constructor': ast.Block,
        'statements': [{
          'constructor': ast.Name,
          'name': 'x',
        }],
      });
    });
    it('should parse an expression statement', () => {
      const node = parse('Statement', 'x;');
      expect(node).to.containSubset({
        'constructor': ast.Name,
        'name': 'x',
      });
    });
    it('should parse an if statement', () => {
      const node = parse('Statement', 'if (a) { b; } else { c; }');
      expect(node).to.containSubset({
        'constructor': ast.If,
        'condition': {'constructor': ast.Name, 'name': 'a'},
        'body': {
          'constructor': ast.Block,
          'statements': [{'constructor': ast.Name, 'name': 'b'}],
        },
        'other': {
          'constructor': ast.Block,
          'statements': [{'constructor': ast.Name, 'name': 'c'}],
        },
      });
    });
    it('should parse a while statement', () => {
      const node = parse('Statement', 'while (a) { b; }');
      expect(node).to.containSubset({
        'constructor': ast.While,
        'condition': {'constructor': ast.Name, 'name': 'a'},
        'body': {
          'constructor': ast.Block,
          'statements': [{'constructor': ast.Name, 'name': 'b'}],
        },
      });
    });
  });
});

