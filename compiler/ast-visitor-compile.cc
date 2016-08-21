#include "ast-visitor-compile.hh"
#include "ast-state.hh"
#include "bin-builder.hh"
#include "runtime-scope.hh"
#include "slib.hh"
#include "logs.hh"
#include "../vm/dvar.h"

ASTVisitorCompile::ASTVisitorCompile(ASTState* state, BinBuilder* builder)
   : _state(state), _builder(builder)
{
   _state->ast()->accept(*this);
}

void ASTVisitorCompile::visitChildren()
{
   for(ASTState* child: _state->children())
   {
      ASTVisitorCompile {child, _builder};
   }
}


void ASTVisitorCompile::compileOpu(t_vm_ins code)
{
   visitChildren();

   t_vm_saddr dst = _state->getVar(0);
   t_vm_saddr op = _state->getVar(1);

   _builder->addia2(code, _builder->aSadddr(op), _builder->aSadddr(dst));
}

void ASTVisitorCompile::compileOpb(t_vm_ins code)
{
   visitChildren();

   t_vm_saddr dst = _state->getVar(0);
   t_vm_saddr op1 = _state->getVar(1);
   t_vm_saddr op2 = _state->getVar(2);

   _builder->addia3(code, _builder->aSadddr(op1),_builder->aSadddr(op2),
                    _builder->aSadddr(dst));
}

void ASTVisitorCompile::compileOpIncDec(t_vm_ins code)
{
   visitChildren();

   t_vm_saddr dst = _state->getVar(0);
   t_vm_saddr op = _state->getVar(1);

   _builder->addia2(code, _builder->aSadddr(op), _builder->aSadddr(dst));
}

void ASTVisitorCompile::compileAssign(t_vm_ins code)
{
   visitChildren();

   t_vm_saddr dst = _state->getVar(0);
   t_vm_saddr op1 = _state->getVar(1);
   t_vm_saddr op2 = _state->getVar(2);

   _builder->addia3(code, _builder->aSadddr(op1),_builder->aSadddr(op2),
                    _builder->aSadddr(dst));
}

ASTState* ASTVisitorCompile::getParentLoop()
{
   ASTState* parent = _state->parent();
   while(parent->type() != ASTType::statement_while
         && parent->type() != ASTType::statement_do
         && parent->type() != ASTType::statement_for)
      parent = parent->parent();
   return parent;
}




void ASTVisitorCompile::visit(ASTInt* e)
{
   _builder->addiPutint(_state->getVar(0), DVAR_MVAR,
                        (t_vm_int) e->getValue());
}


void ASTVisitorCompile::visit(ASTDouble* e)
{
   _builder->addiPutdouble(_state->getVar(0), DVAR_MVAR,
                           (t_vm_double) e->getValue());
}

void ASTVisitorCompile::visit(ASTChar* e)
{
   _builder->addiPutchar(_state->getVar(0), DVAR_MVAR,
                        (t_vm_char) e->getValue());
}

void ASTVisitorCompile::visit(ASTString* e)
{
   std::string str = e->getValue();
   std::string label = _builder->addSharedString(str);
   t_vm_saddr outAddr = _state->getVar(0);
   t_vm_int size = static_cast<t_vm_int> (str.size());

   _builder->addiPutstring(outAddr, DVAR_MVAR, label, size);
}

void ASTVisitorCompile::visit(ASTTrue*)
{
   _builder->addiPutbool(_state->getVar(0), DVAR_MVAR, TRUE);
}

void ASTVisitorCompile::visit(ASTFalse*)
{
   _builder->addiPutbool(_state->getVar(0), DVAR_MVAR, FALSE);
}

void ASTVisitorCompile::visit(ASTNull*)
{
   _builder->addiPutnull(_state->getVar(0), DVAR_MVAR);
}

void ASTVisitorCompile::visit(ASTSymbol*)
{

}

