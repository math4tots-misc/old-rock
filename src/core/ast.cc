#include "rock/core/all.hh"

#include <sstream>

#include <iostream>

namespace rock {

Ast::Ast(const Token &t): token(t) {}

Result Ast::eval(Scope&) const {
  return Result(
      Result::Type::EXCEPTION,
      new Exception(
          std::string(typeid(*this).name()) +
              " doesn't implement 'eval' yet"));
}

std::string Ast::debug() const {
  return typeid(*this).name();
}

ParseError::ParseError(const Token &t, const std::string &m):
    Ast(t), message(m) {}

Result ParseError::eval(Scope &scope) const {
  // TODO: Save token on stack, so that the location of the parse
  // error shows up on stack trace.
  return Result(
      Result::Type::EXCEPTION,
      new Exception("Parse error: " + message));
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

Result Block::eval(Scope &parentScope) const {
  Reference scope(new Scope(&parentScope));
  return evalWithoutNewScope(*scope.as<Scope>());
}

Result Block::evalWithoutNewScope(Scope &scope) const {
  Result last = Result(Result::Type::OK, nil);
  for (Ast *e: expressions) {
    last = e->eval(scope);
    if (last.type != Result::Type::OK) { return last; }
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

Result If::eval(Scope &scope) const {
  Result c = condition->eval(scope);
  if (c.type != Result::Type::OK) { return c; }

  if (c.value->truthy()) {
    return body->eval(scope);
  } else if (other) {
    return other->eval(scope);
  }
  return Result(Result::Type::OK, nil);
}

Or::Or(const Token &t, Ast *l, Ast *r): Ast(t), left(l), right(r) {}

Result Or::eval(Scope &scope) const {
  Result leftResult = left->eval(scope);
  if (leftResult.type != Result::Type::OK) { return leftResult; }
  return leftResult.value->truthy() ? leftResult : right->eval(scope);
}

And::And(const Token &t, Ast *l, Ast *r): Ast(t), left(l), right(r) {}

Result And::eval(Scope &scope) const {
  Result leftResult = left->eval(scope);
  if (leftResult.type != Result::Type::OK) { return leftResult; }
  return !leftResult.value->truthy() ? leftResult : right->eval(scope);
}

Arguments::Arguments(const Token &t, const std::vector<Ast*> &as):
    Arguments(t, as, nullptr) {}
Arguments::Arguments(const Token &t, const std::vector<Ast*> &as, Ast *va):
    token(t), args(as), vararg(va) {}
Result Arguments::evalargs(Scope &scope, std::vector<Reference> &out) const {
  if (vararg) {
    return Result(
        Result::Type::EXCEPTION,
        new Exception("splat arguments not yet supported"));
  }
  for (Ast *e: args) {
    Result r = e->eval(scope);
    if (r.type != Result::Type::OK) {
      return r;
    }
    out.push_back(r.value);
  }
  return Result(Result::Type::OK, nil);
}


MethodCall::MethodCall(
    const Token &t, Ast *o, const std::string &n, Arguments *a):
        Ast(t), owner(o), name(n), args(a) {}

Result MethodCall::eval(Scope &scope) const {
  Result result = this->owner->eval(scope);
  if (result.type != Result::Type::OK) { return result; }
  Reference owner = result.value;

  std::vector<Reference> args;
  {
    Result result = this->args->evalargs(scope, args);
    if (result.type != Result::Type::OK) { return result; }
  }

  StackFrame sf(&token);

  return owner->call(name, args);
}

Break::Break(const Token &t): Ast(t) {}

Result Break::eval(Scope &scope) const {
  return Result(Result::Type::BREAK, nil);
}

Continue::Continue(const Token &t): Ast(t) {}

Result Continue::eval(Scope &scope) const {
  return Result(Result::Type::CONTINUE, nil);
}

While::While(const Token &t, Ast *c, Ast *b):
    Ast(t), condition(c), body(b) {}

Result While::eval(Scope &scope) const {
  while (true) {
    Result condResult = this->condition->eval(scope);
    if (condResult.type != Result::Type::OK) { return condResult; }
    if (!condResult.value->truthy()) { break; }
    Result bodyResult = this->body->eval(scope);
    if (bodyResult.type == Result::Type::OK ||
        bodyResult.type == Result::Type::CONTINUE) {
      continue;
    } else if (bodyResult.type == Result::Type::BREAK) {
      break;
    } else {
      return bodyResult;
    }
  }
  return Result(Result::Type::OK, nil);
}

Return::Return(const Token &t, Ast *v): Ast(t), value(v) {}

Result Return::eval(Scope &scope) const {
  Result result = value->eval(scope);
  if (result.type != Result::Type::OK) { return result; }
  return Result(Result::Type::RETURN, result.value);
}

Declaration::Declaration(const Token &t, const std::string &n, Ast *v):
    Ast(t), name(n), value(v) {}

Result Declaration::eval(Scope &scope) const {
  if (value) {
    Result result = value->eval(scope);
    if (result.type != Result::Type::OK) { return result; }
    return scope.declare(name, result.value);
  } else {
    return scope.declare(name);
  }
}

Assignment::Assignment(const Token &t, const std::string &n, Ast *v):
    Ast(t), name(n), value(v) {}

Result Assignment::eval(Scope &scope) const {
  Result result = value->eval(scope);
  if (result.type != Result::Type::OK) { return result; }
  return scope.set(name, result.value);
}

Name::Name(const Token &t, const std::string &n):
    Ast(t), name(n) {}

Result Name::eval(Scope &scope) const {
  StackFrame sf(&token);
  return scope.get(name);
}

Literal::Literal(const Token &t, Reference v):
    Ast(t), value(v) {}

Result Literal::eval(Scope &scope) const {
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

Result FunctionDisplay::eval(Scope &scope) const {
  Reference f(new UserFunction(name, this, scope));
  if (!name.empty()) {
    Result result = scope.declare(name, f);
    if (result.type != Result::Type::OK) { return result; }
  }
  return Result(Result::Type::OK, f);
}

ClassDisplay::ClassDisplay(
    const Token &t, const std::string &n, Arguments *bs,
    const std::set<std::string> &fs,
    const std::map<std::string,FunctionDisplay*> &ms):
        Ast(t), name(n), bases(bs), fields(fs), methods(ms) {}

Result ClassDisplay::eval(Scope &scope) const {
  StackFrame sf(&token);

  std::vector<Reference> bases;
  {
    Result result = this->bases->evalargs(scope, bases);
    if (result.type != Result::Type::OK) { return result; }
  }

  std::set<std::string> fields(this->fields);
  std::map<std::string,Method> methods;

  for (const Reference &base: bases) {
    checktype(classClass, base);
    // TODO: Check for duplicate field declarations
    for (const std::string &name: base.as<Class>()->fields) {
      fields.insert(name);
    }
  }

  for (const std::string &name: this->fields) {
    std::string getName = "__get_" + name;
    methods[getName] = [=](Reference owner, const Args &args) {
      checkargs(0, args);
      return owner.as<UserObject>()->getField(name);
    };
    std::string setName = "__set_" + name;
    methods[setName] = [=](Reference owner, const Args &args) {
      checkargs(1, args);
      return owner.as<UserObject>()->setField(name, args[0]);
    };
  }

  // TODO: Check for duplicates
  for (auto it = this->methods.begin(); it != this->methods.end(); ++it) {
    FunctionDisplay *fd = it->second;
    Reference parentScope(&scope);
    // TODO: Factor this with UserFunction::eval.
    methods[it->first] = [=](Reference owner, const Args &args) {
      Reference scope(new Scope(parentScope.as<Scope>()));
      scope.as<Scope>()->declare("this", owner);
      fd->args->resolve(*scope.as<Scope>(), args);
      Result result = fd->body->eval(*scope.as<Scope>());
      switch (result.type) {
      case Result::Type::OK:
      case Result::Type::EXCEPTION:
        return result;
      case Result::Type::RETURN:
        return Result(Result::Type::OK, result.value);
      default:
        // TODO: Do something nicer
        return Result(
            Result::Type::EXCEPTION, new Exception("Invalid result type"));
      }
    };
  }

  if (bases.empty()) {
    bases.push_back(classObject);
  }

  Reference cls = new Class(name, bases, true, fields, methods);
  Result result = scope.declare(name, cls);
  if (result.type != Result::Type::OK) { return result; }
  return Result(Result::Type::OK, cls);
}

}
