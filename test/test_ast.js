const {expect} = require('chai');
const {File, Token} = require('../rock/file.js');
const ast = require('../rock/ast.js');

describe('Typename', () => {
  it('should toTypeString', () => {
    const type = new ast.Typename([], 'Foo');
    expect(type.toTypeString()).to.equal('Foo');
  });
  it('should toString', () => {
    const type = new ast.Typename([], 'Foo');
    expect(type.toString()).to.equal('Foo');
  });
  it('should instantiateType (no-op)', () => {
    const type = new ast.Typename([], 'Foo');
    const mappings = {'Bar': 'Baz'};
    const newType = type.instantiateType([], mappings);
    expect(newType.toString()).to.equal('Foo');
  });
  it('should instantiateType (substitute)', () => {
    const type = new ast.Typename([], 'Foo');
    const mappings = {'Foo': new ast.Typename([], 'Bar')};
    const newType = type.instantiateType([], mappings);
    expect(newType.toString()).to.equal('Bar');
  });
});

describe('TemplateType', () => {
  it('should toTypeString', () => {
    const type = new ast.TemplateType(
        [], 'Foo', [new ast.Typename([], 'Bar')]);
    expect(type.toTypeString()).to.equal('T1__Foo__Bar');
  });
  it('should toString', () => {
    const type = new ast.TemplateType(
        [], 'Foo', [new ast.Typename([], 'Bar')]);
    expect(type.toString()).to.equal('Foo[Bar]');
  });
});
