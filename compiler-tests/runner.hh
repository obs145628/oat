#ifndef RUNNER_HH_
# define RUNNER_HH_

#include "ast-visitor.hh"
#include "dot-tree.hh"
#include "scoped-table.hh"
#include <vector>
#include <map>
#include "scanner.hh"

typedef long (*f_nativeFunction)(const std::vector<long>& unit);

class Runner
{
public:
   Runner(Scanner* scanner);

   ASTProgram* getApp() const;

   void run();

private:
   Scanner* _scanner;
   ASTProgram* _app;
   ScopedTable _globalTable;
};

class RunnerVisitor : public ASTVisitor
{
public:
   RunnerVisitor(AST* ast, ScopedTable* table, Runner* runner);

   long getResult() const;

   virtual void visit(ASTOp1Plus* e) override;
   virtual void visit(ASTOp1Minus* e) override;
   virtual void visit(ASTOp2Plus* e) override;
   virtual void visit(ASTOp2Minus* e) override;
   virtual void visit(ASTOp2Mul* e) override;
   virtual void visit(ASTOp2Div* e) override;
   virtual void visit(ASTOp2Mod* e) override;
   virtual void visit(ASTIntValue* e) override;
   virtual void visit(ASTVarValue* e) override;
   virtual void visit(ASTStatementsList* e) override;
   virtual void visit(ASTStatementEmpty* e) override;
   virtual void visit(ASTStatementLet* e) override;
   virtual void visit(ASTStatementPrint* e) override;
   virtual void visit(ASTFunctionDef* e) override;
   virtual void visit(ASTProgram* e) override;
   virtual void visit(ASTStatementReturn* e) override;
   virtual void visit(ASTFunctionCall* e) override;


private:
   bool _hasResult;
   long _result;
   ScopedTable* _table;
   Runner* _runner;

   void setResult(long result);

   void runFunction(ASTFunctionDef* f, std::vector<long> args);
   void runFunction(ASTProgram* p, const std::string& name,
                    std::vector<long> args);

   static std::map<std::string, f_nativeFunction> _native;

};



# endif //!RUNNER_HH_