void ASTVisitorCompile::visit(ASTSymbolValue* e)
{
   std::string name = e->getName();
   t_vm_saddr dst = _state->getVar(0);

   if(_state->scope()->hasVar(name))
   {
      t_vm_saddr src = _state->scope()->getVar(name).pos;
      _builder->addiPutref(dst, src);
   }

   else if(_state->scope()->hasGlobal(name))
   {
      GlobalVar g = _state->scope()->getGlobal(name);
      std::string srcLabel = g.label;
      _builder->addiLoad(dst, srcLabel);
   }

   else if(_state->scope()->hasFunction(name))
   {
      ASTFunctionDef* f = _state->scope()->getFunction(name);
      ASTState* fState = _state->root()->getChild(f);
      std::string label =  fState->getLabel(0);
      _builder->addiPutfunction(dst, DVAR_MVAR, label);
   }

   else if(SLib::hasFunction(name))
   {
      SLibFunction f = SLib::getFunction(name);
      t_vm_int syscall = f.syscall;
      _builder->addiPutsyscall(dst, DVAR_MVAR, syscall);
   }

   else
   {
      //should never happen
      _state->tokenError("Unknown symbol_value type");
   }
}

void ASTVisitorCompile::visit(ASTOp1Plus*)
{
   compileOpu(VM_INS_UPLUS);
}

void ASTVisitorCompile::visit(ASTOp1Minus*)
{
   compileOpu(VM_INS_UMINUS);
}

void ASTVisitorCompile::visit(ASTOp2Plus*)
{
   compileOpb(VM_INS_BPLUS);
}

void ASTVisitorCompile::visit(ASTOp2Minus*)
{
   compileOpb(VM_INS_BMINUS);
}

void ASTVisitorCompile::visit(ASTOp2Mul*)
{
   compileOpb(VM_INS_MUL);
}

void ASTVisitorCompile::visit(ASTOp2Div*)
{
   compileOpb(VM_INS_DIV);
}

void ASTVisitorCompile::visit(ASTOp2Mod*)
{
   compileOpb(VM_INS_MOD);
}

void ASTVisitorCompile::visit(ASTOpPreInc*)
{
   compileOpIncDec(VM_INS_PREINC);
}

void ASTVisitorCompile::visit(ASTOpPreDec*)
{
   compileOpIncDec(VM_INS_PREDEC);
}

void ASTVisitorCompile::visit(ASTOpPostInc*)
{
   compileOpIncDec(VM_INS_POSTINC);
}

void ASTVisitorCompile::visit(ASTOpPostDec*)
{
   compileOpIncDec(VM_INS_POSTDEC);
}

void ASTVisitorCompile::visit(ASTOpCall* e)
{
   t_vm_saddr stackSize = _state->frame()->getSize();
   t_vm_saddr outAddr = _state->getVar(0);
   ASTState* left = _state->child(0);

   ASTVisitorCompile {left, _builder};

   for(size_t i = 0; i < e->argsSize(); ++i)
   {
      ASTState* arg = _state->getChild(e->getArg(i));
      ASTVisitorCompile {arg, _builder};
   }

   for(size_t i = 0; i < e->argsSize(); ++i)
   {
      t_vm_saddr argIn = _state->getVar(i + 1);
      t_vm_saddr argOut = stackSize + static_cast<t_vm_saddr> (i);
      _builder->addiMove(argOut, argIn);
   }

   t_vm_int nbArgs = static_cast<t_vm_int> (e->argsSize());
   _builder->addiFcall(outAddr, stackSize, nbArgs);
   _builder->addiMove(outAddr, stackSize);
}

void ASTVisitorCompile::visit(ASTOpEq*)
{
   compileOpb(VM_INS_EQ);
}

void ASTVisitorCompile::visit(ASTOpNeq*)
{
   compileOpb(VM_INS_NEQ);
}

void ASTVisitorCompile::visit(ASTOpGt*)
{
   compileOpb(VM_INS_GT);
}

void ASTVisitorCompile::visit(ASTOpLt*)
{
   compileOpb(VM_INS_LT);
}

void ASTVisitorCompile::visit(ASTOpGeq*)
{
   compileOpb(VM_INS_GEQ);
}

