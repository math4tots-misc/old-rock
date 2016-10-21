const {expect} = require('chai');
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
    it('should parse simple class', () => {
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
  });
});

