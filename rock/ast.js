class Ast {
  constructor(token) {
    this.token = token;
  }
}

class Module extends Ast {
  constructor(token, namespace, imports, functions) {
    super(token);
    this.namespace = namespace;
    this.imports = imports;
    this.functions = functions;
  }
}

class Import extends Ast {
  constructor(token, path) {
    super(token);
    this.path = path;
  }
}
