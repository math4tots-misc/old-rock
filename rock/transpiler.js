const {RockError} = require('./file.js');
const {Parser} = require('./parser.js');

class Transpiler {
  constructor(fileLoader, codeGenerator) {
    this.fileLoader = fileLoader;
    this.codeGenerator = codeGenerator;
  }
  loadClassMap(path, classMap, seenFiles, callTokens) {
    classMap = classMap || Object.create(null);
    seenFiles = seenFiles || Object.create(null);
    callTokens = Array.from(callTokens || []);
    if (seenFiles[path]) {
      return classMap;
    }
    seenFiles[path] = true;
    const file = this.fileLoader.load(path);
    const module = new Parser(file, callTokens).parseModule();
    for (const inc of module.includes) {
      this.loadClassMap(path, classMap, seenFiles, module.tokens);
    }
    for (const cls of module.classes) {
      if (classMap[cls.name]) {
        throw RockError(
            classMap[cls.name].tokens.concat(cls.tokens),
            'Class name conflict');
      }
    }
    return classMap;
  }
  transpile(source, target, callTokens) {
  }
}
