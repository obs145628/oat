#include "ast-visitor-check.hh"
#include "ast-state.hh"
#include "runtime-scope.hh"
#include "slib.hh"

ASTVisitorCheck::ASTVisitorCheck(ASTState* state)
   : _state(state)
{
   _state->ast()->accept(*this);
}


void ASTVisitorCheck::visitChildren()
{
   for(ASTState* child: _state->children())
      ASTVisitorCheck {child};
}


void ASTVisitorCheck::checkOpu()
{
   visitChildren();
   ASTState* op = _state->child(0);
   _state->addVar(op->getVar(0));
}

void ASTVisitorCheck::checkOpb()
{
   visitChildren();
   ASTState* op1 = _state->child(0);
   ASTState* op2 = _state->child(1);

   _state->addVar(op1->getVar(0));
   _state->addVar(op2->getVar(0));

   _state->frame()->removeVar(op2->getVar(0));
}

void ASTVisitorCheck::checkIncDec()
{
   visitChildren();
   ASTState* op = _state->child(0);

   if(op->type() != ASTType::val_symbol_val)
   {
      op->tokenError("variable name expected");
   }

   _state->addVar(op->getVar(0));
}

void ASTVisitorCheck::checkAssign()
{
   visitChildren();
   ASTState* op1 = _state->child(0);
   ASTState* op2 = _state->child(1);

   if(op1->type() != ASTType::val_symbol_val)
   {
      op1->tokenError("variable name expected");
   }

   _state->addVar(op2->getVar(0));
}


void ASTVisitorCheck::visit(ASTInt*)
{
   _state->addVar(_state->frame()->addVar());
}


void ASTVisitorCheck::visit(ASTDouble*)
{
   _state->addVar(_state->frame()->addVar());
}

void ASTVisitorCheck::visit(ASTChar*)
{
   _state->addVar(_state->frame()->addVar());
}

void ASTVisitorCheck::visit(ASTString*)
{
   _state->addVar(_state->frame()->addVar());
}

void ASTVisitorCheck::visit(ASTTrue*)
{
   _state->addVar(_state->frame()->addVar());
}

void ASTVisitorCheck::visit(ASTFalse*)
{
   _state->addVar(_state->frame()->addVar());
}

void ASTVisitorCheck::visit(ASTNull*)
{
   _state->addVar(_state->frame()->addVar());
}

void ASTVisitorCheck::visit(ASTSymbol*)
{

}

void ASTVisitorCheck::visit(ASTSymbolValue* e)
{
   std::string name = e->getName();
   RuntimeScope* scope = _state->scope();
   if(!scope->hasVar(name) && !scope->hasFunction(name))
   {
      _state->tokenError("Variable " + name + " is undefined");
   }

   _state->addVar(_state->frame()->addVar());
}

void ASTVisitorCheck::visit(ASTOp1Plus*)
{
   checkOpu();
}

void ASTVisitorCheck::visit(ASTOp1Minus*)
{
   checkOpu();
}

void ASTVisitorCheck::visit(ASTOp2Plus*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOp2Minus*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOp2Mul*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOp2Div*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOp2Mod*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpPreInc*)
{
   checkIncDec();
}

void ASTVisitorCheck::visit(ASTOpPreDec*)
{
   checkIncDec();
}

void ASTVisitorCheck::visit(ASTOpPostInc*)
{
   checkIncDec();
}

void ASTVisitorCheck::visit(ASTOpPostDec*)
{
   checkIncDec();
}

void ASTVisitorCheck::visit(ASTOpCall* e)
{
   ASTState* left = _state->getChild(e->left());

   if(left->type() != ASTType::val_symbol_val)
      left->tokenError("Function name expected");

   std::string name = left->ast()->getToken().getRepresentation();
   std::size_t callArgs = e->argsSize();
   std::size_t fnArgs;

   if(_state->scope()->hasFunction(name))
      fnArgs = _state->scope()->getFunction(name)->argsSize();
   else if(SLib::hasFunction(name))
      fnArgs = SLib::getFunction(name).nbArgs;
   else
      left->tokenError("Undefined function " + name);

   if(fnArgs != callArgs)
   {
      left->tokenError("function called with "
                           + std::to_string(callArgs)
                           +  " arguments, but expected "
                           + std::to_string(fnArgs) + " arguments" );
   }

   for(size_t i = 0; i < callArgs; ++i)
   {
      ASTState* arg = _state->getChild(e->getArg(i));
      ASTVisitorCheck {arg};
      _state->frame()->removeVar(arg->getVar(0));
   }

   _state->addVar(_state->frame()->addVar());
}

void ASTVisitorCheck::visit(ASTOpEq*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpNeq*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpGt*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpLt*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpGeq*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpLeq*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpLand*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpLor*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpLnot*)
{
   checkOpu();
}

void ASTVisitorCheck::visit(ASTOpAssign*)
{
   checkAssign();
}

void ASTVisitorCheck::visit(ASTOpPluseq*)
{
   checkAssign();
}

void ASTVisitorCheck::visit(ASTOpMinuseq*)
{
   checkAssign();
}

void ASTVisitorCheck::visit(ASTOpMuleq*)
{
   checkAssign();
}

void ASTVisitorCheck::visit(ASTOpDiveq*)
{
   checkAssign();
}

void ASTVisitorCheck::visit(ASTOpModeq*)
{
   checkAssign();
}

void ASTVisitorCheck::visit(ASTStatementsBlock*)
{
   visitChildren();
}

void ASTVisitorCheck::visit(ASTStatementEmpty*)
{

}

void ASTVisitorCheck::visit(ASTStatementDefine* e)
{
   ASTState* symbol = _state->getChild(e->getSymbol());
   std::string name = e->getSymbol()->getName();
   ASTState* value = _state->getChild(e->getValue());
   ASTVisitorCheck{value};

   if(_state->scope()->ownVar(name))
      symbol->tokenError("variable "
                           + name + " already defined");

   _state->scope()->defineVar(name, -1);
   _state->frame()->removeVar(value->getVar(0));
}

void ASTVisitorCheck::visit(ASTStatementReturn*)
{
   visitChildren();
}

void ASTVisitorCheck::visit(ASTStatementIf*)
{
   visitChildren();
}

void ASTVisitorCheck::visit(ASTStatementWhile*)
{
   visitChildren();
}

void ASTVisitorCheck::visit(ASTFunctionDef* e)
{
   ASTStatementsBlock* block = e->getStatement();
   ASTState* blockState = _state->getChild(block);

   for(size_t i = 0; i < e->argsSize(); ++i)
   {
      ASTSymbol* arg = e->getArg(i);
      blockState->scope()->defineVar(arg->getName(), -1);
   }

   visitChildren();
}

void ASTVisitorCheck::visit(ASTModule*)
{
   visitChildren();
}