void ASTVisitorCompile::visit(ASTOpLeq*)
{
   compileOpb(VM_INS_LEQ);
}

void ASTVisitorCompile::visit(ASTOpLand*)
{
   compileOpb(VM_INS_LAND);
}

void ASTVisitorCompile::visit(ASTOpLor*)
{
   compileOpb(VM_INS_LOR);
}

void ASTVisitorCompile::visit(ASTOpLnot*)
{
   compileOpu(VM_INS_LNOT);
}

void ASTVisitorCompile::visit(ASTOpLshift*)
{
   compileOpb(VM_INS_LSHIFT);
}

void ASTVisitorCompile::visit(ASTOpRshift*)
{
   compileOpb(VM_INS_RSHIFT);
}

void ASTVisitorCompile::visit(ASTOpBand*)
{
   compileOpb(VM_INS_BAND);
}

void ASTVisitorCompile::visit(ASTOpBxor*)
{
   compileOpb(VM_INS_BXOR);
}

void ASTVisitorCompile::visit(ASTOpBor*)
{
   compileOpb(VM_INS_BOR);
}

void ASTVisitorCompile::visit(ASTOpBnot*)
{
   compileOpu(VM_INS_BNOT);
}

void ASTVisitorCompile::visit(ASTOpAssign*)
{
   compileAssign(VM_INS_ASSIGN);
}

void ASTVisitorCompile::visit(ASTOpPluseq*)
{
   compileAssign(VM_INS_PLUSEQ);
}

void ASTVisitorCompile::visit(ASTOpMinuseq*)
{
   compileAssign(VM_INS_MINUSEQ);
}

void ASTVisitorCompile::visit(ASTOpMuleq*)
{
   compileAssign(VM_INS_MULEQ);
}

void ASTVisitorCompile::visit(ASTOpDiveq*)
{
   compileAssign(VM_INS_DIVEQ);
}

void ASTVisitorCompile::visit(ASTOpModeq*)
{
   compileAssign(VM_INS_MODEQ);
}

void ASTVisitorCompile::visit(ASTOpLshifteq*)
{
   compileAssign(VM_INS_LSHIFTEQ);
}

void ASTVisitorCompile::visit(ASTOpRshifteq*)
{
   compileAssign(VM_INS_RSHIFTEQ);
}

void ASTVisitorCompile::visit(ASTOpBandeq*)
{
   compileAssign(VM_INS_BANDEQ);
}

void ASTVisitorCompile::visit(ASTOpBxoreq*)
{
   compileAssign(VM_INS_BXOREQ);
}

void ASTVisitorCompile::visit(ASTOpBoreq*)
{
   compileAssign(VM_INS_BOREQ);
}

void ASTVisitorCompile::visit(ASTOpTernary*)
{
   visitChildren();

   t_vm_saddr dst = _state->getVar(0);
   t_vm_saddr a1 = _state->getVar(1);
   t_vm_saddr a2 = _state->getVar(2);
   t_vm_saddr a3 = _state->getVar(3);

   _builder->addia4(VM_INS_TERNARY, _builder->aSadddr(a1),_builder->aSadddr(a2),
                    _builder->aSadddr(a3), _builder->aSadddr(dst));
}

void ASTVisitorCompile::visit(ASTOpSubscript*)
{
   compileOpb(VM_INS_SUBSCRIPT);
}

void ASTVisitorCompile::visit(ASTOpMember* e)
{
   std::string member = e->getMember();
   t_vm_int len = static_cast<t_vm_int> (member.size());
   std::string label = _builder->addSharedString(member);
   visitChildren();

   t_vm_saddr dst = _state->getVar(0);
   t_vm_saddr op = _state->getVar(1);

   _builder->addiMember(op, label, len, dst);
}

void ASTVisitorCompile::visit(ASTOpNew*)
{
   //TODO: implement new operator
   _builder->addiPutnull(_state->getVar(0), DVAR_MVAR);
}


void ASTVisitorCompile::visit(ASTStatementsBlock*)
{
   visitChildren();
}

