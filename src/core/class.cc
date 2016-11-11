#include "rock/core/all.hh"

#include <algorithm>

namespace rock {

Class *classClass;

namespace {
Init init(110, __FILE__, []() {
  classClass = new Class("Class", {classObject}, {
    {"__call", [](Reference owner, Class*, const Args &args) {
      Reference instance(new UserObject(owner.as<Class>()));
      instance->call("__init", args);
      return instance;
    }},
    {"extends", [](Reference owner, Class*, const Args &args) {
      checkargs(1, args);
      checktype(classClass, args[0]);
      Class *cls = owner.as<Class>();
      Class *base = args[0].as<Class>();
      return cls->extends(base) ? xtrue : xfalse;
    }},
    {"__eq", [](Reference owner, Class*, const Args &args) -> Reference {
      checkargs(1, args);
      if (!dynamic_cast<Class*>(args[0].as<Object>())) {
        return xfalse;
      }
      return Bool::from(owner.as<Class>() == args[0].as<Class>());
    }},
    {"__get_mro", [](Reference owner, Class*, const Args &args) {
      checkargs(0, args);
      List *list = new List({});
      for (Class *c: owner.as<Class>()->getMro()) {
        list->value.push_back(c);
      }
      return list;
    }},

    // This really should be a method on classClassClass
    {"of", [](Reference owner, Class*, const Args &args) {
      checkargs(1, args);
      return args[0]->getClass();
    }},
  });
  builtins->declare("Class", classClass);
});

}

bool instanceof(Reference r, const Class *c) {
  return r->getClass().as<Class>() == c;
}

std::vector<Class*>
Class::makeMro(Class *owner, const std::vector<Reference> &bases) {
  // Naive C3 linearization
  std::vector<Class*> mro;
  std::vector<Class*> bs;
  for (const Reference &base: bases) {
    bs.push_back(base.as<Class>());
  }
  std::vector<std::vector<Class*>> queue;
  for (Class *b: bs) {
    queue.push_back(b->mro);
  }
  queue.push_back(bs);
  while (any_of(
      queue.begin(), queue.end(),
      [](const std::vector<Class*> &cs) {
        return !cs.empty();
      })) {
    int i;
    for (i = 0; i < queue.size(); i++) {
      if (queue[i].empty()) {
        continue;
      }
      bool good = true;
      Class *c = queue[i].back();
      for (auto &v: queue) {
        for (int j = 0; j + 1 < v.size(); j++) {
          if (v[j] == c) {
            good = false;
            break;
          }
        }
        if (!good) {
          break;
        }
      }
      if (good) {
        break;
      }
    }
    if (i == queue.size()) {
      throw exception("No valid Method Resolution Order found");
    }
    Class *next = queue[i].back();
    for (std::vector<Class*> &v: queue) {
      if (!v.empty() && v.back() == next) {
        v.pop_back();
      }
    }
    mro.push_back(next);
  }
  mro.push_back(owner);
  std::reverse(mro.begin(), mro.end());
  return mro;
}

Class::Class(
    const std::string &n,
    const std::vector<Reference> &bs,
    const std::map<std::string,Method> &ms):
        Class(n, bs, false, {}, ms) {}
Class::Class(
    Reference c,
    const std::string &n,
    const std::vector<Reference> &bs,
    const std::map<std::string,Method> &ms):
        Class(c, n, bs, false, {}, ms) {}
Class::Class(
    const std::string &n,
    const std::vector<Reference> &bs,
    bool uc,
    const std::set<std::string> &fs,
    const std::map<std::string,Method> &ms):
        Class(nullptr, n, bs, uc, fs, ms) {}
Class::Class(
    Reference c,
    const std::string &n,
    const std::vector<Reference> &bs,
    bool uc,
    const std::set<std::string> &fs,
    const std::map<std::string,Method> &ms):
        cls(c), name(n), bases(bs), mro(makeMro(this, bs)),
        userConstructible(uc), fields(fs), methods(ms) {}

Reference Class::getClass() const {
  return cls.is_null() ? classClass : cls;
}

Method Class::getDirectMethod(const std::string &name) const {
  auto pair = methods.find(name);
  return pair != methods.end() ? pair->second : nullptr;
}

Class *Class::findClassWithMethod(const std::string &name) {
  auto pair = methods.find(name);
  if (pair != methods.end()) {
    return this;
  }
  for (Class *base: mro) {
    Method method = base->getDirectMethod(name);
    if (method) {
      return base;
    }
  }
  return nullptr;
}

const std::set<std::string> &Class::getFields() const {
  return fields;
}

const std::vector<Class*> &Class::getMro() const {
  return mro;
}

std::string Class::debug() const {
  return "<Class " + name + ">";
}

bool Class::extends(Class *base) const {
  if (this == base) {
    return true;
  }
  for (const Reference &b: bases) {
    if (b.as<Object>() == base) {
      return true;
    }
  }
  return false;
}

}
