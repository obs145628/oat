#ifndef COMPILER_HH_
# define COMPILER_HH_

# include <string>
# include "scanner.hh"
# include "bin-builder.hh"

class AST;
class ASTState;
class DotTree;

class Compiler
{

public:
   Compiler(const std::string& path);
   ~Compiler();

   DotTree* buildDotTree() const;
   void compileToPath(const std::string& path);

private:
   Scanner _scanner;
   BinBuilder _builder;
   AST* _ast;
   ASTState* _state;

   void buildTree();
   void precheck();
   void check();
   void compile();

   void buildBinaries();

};

#endif //!COMPILER_HH_
