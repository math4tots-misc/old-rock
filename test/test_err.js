const {expect} = require('chai');
const err = require('../rock/err.js');

describe('Token', () => {
  describe('getLocationMessage', () => {
    const file = new err.File('foo.txt', 'hello\naa bb');
    it('should handle beginning of contents', () => {
      const token = new err.Token('type', 'value', file, 0);
      expect(token.getLocationMessage()).to.equal(
        '\nin file "foo.txt" on line 1' +
        '\nhello' +
        '\n*');
    });
    it('should handle another simplecase', () => {
      const token = new err.Token('type', 'value', file, 9);
      expect(token.getLocationMessage()).to.equal(
        '\nin file "foo.txt" on line 2' +
        '\naa bb' +
        '\n   *');
    });
  });
});
