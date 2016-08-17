#include "compiler.hh"
#include <cassert>
#include "ast-visitor-precheck.hh"
#include "ast-visitor-check.hh"
#include "ast-visitor-compile.hh"
#include "ast-state.hh"
#include "runtime-scope.hh"
#include "parser.hh"
#include "ast-infos.hh"

Compiler::Compiler(const std::string& path)
   : _scanner(Scanner::fromFile(path))
{
   buildTree();
}

Compiler::~Compiler()
{
   delete _state;
   delete _ast;
}

DotTree* Compiler::buildDotTree() const
{
   return ASTInfos::buildDotTree(_ast);
}

void Compiler::compileToPath(const std::string& path)
{
   buildBinaries();
   _builder.saveToFile(path);
}

void Compiler::buildTree()
{
   Parser parser(&_scanner);
   _ast = parser.buildAST();
   _state = new ASTState(_ast, &_scanner);
}

void Compiler::precheck()
{
   ASTVisitorPrecheck{_state};
   RuntimeScope* scope = _state->scope();

   if(!scope->hasFunction("main"))
      _state->tokenError("main function missing");

   ASTFunctionDef* mainFunction = scope->getFunction("main");
   if(mainFunction->argsSize() != 0)
      _state->getChild(mainFunction)->tokenError(
                 "main function must takes 0 arguments but receives "
                 + std::to_string(mainFunction->argsSize())
                 + " arguments");
}

void Compiler::check()
{
   ASTVisitorCheck {_state};
}

void Compiler::compile()
{
   _builder.addiFjump("_main", "main", 0);

   _builder.addiPutnull(1);
   _builder.addiNeq(0, 1, 1);
   _builder.addiCjump(1, "_main_end");
   _builder.addiPutint(0, 0);
   _builder.addiSyscall("_main_end", VM_SYSCALL_EXIT);

   ASTVisitorCompile {_state, &_builder};
}

void Compiler::buildBinaries()
{
   precheck();
   check();
   compile();
}
