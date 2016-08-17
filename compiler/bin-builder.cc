#include "bin-builder.hh"

BinBuilder::BinBuilder()
   : _builder(bin_builder_new())
{

}

BinBuilder::~BinBuilder()
{
   bin_builder_free(_builder);
   for(std::string* s : _strs)
      delete s;
}

void BinBuilder::saveToFile(const std::string& outPath)
{
   bin_builder_save(_builder, outPath.c_str());
}

std::string BinBuilder::getUniqueLabel()
{
   return "__ref_" + std::to_string(_unique++);
}

s_bin_builder_arg BinBuilder::aLabel(const std::string& x)
{
   s_bin_builder_arg a;
   a.p_label = getLabel(x);
   return a;
}

s_bin_builder_arg BinBuilder::aAdddr(t_vm_addr x)
{
   s_bin_builder_arg a;
   a.p_addr = x;
   return a;
}

s_bin_builder_arg BinBuilder::aSadddr(t_vm_saddr x)
{
   s_bin_builder_arg a;
   a.p_saddr = x;
   return a;
}

s_bin_builder_arg BinBuilder::aInt(t_vm_int x)
{
   s_bin_builder_arg a;
   a.p_int = x;
   return a;
}

s_bin_builder_arg BinBuilder::aDouble(t_vm_double x)
{
   s_bin_builder_arg a;
   a.p_double = x;
   return a;
}

s_bin_builder_arg BinBuilder::aChar(t_vm_char x)
{
   s_bin_builder_arg a;
   a.p_char = x;
   return a;
}

s_bin_builder_arg BinBuilder::aBool(t_vm_bool x)
{
   s_bin_builder_arg a;
   a.p_bool = x;
   return a;
}

void BinBuilder::addInt(t_vm_int x)
{
   bin_builder_add_int(_builder, 0, x);
}

void BinBuilder::addInt(const std::string& label, t_vm_int x)
{
   bin_builder_add_int(_builder, getLabel(label), x);
}

void BinBuilder::addDouble(t_vm_double x)
{
   bin_builder_add_double(_builder, 0, x);
}

void BinBuilder::addDouble(const std::string& label, t_vm_double x)
{
   bin_builder_add_double(_builder, getLabel(label), x);
}

void BinBuilder::addChar(t_vm_char x)
{
   bin_builder_add_char(_builder, 0, x);
}

void BinBuilder::addChar(const std::string& label, t_vm_char x)
{
   bin_builder_add_char(_builder, getLabel(label), x);
}

void BinBuilder::addBool(t_vm_bool x)
{
   bin_builder_add_bool(_builder, 0, x);
}

void BinBuilder::addBool(const std::string& label, t_vm_bool x)
{
   bin_builder_add_bool(_builder, getLabel(label), x);
}


void BinBuilder::addString(const std::string& x)
{
   bin_builder_add_string(_builder, 0, getLabel(x));
}

void BinBuilder::addString(const std::string& label, const std::string& x)
{
   bin_builder_add_string(_builder, getLabel(label), getLabel(x));
}

std::string BinBuilder::addSharedString(const std::string& x)
{
   std::string label;
   auto it = _sharedStrs.find(x);

   if(it == _sharedStrs.end())
   {
      label = getUniqueLabel();
      _sharedStrs[x] = label;
      addString(label, x);
   }
   else
   {
      label = it->second;
   }

   return label;
}



void BinBuilder::addiNop()
{
   bin_builder_addi_nop(_builder, 0);
}

void BinBuilder::addiJump(const std::string& addrLabel)
{
   bin_builder_addi_jump(_builder, 0, getLabel(addrLabel));
}

void BinBuilder::addiCjump(t_vm_saddr saddr, const std::string& addrLabel)
{
   bin_builder_addi_cjump(_builder, 0, saddr, getLabel(addrLabel));
}

void BinBuilder::addiFjump(const std::string& addrLabel, t_vm_saddr saddr)
{
   bin_builder_addi_fjump(_builder, 0, getLabel(addrLabel), saddr);
}

void BinBuilder::addiFret()
{
   bin_builder_addi_fret(_builder, 0);
}

void BinBuilder::addiBclear(t_vm_saddr saddr, t_vm_saddr size)
{
   bin_builder_addi_bclear(_builder, 0, saddr, size);
}

void BinBuilder::addiPutnull(t_vm_saddr saddr)
{
   bin_builder_addi_putnull(_builder, 0, saddr);
}

void BinBuilder::addiPutint(t_vm_saddr saddr, t_vm_int value)
{
   bin_builder_addi_putint(_builder, 0, saddr, value);
}

void BinBuilder::addiPutdouble(t_vm_saddr saddr, t_vm_double value)
{
   bin_builder_addi_putdouble(_builder, 0, saddr, value);
}

