#include "ast-visitor-compile.hh"
#include "ast-state.hh"
#include "bin-builder.hh"
#include "runtime-scope.hh"
#include "slib.hh"

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
   t_vm_saddr addr = _state->getVar(0);
   _builder->addia2(code, _builder->aSadddr(addr), _builder->aSadddr(addr));
}

void ASTVisitorCompile::compileOpb(t_vm_ins code)
{
   visitChildren();

   t_vm_saddr a1 = _state->getVar(0);
   t_vm_saddr a2 = _state->getVar(1);

   _builder->addia3(code, _builder->aSadddr(a1),_builder->aSadddr(a2),
                    _builder->aSadddr(a1));
}

void ASTVisitorCompile::compileOpIncDec(t_vm_ins code)
{
   ASTSymbolValue* right = dynamic_cast<ASTSymbolValue*>
      (_state->child(0)->ast());
   std::string name = right->getName();

   t_vm_saddr a1 =  _state->scope()->getVar(name).pos;
   t_vm_saddr a2 = _state->getVar(0);

   _builder->addia2(code, _builder->aSadddr(a1),_builder->aSadddr(a2));
}

void ASTVisitorCompile::compileAssign(t_vm_ins code)
{
   ASTState* left = _state->child(0);
   ASTState* right = _state->child(1);
   std::string name = left->ast()->getToken().getRepresentation();

   t_vm_addr a1 = _state->scope()->getVar(name).pos;
   t_vm_addr a2 = _state->getVar(0);

   ASTVisitorCompile {right, _builder};

   _builder->addia3(code, _builder->aSadddr(a1),_builder->aSadddr(a2),
                    _builder->aSadddr(a2));
}




void ASTVisitorCompile::visit(ASTInt* e)
{
   _builder->addiPutint(_state->getVar(0),
                        (t_vm_int) e->getValue());
}


void ASTVisitorCompile::visit(ASTDouble* e)
{
   _builder->addiPutdouble(_state->getVar(0),
                           (t_vm_double) e->getValue());
}

void ASTVisitorCompile::visit(ASTChar* e)
{
  _builder->addiPutchar(_state->getVar(0),
                        (t_vm_char) e->getValue());
}

void ASTVisitorCompile::visit(ASTString* e)
{
   std::string str = e->getValue();
   std::string label = _builder->addSharedString(str);
   t_vm_saddr outAddr = _state->getVar(0);
   t_vm_int size = static_cast<t_vm_int> (str.size());

   _builder->addiPutstring(outAddr, label, size);
}

void ASTVisitorCompile::visit(ASTTrue*)
{
   _builder->addiPutbool(_state->getVar(0), TRUE);
}

void ASTVisitorCompile::visit(ASTFalse*)
{
   _builder->addiPutbool(_state->getVar(0), FALSE);
}

void ASTVisitorCompile::visit(ASTNull*)
{
   _builder->addiPutnull(_state->getVar(0));
}

void ASTVisitorCompile::visit(ASTSymbol*)
{

}

void ASTVisitorCompile::visit(ASTSymbolValue* e)
{
   std::string name = e->getName();
   t_vm_saddr varPos = _state->scope()->getVar(name).pos;
   t_vm_saddr outPos = _state->getVar(0);

   _builder->addiAssign(outPos, varPos, outPos);
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
   std::string name = e->left()->getToken().getRepresentation();

   for(size_t i = 0; i < e->argsSize(); ++i)
   {
      ASTState* arg = _state->getChild(e->getArg(i));
      ASTVisitorCompile {arg, _builder};
      t_vm_saddr argIn = arg->getVar(0);
      t_vm_saddr argOut = stackSize + static_cast<t_vm_saddr> (i);
      _builder->addiAssign(argOut, argIn, argOut);
   }

   if(SLib::hasFunction(name))
   {
      SLibFunction fn = SLib::getFunction(name);
      _builder->addiSpup(stackSize);
      _builder->addiSyscall(fn.syscall);
      _builder->addiSpdown(stackSize);
   }

   else
   {
      _builder->addiFjump(name, stackSize);
   }

   _builder->addiAssign(outAddr, stackSize, outAddr);
   _builder->addiBclear(stackSize, 1);
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

void ASTVisitorCompile::visit(ASTStatementsBlock*)
{
   visitChildren();
}

void ASTVisitorCompile::visit(ASTStatementEmpty*)
{

}

void ASTVisitorCompile::visit(ASTStatementDefine* e)
{
   ASTState* value = _state->getChild(e->getValue());
   std::string name = e->getSymbol()->getName();
   t_vm_addr varAddr = _state->scope()->getVar(name).pos;

   ASTVisitorCompile {value, _builder};
   t_vm_addr valAddr = value->getVar(0);


   _builder->addiAssign(varAddr, valAddr, varAddr);
}

void ASTVisitorCompile::visit(ASTStatementReturn* e)
{
   ASTState* value = _state->getChild(e->getValue());
   ASTVisitorCompile {value, _builder};
   t_vm_addr valAddr = value->getVar(0);
   t_vm_saddr stackSize = _state->frame()->getSize();


   _builder->addiAssign(0, valAddr, 0);
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
   std::string conditionLabel = _builder->getUniqueLabel();
   std::string endLabel = _builder->getUniqueLabel();
   ASTState* condition = _state->getChild(e->getCondition());
   ASTState* block = _state->getChild(e->getWhileStatement());

   _builder->addiNop(conditionLabel);

   ASTVisitorCompile  {condition, _builder};
   t_vm_saddr addr = condition->getVar(0);

   _builder->addiLnot(addr, addr);
   _builder->addiCjump(addr, endLabel);

   ASTVisitorCompile {block, _builder};
   _builder->addiJump(conditionLabel);

   _builder->addiNop(endLabel);
}

void ASTVisitorCompile::visit(ASTFunctionDef* e)
{
   std::string name = e->getName()->getName();
   ASTState* block = _state->getChild(e->getStatement());
   t_vm_saddr stackSize = _state->frame()->getSize();

   _builder->addiNop(name);

   ASTVisitorCompile {block, _builder};

   _builder->addiBclear(0, stackSize);
   _builder->addiFret();
}

void ASTVisitorCompile::visit(ASTModule*)
{
   visitChildren();
}
