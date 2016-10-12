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
  describe('parseModule', () => {
    it('should handle programs with no methods', () => {
      const file = new rock.File('foo.txt', `
      package foo.bar;

      trait Baz {
        String x;
      }

      class Foo[] with Bar[Int], Baz {}
      `);
      const parser = new rock.Parser(file);
      const module = parser.parseModule();
      expect(module.pkg).to.equal('foo.bar');
      expect(module.classes.length).to.equal(2);
      {
        const cls = module.classes[0];
        expect(cls.isTrait).to.equal(true);
        expect(cls.name).to.equal('Baz');
        expect(cls.traits.length).to.equal(0);
      }
      {
        const cls = module.classes[1];
        expect(cls.isTrait).to.equal(false);
        expect(cls.name).to.equal('Foo');
        expect(cls.traits.length).to.equal(2);
        {
          const type = cls.traits[0];
          expect(type.name).to.equal('Bar');
          expect(type.args.length).to.equal(1);
          const argtype = type.args[0];
          expect(argtype.name).to.equal('Int');
        }
        {
          const type = cls.traits[1];
          expect(type.name).to.equal('Baz');
        }
      }
    });
    it('should set a default package if none is set', () => {
      const file = new rock.File('foo.txt', `
      `);
      const parser = new rock.Parser(file);
      const module = parser.parseModule();
      expect(module.pkg).to.equal('__default__');
    });
    it('should parse programs with an empty method', () => {
      const file = new rock.File('foo.txt', `
      class Foo {
        Void bar(String a, String b) {}
      }
      `);
      const parser = new rock.Parser(file);
      const module = parser.parseModule();
    });
    it('should parse programs with a single method', () => {
      const file = new rock.File('foo.txt', `
      class Foo {
        Void bar(String a, String b) {
          break;
          continue;
        }
      }
      `);
      const parser = new rock.Parser(file);
      const module = parser.parseModule();
    });
  });
  describe('parseStatement', () => {
    it('should parse return statements without values', () => {
      const file = new rock.File('foo.txt', 'return;');
      const parser = new rock.Parser(file);
      const statement = parser.parseStatement();
      expect(statement.constructor).to.equal(rock.Return);
      expect(statement.value).to.equal(null);
    });
    it('should parse return statements with values', () => {
      const file = new rock.File('foo.txt', 'return 5;');
      const parser = new rock.Parser(file);
      const statement = parser.parseStatement();
      expect(statement.constructor).to.equal(rock.Return);
      expect(statement.value.constructor).to.equal(rock.IntLiteral);
      expect(statement.value.value).to.equal(5);
    });
  });
  describe('parseExpression', () => {
    function parseExpression(string) {
      const file = new rock.File('foo.txt', string);
      const parser = new rock.Parser(file);
      return parser.parseExpression();
    }
    it('should parse Int expressions', () => {
      const file = new rock.File('foo.txt', '261');
      const parser = new rock.Parser(file);
      const expression = parser.parseExpression();
      expect(expression.constructor).to.equal(rock.IntLiteral);
      expect(expression.value).to.equal(261);
    });
    it('should parse Name expressions', () => {
      const file = new rock.File('foo.txt', 'hoi');
      const parser = new rock.Parser(file);
      const expression = parser.parseExpression();
      expect(expression.constructor).to.equal(rock.Name);
      expect(expression.name).to.equal('hoi');
    });
    it('should parse GetField expressions', () => {
      const expression = parseExpression('a.x');
      expect(expression.constructor).to.equal(rock.GetField);
      expect(expression.name).to.equal('x');
      const owner = expression.owner;
      expect(owner.constructor).to.equal(rock.Name);
      expect(owner.name).to.equal('a');
    });
    it('should parse SetField expressions', () => {
      const expression = parseExpression('a.x = b');
      expect(expression.constructor).to.equal(rock.SetField);
      expect(expression.name).to.equal('x');
      const owner = expression.owner;
      expect(owner.constructor).to.equal(rock.Name);
      expect(owner.name).to.equal('a');
      const value = expression.value;
      expect(value.constructor).to.equal(rock.Name);
      expect(value.name).to.equal('b');
    });
    it('should parse MethodCall expressions', () => {
      const expression = parseExpression('a.f(1, 2)');
      expect(expression.constructor).to.equal(rock.MethodCall);
      expect(expression.name).to.equal('f');
      const owner = expression.owner;
      expect(owner.constructor).to.equal(rock.Name);
      expect(owner.name).to.equal('a');
      const args = expression.args;
      expect(args.length).to.equal(2);
      expect(args[0].constructor).to.equal(rock.IntLiteral);
      expect(args[0].value).to.equal(1);
      expect(args[1].constructor).to.equal(rock.IntLiteral);
      expect(args[1].value).to.equal(2);
    });
    it('should parse Cast expressions', () => {
      const file = new rock.File('foo.txt', '5 as Int');
      const parser = new rock.Parser(file);
      const expression = parser.parseExpression();
      expect(expression.constructor).to.equal(rock.Cast);
      const original = expression.value;
      expect(original.constructor).to.equal(rock.IntLiteral);
      expect(original.value).to.equal(5);
      const type = expression.type;
      expect(type.constructor).equals(rock.Typename);
      expect(type.name).equals('Int');
    });
  });
});