void ASTVisitorCompile::visit(ASTStatementEmpty*)
{

}

void ASTVisitorCompile::visit(ASTStatementDefine* e)
{
   std::string name = e->getSymbol()->getName();
   t_vm_addr varAddr = _state->scope()->getVar(name).pos;
   t_vm_mode mode = e->isConst() ? DVAR_MCONST : DVAR_MVAR;

   if(e->hasValue())
   {
      ASTState* value = _state->getChild(e->getValue());
      ASTVisitorCompile {value, _builder};
      t_vm_addr valAddr = _state->getVar(0);
      _builder->addiPutvar(varAddr, mode, valAddr);
   }

   else
   {
      _builder->addiPutnull(varAddr, mode);
   }
}

void ASTVisitorCompile::visit(ASTStatementReturn* e)
{
   ASTState* value = _state->getChild(e->getValue());
   ASTVisitorCompile {value, _builder};
   t_vm_addr valAddr = value->getVar(0);
   t_vm_saddr stackSize = _state->frame()->getSize();


   _builder->addiMove(0, valAddr);
   _builder->addiBclear(1, stackSize - 1);
   _builder->addiFret();
}

void ASTVisitorCompile::visit(ASTStatementIf* e)
{
   std::string elseLabel = _builder->getUniqueLabel();
   std::string endLabel = _builder->getUniqueLabel();
   ASTState* condition = _state->getChild(e->getCondition());
   ASTState* ifBlock = _state->getChild(e->getIfStatement());

   ASTVisitorCompile {condition, _builder};
   t_vm_saddr addr = condition->getVar(0);

   _builder->addiLnot(addr, addr);
   _builder->addiCjump(addr, elseLabel);
   ASTVisitorCompile {ifBlock, _builder};
   _builder->addiJump(endLabel);

   _builder->addiNop(elseLabel);
   if(e->hasElseStatement())
      ASTVisitorCompile {_state->getChild(e->getElseStatement()), _builder};

   _builder->addiNop(endLabel);
}

void ASTVisitorCompile::visit(ASTStatementWhile* e)
{
   std::string conditionLabel = _builder->getUniqueLabel("while_condition_");
   std::string endLabel = _builder->getUniqueLabel("while_end_");
   ASTState* condition = _state->getChild(e->getCondition());
   ASTState* block = _state->getChild(e->getWhileStatement());
   t_vm_saddr temp = _state->getVar(0);

   _state->addLabel(conditionLabel);
   _state->addLabel(endLabel);

   _builder->addiNop(conditionLabel);

   ASTVisitorCompile  {condition, _builder};
   t_vm_saddr addr = condition->getVar(0);

   _builder->addiLnot(addr, temp);
   _builder->addiCjump(temp, endLabel);

   ASTVisitorCompile {block, _builder};
   _builder->addiJump(conditionLabel);

   _builder->addiNop(endLabel);
}

void ASTVisitorCompile::visit(ASTStatementDo* e)
{
   std::string beginLabel = _builder->getUniqueLabel("do_begin_");
   std::string conditionLabel = _builder->getUniqueLabel("do_condition_");
   std::string endLabel = _builder->getUniqueLabel("do_end_");
   ASTState* condition = _state->getChild(e->getCondition());
   ASTState* block = _state->getChild(e->getDoStatement());
   t_vm_saddr addr = condition->getVar(0);

   _state->addLabel(conditionLabel);
   _state->addLabel(endLabel);


   _builder->addiNop(beginLabel);
   ASTVisitorCompile {block, _builder};

   _builder->addiNop(conditionLabel);
   ASTVisitorCompile  {condition, _builder};
   _builder->addiCjump(addr, beginLabel);

   _builder->addiNop(endLabel);
}

