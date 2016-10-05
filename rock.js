// jshint esversion: 6

(function() {
'use strict';

class File {
  constructor(uri, contents) {
    this.uri = uri;
    this.contents = contents;
  }
}

class Token {
  constructor(type, value, file, i) {
    this.type = type;
    this.value = value;
    this.file = file;
    this.i = i;
  }

  getLineNumber() {
    let lineNumber = 1;
    for (let i = 0; i < this.i; i++) {
      if (this.file.contents[i] === '\n') {
        lineNumber++;
      }
    }
    return lineNumber;
  }

  getColumnNumber() {
    let columnNumber = 1;
    for (let i = this.i; i > 0 && this.file.contents[i - 1] !== '\n'; i--) {
      columnNumber++;
    }
    return columnNumber;
  }

  getLine() {
    const start = this.i - this.getColumnNumber() + 1;
    let end = this.i;
    while (end < this.file.contents.length &&
           this.file.contents[end] !== '\n') {
      end++;
    }
    return this.file.contents.slice(start, end);
  }

  getLocationMessage() {
    return '\nin file "' + this.file.uri + '" on line ' +
           this.getLineNumber() +
           '\n' + this.getLine() +
           '\n' + ' '.repeat(this.getColumnNumber() - 1) + '*';
  }
}

class RockError extends Error {
  constructor(message, tokens) {
    super(message +
          tokens.map(token => token.getLocationMessage()).join('') +
          '\n');
  }
}


const exports =
    typeof module === 'undefined' ? Object.create(null) : module.exports;
exports.File = File;
exports.Token = Token;
exports.RockError = RockError;

})();
