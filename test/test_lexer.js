const {expect} = require('chai');
const err = require('../rock/err.js');
const lexer = require('../rock/lexer.js');

describe('Lexer', () => {
  const file = new err.File('foo.txt', 'a "b" 3 4.5 ; Foo class');
  it('should handle simple lexes', () => {
    const tokens = new lexer.Lexer(file).toArray();
    expect(tokens.length).to.equal(8);
    expect(tokens[0].type).to.equal('NAME');
    expect(tokens[0].value).to.equal('a');
    expect(tokens[1].type).to.equal('STRING');
    expect(tokens[1].value).to.equal('b');
    expect(tokens[2].type).to.equal('INT');
    expect(tokens[2].value).to.equal(3);
    expect(tokens[3].type).to.equal('FLOAT');
    expect(tokens[3].value).to.equal(4.5);
    expect(tokens[4].type).to.equal(';');
    expect(tokens[5].type).to.equal('TYPENAME');
    expect(tokens[5].value).to.equal('Foo');
    expect(tokens[6].type).to.equal('class');
    expect(tokens[7].type).to.equal('EOF');
  });
});