void BinBuilder::addiPutchar(t_vm_saddr saddr, t_vm_char value)
{
   bin_builder_addi_putchar(_builder, 0, saddr, value);
}

void BinBuilder::addiPutbool(t_vm_saddr saddr, t_vm_bool value)
{
   bin_builder_addi_putbool(_builder, 0, saddr, value);
}

void BinBuilder::addiPutstring(t_vm_saddr saddr, const std::string& addrLabel,
                   t_vm_int size)
{
   bin_builder_addi_putstring(_builder, 0, saddr, getLabel(addrLabel), size);
}

void BinBuilder::addiSpup(t_vm_saddr saddr)
{
   bin_builder_addi_spup(_builder, 0, saddr);
}
void BinBuilder::addiSpdown(t_vm_saddr saddr)
{
   bin_builder_addi_spdown(_builder, 0, saddr);
}

void BinBuilder::addiSyscall(t_vm_int value)
{
   bin_builder_addi_syscall(_builder, 0, value);
}

void BinBuilder::addiPostinc(t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_postinc(_builder, 0, a1, a2);
}

void BinBuilder::addiPostdec(t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_postdec(_builder, 0, a1, a2);
}

void BinBuilder::addiPreinc(t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_preinc(_builder, 0, a1, a2);
}

void BinBuilder::addiPredec(t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_predec(_builder, 0, a1, a2);
}

void BinBuilder::addiUplus(t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_uplus(_builder, 0, a1, a2);
}

void BinBuilder::addiUminus(t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_uminus(_builder, 0, a1, a2);
}

void BinBuilder::addiLnot(t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_lnot(_builder, 0, a1, a2);
}

