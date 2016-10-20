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
});

describe('TemplateType', () => {
  it('should toTypeString', () => {
    const type = new ast.TemplateType(
        [], 'Foo', [new ast.Typename([], 'Bar')]);
    expect(type.toTypeString()).to.equal('T__1__Foo__Bar');
  });
  it('should toString', () => {
    const type = new ast.TemplateType(
        [], 'Foo', [new ast.Typename([], 'Bar')]);
    expect(type.toString()).to.equal('Foo[Bar]');
  });
});
