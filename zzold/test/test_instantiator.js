const {expect} = require('chai');
const {File, Token} = require('../rock/file.js');
const ast = require('../rock/ast.js');
const {Instantiator} = require('../rock/instantiator.js');

// TODO: Write tests for the other instantiator methods after
// implementing the parser. It would be easier to do these tests with
// a parser.

describe('Instantiator', () => {
  const i = new Instantiator();
  describe('instantiateType', () => {
    it('should leave non-matching type as is', () => {
      const type = new ast.Typename([], 'Foo');
      expect(type.toString()).to.equal('Foo');
      const mappings = {'Bar': new ast.Typename([], 'Baz')};
      const newType = i.instantiateType(type, [], mappings);
      expect(newType.toString()).to.equal('Foo');
    });
    it('should replace matching type', () => {
      const type = new ast.Typename([], 'Foo');
      expect(type.toString()).to.equal('Foo');
      const mappings = {'Foo': new ast.Typename([], 'Bar')};
      const newType = i.instantiateType(type, [], mappings);
      expect(newType.toString()).to.equal('Bar');
    });
    it('should replace matching type in template type', () => {
      const type = new ast.TemplateType(
          [], 'Foo', [new ast.Typename([], 'Bar')]);
      expect(type.toString()).to.equal('Foo[Bar]');
      const mappings = {'Bar': new ast.Typename([], 'Baz')};
      const newType = i.instantiateType(type, [], mappings);
      expect(newType.toString()).to.equal('Foo[Baz]');
    });
  });
});
