// jshint esversion: 6

const {expect} = require('chai');
const rock = require('./rock.js');


describe('Token', () => {
  describe('#getLocationMessage', () => {
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
