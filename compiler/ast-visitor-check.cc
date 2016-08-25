#include "ast-visitor-check.hh"
#include "ast-state.hh"
#include "runtime-scope.hh"
#include "../vm/dvar.h"
#include "slib.hh"
#include "logs.hh"
#include "bin-builder.hh"

#include <iostream>

ASTVisitorCheck::ASTVisitorCheck(ASTState* state, BinBuilder* builder)
   : _state(state), _builder(builder)
{
   _state->ast()->accept(*this);
}


void ASTVisitorCheck::visitChildren()
{
   for(ASTState* child: _state->children())
      ASTVisitorCheck {child, _builder};
}

void ASTVisitorCheck::checkCollection()
{
   t_vm_saddr size = static_cast<t_vm_saddr>(_state->ast()->size());
   t_vm_saddr arr = _state->frame()->addBlock(size);
   t_vm_saddr out = _state->frame()->addVar();
   _state->addVar(out);
   _state->addVar(arr);

   for(ASTState* child : _state->children())
   {
      ASTVisitorCheck {child, _builder};
      child->frame()->removeVar(child->getVar(0));
   }

   _state->frame()->removeBlock(arr, size);
}

void ASTVisitorCheck::checkOpu()
{
   visitChildren();
   ASTState* op = _state->child(0);
   _state->addVar(_state->frame()->addVar());
   _state->addVar(op->getVar(0));
   op->frame()->removeVar(op->getVar(0));
}

void ASTVisitorCheck::checkOpb()
{
   visitChildren();
   ASTState* op1 = _state->child(0);
   ASTState* op2 = _state->child(1);

   _state->addVar(_state->frame()->addVar());
   _state->addVar(op1->getVar(0));
   _state->addVar(op2->getVar(0));

   op1->frame()->removeVar(op1->getVar(0));
   op2->frame()->removeVar(op2->getVar(0));
}

void ASTVisitorCheck::checkIncDec()
{
   visitChildren();
   ASTState* op = _state->child(0);
   _state->addVar(_state->frame()->addVar());
   _state->addVar(op->getVar(0));
   op->frame()->removeVar(op->getVar(0));
}

void ASTVisitorCheck::checkAssign()
{
   visitChildren();
   ASTState* op1 = _state->child(0);
   ASTState* op2 = _state->child(1);

   _state->addVar(_state->frame()->addVar());
   _state->addVar(op1->getVar(0));
   _state->addVar(op2->getVar(0));

   op1->frame()->removeVar(op1->getVar(0));
   op2->frame()->removeVar(op2->getVar(0));
}

void ASTVisitorCheck::checkLoop()
{
   t_vm_saddr temp = _state->frame()->addVar();
   _state->addVar(temp);
   visitChildren();
   _state->frame()->removeVar(temp);
}

bool ASTVisitorCheck::inLoop()
{
   ASTState* parent = _state->parent();
   while(parent && parent->type() != ASTType::statement_while
         && parent->type() != ASTType::statement_do
         && parent->type() != ASTType::statement_for)
      parent = parent->parent();
   return !!parent;
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

   if(LOG_CHECK)
      std::cout << "check symbol_value: " << name << std::endl;


   if(!scope->hasVar(name) && !scope->hasGlobalSymbol(name))
   {
      _state->tokenError("Symbol " + name + " is undefined");
   }

   _state->addVar(_state->frame()->addVar());
}

void ASTVisitorCheck::visit(ASTArray*)
{
   checkCollection();
}

void ASTVisitorCheck::visit(ASTSet*)
{
   checkCollection();
}

void ASTVisitorCheck::visit(ASTMap*)
{
   checkCollection();
}