void ASTVisitorCompile::visit(ASTStatementFor* e)
{
   std::string conditionLabel = _builder->getUniqueLabel("for_condition_");
   std::string endLabel = _builder->getUniqueLabel("for_end_");
   std::string incLabel = _builder->getUniqueLabel("for_inc_");
   ASTState* init = _state->getChild(e->getInit());
   ASTState* condition = _state->getChild(e->getCondition());
   ASTState* inc = _state->getChild(e->getInc());
   ASTState* block = _state->getChild(e->getForStatement());
   t_vm_saddr temp = _state->getVar(0);
   t_vm_saddr addr = condition->getVar(0);

   _state->addLabel(incLabel);
   _state->addLabel(endLabel);

   ASTVisitorCompile {init, _builder};
   _builder->addiNop(conditionLabel);

   ASTVisitorCompile  {condition, _builder};


   _builder->addiLnot(addr, temp);
   _builder->addiCjump(temp, endLabel);

   ASTVisitorCompile {block, _builder};

   _builder->addiNop(incLabel);
   ASTVisitorCompile {inc, _builder};
   _builder->addiJump(conditionLabel);

   _builder->addiNop(endLabel);
}

void ASTVisitorCompile::visit(ASTStatementBreak*)
{
   ASTState* loop = getParentLoop();
   std::string endLabel = loop->getLabel(1);
   _builder->addiJump(endLabel);
}

void ASTVisitorCompile::visit(ASTStatementContinue*)
{
   ASTState* loop = getParentLoop();
   std::string conditionLabel = loop->getLabel(0);
   _builder->addiJump(conditionLabel);
}

void ASTVisitorCompile::visit(ASTModule*)
{
   ASTState* main = _state->getChild(_state->scope()->getFunction("main"));
   std::string mainLabel = main->getLabel(0);

   if(LOG_COMPILE)
      std::cout << "compile: create internal _main function" << std::endl;

   _builder->addiNop("_main");

   for(ASTState* child: _state->children())
   {
      if(child->type() == ASTType::global_def)
         ASTVisitorCompile {child, _builder};
   }

   t_vm_saddr scopeSize = _state->frame()->getSize();
   t_vm_saddr temp1 = _state->getVar(0);
   t_vm_saddr temp2 = _state->getVar(1);

   _builder->addiFjump(mainLabel, scopeSize);
   _builder->addiPutnull(temp1, DVAR_MVAR);
   _builder->addiNeq(scopeSize, temp1, temp2);
   _builder->addiCjump(temp2, "_main_end");
   _builder->addiPutint(scopeSize, DVAR_MVAR, 0);
   _builder->addiNop("_main_end");
   _builder->addiMove(0, scopeSize);
   _builder->addiSyscall(VM_SYSCALL_EXIT);

   for(ASTState* child: _state->children())
   {
      if(child->type() == ASTType::function_def)
         ASTVisitorCompile {child, _builder};
   }

   if(LOG_COMPILE)
      std::cout << "compile: done !" << std::endl;
}


void ASTVisitorCompile::visit(ASTFunctionDef* e)
{
   std::string name = e->getName()->getName();
   ASTState* block = _state->getChild(e->getStatement());
   t_vm_saddr stackSize = _state->frame()->getSize();
   std::string label = _state->getLabel(0);

   if(LOG_COMPILE)
      std::cout << "compile: function " + name << std::endl;

   _builder->addiNop(label);

   ASTVisitorCompile {block, _builder};

   _builder->addiBclear(0, stackSize);
   _builder->addiFret();
}

void ASTVisitorCompile::visit(ASTGlobalDef* e)
{
   std::string name = e->getSymbol()->getName();
   GlobalVar g = _state->scope()->getGlobal(name);
   t_vm_mode mode = g.mode;
   std::string label = g.label;

   t_vm_saddr temp = _state->getVar(0);

   if(LOG_COMPILE)
      std::cout << "compile: global variable " + name << std::endl;

   _builder->addVar(label);

   if(e->hasValue())
   {
      ASTState* value = _state->getChild(e->getValue());
      ASTVisitorCompile {value, _builder};
      t_vm_saddr result = value->getVar(0);
      _builder->addiPutvar(temp, mode, result);
   }

   else
   {
      _builder->addiPutnull(temp, mode);
   }

;
   _builder->addiInit(temp, label);
}
