// jshint esversion: 6

const {expect} = require('chai');
const rock = require('./rock.js');


describe('Token', () => {
  describe('getLocationMessage', () => {
    const file = new rock.File('foo.txt', 'hello\naa bb');
    it('should handle beginning of contents', () => {
      const token = new rock.Token('type', 'value', file, 0);
      expect(token.getLocationMessage()).to.equal(
        '\nin file "foo.txt" on line 1' +
        '\nhello' +
        '\n*');
    });
    it('should handle another simplecase', () => {
      const token = new rock.Token('type', 'value', file, 9);
      expect(token.getLocationMessage()).to.equal(
        '\nin file "foo.txt" on line 2' +
        '\naa bb' +
        '\n   *');
    });
  });
});

describe('Lexer', () => {
  const file = new rock.File('foo.txt', 'a "b" 3 4.5 ; Foo class');
  it('should handle simple lexes', () => {
    const tokens = new rock.Lexer(file).toArray();
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

describe('Parser', () => {
  
});
