#include "rock/core/all.hh"

#include <sstream>

#include <iostream>

namespace rock {

Ast::Ast(const Token &t): token(t) {}

Reference Ast::eval(Scope&) const {
  throw exception(
      std::string(typeid(*this).name()) +
      " doesn't implement 'eval' yet");
}

std::string Ast::debug() const {
  return typeid(*this).name();
}

ParseError::ParseError(const Token &t, const std::string &m):
    Ast(t), message(m) {}

Reference ParseError::eval(Scope &scope) const {
  // TODO: Save token on stack, so that the location of the parse
  // error shows up on stack trace.
  throw exception("Parse error: " + message);
}

std::string ParseError::debug() const {
  return "ParseError(" + message + ")";
}

std::string ParseError::str() const {
  std::stringstream ss;
  ss << std::endl << message << token.locationMessage();
  return ss.str();
}

Block::Block(const Token &t, const std::vector<Ast*> &es):
    Ast(t), expressions(es) {}

Reference Block::eval(Scope &parentScope) const {
  Reference scope(new Scope(&parentScope));
  return evalWithoutNewScope(*scope.as<Scope>());
}

Reference Block::evalWithoutNewScope(Scope &scope) const {
  Reference last = nil;
  for (Ast *e: expressions) {
    last = e->eval(scope);
  }
  return last;
}

std::string Block::debug() const {
  std::stringstream ss;
  ss << "Block(";
  for (Ast *a: expressions) {
    ss << a->debug() << ",";
  }
  ss  << ")";
  return ss.str();
}

If::If(const Token &t, Ast *c, Ast *b, Ast *o):
    Ast(t), condition(c), body(b), other(o) {}

Reference If::eval(Scope &scope) const {
  return
      condition->eval(scope)->truthy() ?
      body->eval(scope) : other->eval(scope);
}

Switch::Switch(
    const Token &t, Ast *tg, const std::vector<std::tuple<Ast*,Ast*>> &cs,
    Ast *d):
    Ast(t), target(tg), conditions(cs), default_(d) {}

Reference Switch::eval(Scope &scope) const {
  Reference target = this->target->eval(scope);
  for (const auto& pair: conditions) {
    Reference case_ = std::get<0>(pair)->eval(scope);
    if (target.as<Object>() == case_.as<Object>()) {
      return std::get<1>(pair)->eval(scope);
    }
  }
  if (default_) {
    return default_->eval(scope);
  }
  StackFrame sf(&token);
  throw exception("Switch no matching case: " + target->str());
}

Or::Or(const Token &t, Ast *l, Ast *r): Ast(t), left(l), right(r) {}

Reference Or::eval(Scope &scope) const {
  Reference left = this->left->eval(scope);
  return left->truthy() ? left : right->eval(scope);
}

And::And(const Token &t, Ast *l, Ast *r): Ast(t), left(l), right(r) {}

Reference And::eval(Scope &scope) const {
  Reference left = this->left->eval(scope);
  return !left->truthy() ? left : right->eval(scope);
}

Arguments::Arguments(const Token &t, const std::vector<Ast*> &as):
    Arguments(t, as, nullptr) {}
Arguments::Arguments(const Token &t, const std::vector<Ast*> &as, Ast *va):
    token(t), args(as), vararg(va) {}
std::vector<Reference> Arguments::evalargs(Scope &scope) const {
  std::vector<Reference> out;
  if (vararg) {
    throw exception("splat arguments not yet supported");
  }
  for (Ast *e: args) {
    out.push_back(e->eval(scope));
  }
  return out;
}


MethodCall::MethodCall(
    const Token &t, Ast *o, const std::string &n, Arguments *a):
        Ast(t), owner(o), name(n), args(a) {}

Reference MethodCall::eval(Scope &scope) const {
  Reference owner = this->owner->eval(scope);
  std::vector<Reference> args = this->args->evalargs(scope);
  StackFrame sf(&token);
  return owner->call(name, args);
}

SuperCall::SuperCall(const Token &t, const std::string &n, Arguments *a):
    Ast(t), name(n), args(a) {}

Reference SuperCall::eval(Scope &scope) const {
  Reference owner = scope.get("this");
  Reference clsref = scope.get("super");
  checktype(classClass, clsref);
  std::vector<Reference> args = this->args->evalargs(scope);
  StackFrame sf(&token);
  return owner->callSuper(clsref.as<Class>(), name, args);
}

While::While(const Token &t, Ast *c, Ast *b):
    Ast(t), condition(c), body(b) {}

Reference While::eval(Scope &scope) const {
  while (this->condition->eval(scope)->truthy()) {
    this->body->eval(scope);
  }
  return nil;
}

Declaration::Declaration(const Token &t, const std::string &n, Ast *v):
    Ast(t), name(n), value(v) {}

Reference Declaration::eval(Scope &scope) const {
  return
      value ?
      scope.declare(name, this->value->eval(scope)) : scope.declare(name);
}

Assignment::Assignment(const Token &t, const std::string &n, Ast *v):
    Ast(t), name(n), value(v) {}

Reference Assignment::eval(Scope &scope) const {
  return scope.set(name, value->eval(scope));
}

Name::Name(const Token &t, const std::string &n):
    Ast(t), name(n) {}

Reference Name::eval(Scope &scope) const {
  StackFrame sf(&token);
  return scope.get(name);
}

Literal::Literal(const Token &t, Reference v):
    Ast(t), value(v) {}

Reference Literal::eval(Scope &scope) const {
  return value;
}

Signature::Signature(
    const Token &t,
    const std::vector<std::string> &as,
    const std::vector<std::string> &oas,
    const std::string &va):
        token(t), argnames(as), optargnames(oas), varargname(va) {}

void Signature::resolve(Scope &scope, const Args &args) {
  if (!varargname.empty()) {
    checkargsmin(argnames.size(), args);
  } else if (optargnames.empty()) {
    checkargs(argnames.size(), args);
  } else {
    checkargs(argnames.size(), argnames.size() + optargnames.size(), args);
  }

  auto argsiter = args.begin();
  for (const std::string &name: argnames) {
    scope.declare(name, *argsiter);
    ++argsiter;
  }

  for (const std::string &name: optargnames) {
    if (argsiter == args.end()) {
      scope.declare(name, nil);
    } else {
      scope.declare(name, *argsiter);
      ++argsiter;
    }
  }

  if (!varargname.empty()) {
    std::vector<Reference> varargs;
    while (argsiter != args.end()) {
      varargs.push_back(*argsiter);
      ++argsiter;
    }
    scope.declare(varargname, new List(varargs));
  }
}

FunctionDisplay::FunctionDisplay(
    const Token &t, const std::string &n, Signature *a, Ast *b):
        Ast(t), name(n), args(a), body(b) {}

Reference FunctionDisplay::eval(Scope &scope) const {
  Reference f(new UserFunction(name, this, scope));
  if (!name.empty()) {
    scope.declare(name, f);
  }
  return f;
}

ClassDisplay::ClassDisplay(
    const Token &t, const std::string &n, Arguments *bs,
    const std::set<std::string> &fs,
    const std::map<std::string,FunctionDisplay*> &ms):
        Ast(t), name(n), bases(bs), fields(fs), methods(ms) {}

Reference ClassDisplay::eval(Scope &scope) const {
  StackFrame sf(&token);

  std::vector<Reference> bases = this->bases->evalargs(scope);

  std::set<std::string> fields(this->fields);
  std::map<std::string,Method> methods;

  for (const Reference &base: bases) {
    checktype(classClass, base);
    // TODO: Check for duplicate field declarations
    for (const std::string &name: base.as<Class>()->getFields()) {
      fields.insert(name);
    }
  }

  for (const std::string &name: this->fields) {
    std::string getName = "__get_" + name;
    methods[getName] = [=](Reference owner, Class*, const Args &args) {
      checkargs(0, args);
      return owner.as<UserObject>()->getField(name);
    };
    std::string setName = "__set_" + name;
    methods[setName] = [=](Reference owner, Class*, const Args &args) {
      checkargs(1, args);
      return owner.as<UserObject>()->setField(name, args[0]);
    };
  }

  // TODO: Check for duplicates
  for (auto it = this->methods.begin(); it != this->methods.end(); ++it) {
    FunctionDisplay *fd = it->second;
    Reference parentScope(&scope);
    // TODO: Factor this with UserFunction::eval.
    methods[it->first] = [=](Reference owner, Class *sup, const Args &args) {
      Reference scope(new Scope(parentScope.as<Scope>()));
      scope.as<Scope>()->declare("this", owner);
      scope.as<Scope>()->declare("super", sup);
      fd->args->resolve(*scope.as<Scope>(), args);
      return fd->body->eval(*scope.as<Scope>());
    };
  }

  if (bases.empty()) {
    bases.push_back(classObject);
  }

  Reference cls = new Class(name, bases, true, fields, methods);
  scope.declare(name, cls);
  return cls;
}

}
