#include "runner.hh"
#include <stdexcept>
#include <iostream>
#include "file-parser.hh"



Runner::Runner(Scanner* scanner)
   : _scanner(scanner)
{

}

ASTProgram* Runner::getApp() const
{
   return _app;
}

void Runner::run()
{
   std::cout << "Parsing code...";
   FileParser parser(_scanner);
   _app = dynamic_cast<ASTProgram*> (parser.exec());
   std::cout << " Done !" << std::endl;

   std::cout << "Starting at main function" << std::endl;
   RunnerVisitor rv(_app, &_globalTable, this);
   std::cout << "Program completed with return code 0 " << std::endl;

   delete _app;
}


RunnerVisitor:: RunnerVisitor(AST* ast, ScopedTable* table, Runner* runner)
   : _hasResult(false), _table(table), _runner(runner)
{
   ast->accept(*this);
}

long RunnerVisitor::getResult() const
{
   if(!_hasResult)
      throw std::runtime_error{"missing result"};
   return _result;
}

void RunnerVisitor::setResult(long result)
{
   _hasResult = true;
   _result = result;
}

void RunnerVisitor::runFunction(ASTFunctionDef* f, std::vector<long> args)
{
   std::string name = f->getFunctionName();

   if(args.size() != f->getArgsSize())
   {
      throw std::runtime_error{"Function " + name
            + " called with an incorrect number of arguments"};
   }

   ScopedTable fTable(_table);
   for(size_t i = 0; i < args.size(); ++i)
      fTable.define(f->getArgName(i), args[i]);

   RunnerVisitor rv(f->getStatement(), &fTable, _runner);
   if(rv._hasResult)
      setResult(rv._result);
}

void RunnerVisitor::runFunction(ASTProgram* p, const std::string& name,
                                std::vector<long> args)
{
   ASTFunctionDef* f = nullptr;

   for(AST* child: p->getChildren())
   {
      if(child->getChild(0)->getToken().getRepresentation() == name)
      {
         f = dynamic_cast<ASTFunctionDef*> (child);
         break;
      }
   }

   if(f)
   {
      runFunction(f, args);
      return;
   }

   auto it = _native.find(name);
   if(it == _native.end())
      throw std::runtime_error{"Undefined function " + name};

   setResult(it->second(args));
}

void RunnerVisitor::visit(ASTOp1Plus* e)
{
   RunnerVisitor v1(e->getChild(0), _table, _runner);
   setResult(v1.getResult());
}

void RunnerVisitor::visit(ASTOp1Minus* e)
{
   RunnerVisitor v1(e->getChild(0), _table, _runner);
   setResult(-v1.getResult());
}

void RunnerVisitor::visit(ASTOp2Plus* e)
{
   RunnerVisitor v1(e->getChild(0), _table, _runner);
   RunnerVisitor v2(e->getChild(1), _table, _runner);
   setResult(v1.getResult() + v2.getResult());
}

void RunnerVisitor::visit(ASTOp2Minus* e)
{
   RunnerVisitor v1(e->getChild(0), _table, _runner);
   RunnerVisitor v2(e->getChild(1), _table, _runner);
   setResult(v1.getResult() - v2.getResult());
}

void RunnerVisitor::visit(ASTOp2Mul* e)
{
   RunnerVisitor v1(e->getChild(0), _table, _runner);
   RunnerVisitor v2(e->getChild(1), _table, _runner);
   setResult(v1.getResult() * v2.getResult());
}

void RunnerVisitor::visit(ASTOp2Div* e)
{
   RunnerVisitor v1(e->getChild(0), _table, _runner);
   RunnerVisitor v2(e->getChild(1), _table, _runner);
   setResult(v1.getResult() / v2.getResult());
}

void RunnerVisitor::visit(ASTOp2Mod* e)
{
   RunnerVisitor v1(e->getChild(0), _table, _runner);
   RunnerVisitor v2(e->getChild(1), _table, _runner);
   setResult(v1.getResult() % v2.getResult());
}

void RunnerVisitor::visit(ASTIntValue* e)
{
   setResult(e->getValue());
}

void RunnerVisitor::visit(ASTVarValue* e)
{
   setResult(_table->get(e->getVar()));
}

void RunnerVisitor::visit(ASTStatementsList* e)
{
   for(AST* child: e->getChildren())
   {
      RunnerVisitor rv(child, _table, _runner);
      if(rv._hasResult)
      {
         setResult(rv._result);
         break;
      }
   }
}

void RunnerVisitor::visit(ASTStatementEmpty*)
{

}

void RunnerVisitor::visit(ASTStatementLet* e)
{
   AST* astVar = e->getChild(0);
   AST* astExpr = e->getChild(1);
   RunnerVisitor rv(astExpr, _table, _runner);

   _table->define(astVar->getToken().getRepresentation(), rv.getResult());
}

void RunnerVisitor::visit(ASTStatementPrint* e)
{
   RunnerVisitor v1(e->getChild(0), _table, _runner);
   std::cout << v1.getResult() << std::endl;
}

void RunnerVisitor::visit(ASTFunctionDef*)
{
   throw std::runtime_error{"Function never directly executed"};
}

void RunnerVisitor::visit(ASTProgram* e)
{
   runFunction(e, "main", {});
}

void RunnerVisitor::visit(ASTStatementReturn* e)
{
   RunnerVisitor v1(e->getChild(0), _table, _runner);
   setResult(v1.getResult());
}

void RunnerVisitor::visit(ASTFunctionCall* e)
{
   std::vector<long> args;
   for(size_t i = 0; i < e->getArgsSize(); ++i)
   {
      AST* arg = e->getArgValue(i);
      RunnerVisitor rv(arg, _table, _runner);
      args.push_back(rv.getResult());
   }

   runFunction(_runner->getApp(), e->getFunctionName(), args);
}






static long pow(long a, long b)
{
   if(b < 0)
      return 0;

   long res = 1;
   for(long i = 0; i < b; ++i)
      res *= a;
   return res;
}

static long native_pow(const std::vector<long>& args)
{
   if(args.size() != 2)
      throw std::runtime_error{"pow expected 2 arguments"};
   return pow(args[0], args[1]);
}

std::map<std::string, f_nativeFunction> RunnerVisitor::_native = {
   {"pow", native_pow}
};
