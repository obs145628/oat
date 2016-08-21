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
   ASTVisitorPrecheck {_state, &_builder};
}

void Compiler::check()
{
   ASTVisitorCheck {_state, &_builder};
}

void Compiler::compile()
{
   ASTVisitorCompile {_state, &_builder};
}

void Compiler::buildBinaries()
{
   precheck();
   check();
   compile();
}