void ASTVisitorCheck::visit(ASTThis*)
{
   ASTState* parent = _state->parent();
   while(parent && parent->type() != ASTType::class_method)
      parent = parent->parent();

   if(!parent)
      _state->tokenError("'this' can only be used inside a class method");

   ASTClassMethod* method = dynamic_cast<ASTClassMethod*> (parent->ast());
   if(method->isStatic())
      _state->tokenError("'this' can't be used inside a static class method");

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
   std::size_t callArgs = e->argsSize();
   ASTState* left = _state->child(0);
   ASTVisitorCheck {left, _builder};
   _state->addVar(left->getVar(0));

   for(size_t i = 0; i < callArgs; ++i)
   {
      ASTState* arg = _state->getChild(e->getArg(i));
      ASTVisitorCheck {arg, _builder};
      _state->addVar(arg->getVar(0));
   }

   for(size_t i = 0; i < callArgs; ++i)
      _state->frame()->removeVar(_state->getVar(i + 1));


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

void ASTVisitorCheck::visit(ASTOpBnot*)
{
   checkOpu();
}

void ASTVisitorCheck::visit(ASTOpLshift*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpRshift*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpBand*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpBxor*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpBor*)
{
   checkOpb();
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

void ASTVisitorCheck::visit(ASTOpLshifteq*)
{
   checkAssign();
}

void ASTVisitorCheck::visit(ASTOpRshifteq*)
{
   checkAssign();
}

void ASTVisitorCheck::visit(ASTOpBandeq*)
{
   checkAssign();
}

void ASTVisitorCheck::visit(ASTOpBxoreq*)
{
   checkAssign();
}

void ASTVisitorCheck::visit(ASTOpBoreq*)
{
   checkAssign();
}

void ASTVisitorCheck::visit(ASTOpTernary*)
{
   visitChildren();
   ASTState* op1 = _state->child(0);
   ASTState* op2 = _state->child(1);
   ASTState* op3 = _state->child(2);

   _state->addVar(_state->frame()->addVar());
   _state->addVar(op1->getVar(0));
   _state->addVar(op2->getVar(0));
   _state->addVar(op3->getVar(0));

   op1->frame()->removeVar(op1->getVar(0));
   op2->frame()->removeVar(op2->getVar(0));
   op3->frame()->removeVar(op3->getVar(0));
}

void ASTVisitorCheck::visit(ASTOpSubscript*)
{
   checkOpb();
}

void ASTVisitorCheck::visit(ASTOpMember*)
{
   visitChildren();
   ASTState* op = _state->child(0);
   _state->addVar(_state->frame()->addVar());
   _state->addVar(op->getVar(0));
   op->frame()->removeVar(op->getVar(0));
}

void ASTVisitorCheck::visit(ASTOpNew* e)
{
   std::size_t callArgs = e->argsSize();
   std::string name = e->left()->getName();

   if(LOG_CHECK)
      std::cout << "check new: " << name << std::endl;

   t_vm_saddr out = _state->frame()->addVar();
   t_vm_saddr temp = _state->frame()->addVar();
   _state->addVar(out);
   _state->addVar(temp);




   if(!SLib::hasClass(name) && !_state->scope()->hasClass(name))
   {
      _state->tokenError("new operator requires a class name");
   }

   for(size_t i = 0; i < callArgs; ++i)
   {
      ASTState* arg = _state->getChild(e->getArg(i));
      ASTVisitorCheck {arg, _builder};
      _state->addVar(arg->getVar(0));
   }

   _state->frame()->removeVar(temp);
   for(size_t i = 0; i < callArgs; ++i)
      _state->frame()->removeVar(_state->getVar(i + 2));
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


   if(LOG_CHECK)
      std::cout << "check define: " << name << std::endl;

   if(e->hasValue())
   {
      ASTState* value = _state->getChild(e->getValue());
      ASTVisitorCheck{value, _builder};
      _state->addVar(value->getVar(0));
   }

   if(_state->scope()->ownVar(name))
      symbol->tokenError("variable "
                           + name + " already defined");

   _state->scope()->defineVar(name, DVAR_TNOT, DVAR_MVAR);

   if(e->hasValue())
   {
      _state->frame()->removeVar(_state->getVar(0));
   }
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
   checkLoop();
}

void ASTVisitorCheck::visit(ASTStatementDo*)
{
   visitChildren();
}

void ASTVisitorCheck::visit(ASTStatementFor* e)
{
   ASTState* init = _state->getChild(e->getInit());
   if(init->type() == ASTType::statement_break)
      init->tokenError("break statement must be inside a loop");
   else if(init->type() == ASTType::statement_continue)
      init->tokenError("continue statement must be inside a loop");

   checkLoop();
}

void ASTVisitorCheck::visit(ASTStatementBreak*)
{
   if(!inLoop())
      _state->tokenError("break statement must be inside a loop");
}

void ASTVisitorCheck::visit(ASTStatementContinue*)
{
   if(!inLoop())
      _state->tokenError("continue statement must be inside a loop");
}

void ASTVisitorCheck::visit(ASTModule*)
{
   visitChildren();

   t_vm_saddr temp1 = _state->frame()->addVar();
   t_vm_saddr temp2 = _state->frame()->addVar();
   _state->addVar(temp1);
   _state->addVar(temp2);
   _state->frame()->removeVar(temp1);
   _state->frame()->removeVar(temp2);
}

void ASTVisitorCheck::visit(ASTFunctionDef* e)
{
   ASTStatementsBlock* block = e->getStatement();
   ASTState* blockState = _state->getChild(block);
   std::string name = e->getName()->getName();
   std::string label = _builder->getUniqueLabel(name + "_fun_");

   if(LOG_CHECK)
      std::cout << "check function_def:" << name << std::endl;


   for(size_t i = 0; i < e->argsSize(); ++i)
   {
      ASTSymbol* arg = e->getArg(i);
      blockState->scope()->defineVar(arg->getName(), DVAR_TNOT, DVAR_MVAR);
   }

   _state->addLabel(label);
   visitChildren();
}

void ASTVisitorCheck::visit(ASTGlobalDef*)
{
   visitChildren();

   t_vm_saddr temp = _state->frame()->addVar();
   _state->addVar(temp);
   _state->frame()->removeVar(temp);
}

void ASTVisitorCheck::visit(ASTClass* e)
{
   std::string name = e->getName()->getName();
   std::string nameLabel = _builder->addSharedString(name);
   _state->addLabel(nameLabel);
   visitChildren();
}

void ASTVisitorCheck::visit(ASTClassMethod* e)
{
   ASTStatementsBlock* block = e->getStatement();
   ASTState* blockState = _state->getChild(block);
   std::string name = e->getName()->getName();
   std::string label = _builder->getUniqueLabel(name + "_method_");
   std::string nameLabel = _builder->addSharedString(name);

   if(LOG_CHECK)
      std::cout << "check mthod_def:" << name << std::endl;

   blockState->frame()->addVar(); //this variable

   for(size_t i = 0; i < e->argsSize(); ++i)
   {
      ASTSymbol* arg = e->getArg(i);
      blockState->scope()->defineVar(arg->getName(), DVAR_TNOT, DVAR_MVAR);
   }

   _state->addLabel(nameLabel);
   _state->addLabel(label);
   visitChildren();

   t_vm_saddr temp = _state->frame()->addVar();
   _state->addVar(temp);
   _state->frame()->removeVar(temp);
}

void ASTVisitorCheck::visit(ASTClassVariable* e)
{
   visitChildren();
   t_vm_saddr temp = _state->frame()->addVar();
   _state->addVar(temp);
   _state->frame()->removeVar(temp);

   std::string name = e->getSymbol()->getName();
   std::string nameLabel = _builder->addSharedString(name);
   _state->addLabel(nameLabel);
}
