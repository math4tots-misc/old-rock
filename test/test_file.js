const {expect} = require('chai');
const {File, Token} = require('../rock/file.js');

describe('Token', () => {
  describe('getLocationMessage', () => {
    const file = new File('foo.txt', 'hello\naa bb');
    it('should handle beginning of contents', () => {
      const token = new Token('type', 'value', file, 0);
      expect(token.getLocationMessage()).to.equal(
        '\nin file "foo.txt" on line 1' +
        '\nhello' +
        '\n*');
    });
    it('should handle another simplecase', () => {
      const token = new Token('type', 'value', file, 9);
      expect(token.getLocationMessage()).to.equal(
        '\nin file "foo.txt" on line 2' +
        '\naa bb' +
        '\n   *');
    });
  });
});