void BinBuilder::addiMul(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_mul(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiDiv(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_div(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiMod(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_mod(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiBplus(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bplus(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiBminus(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bminus(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiGt(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_gt(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiLt(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_lt(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiGeq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_geq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiLeq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_leq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiEq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_eq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiNeq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_neq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiLand(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_land(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiLor(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_lor(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiAssign(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_assign(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiPluseq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_pluseq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiMinuseq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_minuseq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiMuleq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_muleq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiDiveq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_diveq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiModeq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_modeq(_builder, 0, a1, a2, a3);
}





void BinBuilder::addiNop(const std::string& label)
{
   bin_builder_addi_nop(_builder, getLabel(label));
}

void BinBuilder::addiJump(const std::string& label,
                          const std::string& addrLabel)
{
   bin_builder_addi_jump(_builder, getLabel(label), getLabel(addrLabel));
}

void BinBuilder::addiCjump(const std::string& label,
                           t_vm_saddr saddr, const std::string& addrLabel)
{
   bin_builder_addi_cjump(_builder, getLabel(label), saddr, getLabel(addrLabel));
}

void BinBuilder::addiFjump(const std::string& label,
                           const std::string& addrLabel, t_vm_saddr saddr)
{
   bin_builder_addi_fjump(_builder, getLabel(label), getLabel(addrLabel), saddr);
}

void BinBuilder::addiFret(const std::string& label)
{
   bin_builder_addi_fret(_builder, getLabel(label));
}

void BinBuilder::addiBclear(const std::string& label,
                            t_vm_saddr saddr, t_vm_saddr size)
{
   bin_builder_addi_bclear(_builder, getLabel(label), saddr, size);
}

void BinBuilder::addiPutnull(const std::string& label, t_vm_saddr saddr)
{
   bin_builder_addi_putnull(_builder, getLabel(label), saddr);
}

void BinBuilder::addiPutint(const std::string& label,
                            t_vm_saddr saddr, t_vm_int value)
{
   bin_builder_addi_putint(_builder, getLabel(label), saddr, value);
}

void BinBuilder::addiPutdouble(const std::string& label,
                               t_vm_saddr saddr, t_vm_double value)
{
   bin_builder_addi_putdouble(_builder, getLabel(label), saddr, value);
}

void BinBuilder::addiPutchar(const std::string& label,
                             t_vm_saddr saddr, t_vm_char value)
{
   bin_builder_addi_putchar(_builder, getLabel(label), saddr, value);
}

void BinBuilder::addiPutbool(const std::string& label,
                             t_vm_saddr saddr, t_vm_bool value)
{
   bin_builder_addi_putbool(_builder, getLabel(label), saddr, value);
}

void BinBuilder::addiPutstring(const std::string& label,
                               t_vm_saddr saddr, const std::string& addrLabel,
                   t_vm_int size)
{
   bin_builder_addi_putstring(_builder, getLabel(label), saddr, getLabel(addrLabel), size);
}

void BinBuilder::addiSpup(const std::string& label,
                          t_vm_saddr saddr)
{
   bin_builder_addi_spup(_builder, getLabel(label), saddr);
}
void BinBuilder::addiSpdown(const std::string& label,
                            t_vm_saddr saddr)
{
   bin_builder_addi_spdown(_builder, getLabel(label), saddr);
}

void BinBuilder::addiSyscall(const std::string& label,
                             t_vm_int value)
{
   bin_builder_addi_syscall(_builder, getLabel(label), value);
}

void BinBuilder::addiPostinc(const std::string& label,
                             t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_postinc(_builder, getLabel(label), a1, a2);
}

void BinBuilder::addiPostdec(const std::string& label,
                             t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_postdec(_builder, getLabel(label), a1, a2);
}

void BinBuilder::addiPreinc(const std::string& label,
                            t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_preinc(_builder, getLabel(label), a1, a2);
}

void BinBuilder::addiPredec(const std::string& label,
                            t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_predec(_builder, getLabel(label), a1, a2);
}

void BinBuilder::addiUplus(const std::string& label,
                           t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_uplus(_builder, getLabel(label), a1, a2);
}

void BinBuilder::addiUminus(const std::string& label,
                            t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_uminus(_builder, getLabel(label), a1, a2);
}

void BinBuilder::addiLnot(const std::string& label,
                          t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_lnot(_builder, getLabel(label), a1, a2);
}

void BinBuilder::addiMul(const std::string& label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_mul(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiDiv(const std::string& label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_div(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiMod(const std::string& label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_mod(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiBplus(const std::string& label,
                           t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bplus(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiBminus(const std::string& label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bminus(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiGt(const std::string& label,
                        t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_gt(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiLt(const std::string& label,
                        t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_lt(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiGeq(const std::string& label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_geq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiLeq(const std::string& label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_leq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiEq(const std::string& label,
                        t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_eq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiNeq(const std::string& label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_neq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiLand(const std::string& label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_land(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiLor(const std::string& label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_lor(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiAssign(const std::string& label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_assign(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiPluseq(const std::string& label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_pluseq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiMinuseq(const std::string& label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_minuseq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiMuleq(const std::string& label,
                           t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_muleq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiDiveq(const std::string& label,
                           t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_diveq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiModeq(const std::string& label,
                           t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_modeq(_builder, getLabel(label), a1, a2, a3);
}



void BinBuilder::addia0(t_vm_ins code)
{
   bin_builder_addia0(_builder, 0, code);
}

void BinBuilder::addia1(t_vm_ins code, s_bin_builder_arg a1)
{
   bin_builder_addia1(_builder, 0, code, a1);
}

void BinBuilder::addia2(t_vm_ins code, s_bin_builder_arg a1,
                        s_bin_builder_arg a2)
{
   bin_builder_addia2(_builder, 0, code, a1, a2);
}

void BinBuilder::addia3(t_vm_ins code, s_bin_builder_arg a1,
                        s_bin_builder_arg a2,  s_bin_builder_arg a3)
{
   bin_builder_addia3(_builder, 0, code, a1, a2, a3);
}

void BinBuilder::addia4(t_vm_ins code, s_bin_builder_arg a1,
                        s_bin_builder_arg a2, s_bin_builder_arg a3,
                        s_bin_builder_arg a4)
{
   bin_builder_addia4(_builder, 0, code, a1, a2, a3, a4);
}

void BinBuilder::addia5(t_vm_ins code, s_bin_builder_arg a1,
                        s_bin_builder_arg a2, s_bin_builder_arg a3,
                        s_bin_builder_arg a4, s_bin_builder_arg a5)
{
   bin_builder_addia5(_builder, 0, code, a1, a2, a3, a4, a5);
}

void BinBuilder::addia0(const std::string& label, t_vm_ins code)
{
   bin_builder_addia0(_builder, getLabel(label), code);
}

void BinBuilder::addia1(const std::string& label, t_vm_ins code,
                        s_bin_builder_arg a1)
{
   bin_builder_addia1(_builder, getLabel(label), code, a1);
}

void BinBuilder::addia2(const std::string& label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2)
{
   bin_builder_addia2(_builder, getLabel(label), code, a1, a2);
}

void BinBuilder::addia3(const std::string& label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2,
                        s_bin_builder_arg a3)
{
   bin_builder_addia3(_builder, getLabel(label), code, a1, a2, a3);
}

void BinBuilder::addia4(const std::string& label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2,
                        s_bin_builder_arg a3, s_bin_builder_arg a4)
{
   bin_builder_addia4(_builder, getLabel(label), code, a1, a2, a3, a4);
}

void BinBuilder::addia5(const std::string& label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2,
                        s_bin_builder_arg a3, s_bin_builder_arg a4,
                        s_bin_builder_arg a5)
{
   bin_builder_addia5(_builder, getLabel(label), code, a1, a2, a3, a4, a5);
}



const char* BinBuilder::getLabel(const std::string& str)
{
   std::string* pstr = new std::string(str);
   _strs.push_back(pstr);
   return pstr->c_str();
}
