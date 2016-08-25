#include "bin-builder.hh"
#include <stdexcept>

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
   if(bin_builder_save(_builder, outPath.c_str()))
      throw std::runtime_error{"Unable to save binaries to " + outPath};
}

std::string BinBuilder::getUniqueLabel(const std::string& prefix)
{
   return prefix + "__ref_" + std::to_string(_unique++);
}

s_bin_builder_arg BinBuilder::aLabel(const std::string& x)
{
   s_bin_builder_arg a;
   a.p_label = getLabel(x);
   a.type = 0;
   return a;
}

s_bin_builder_arg BinBuilder::aAdddr(t_vm_addr x)
{
   s_bin_builder_arg a;
   a.p_addr = x;
   a.type = 1;
   return a;
}

s_bin_builder_arg BinBuilder::aSadddr(t_vm_saddr x)
{
   s_bin_builder_arg a;
   a.p_saddr = x;
   a.type = 2;
   return a;
}

s_bin_builder_arg BinBuilder::aInt(t_vm_int x)
{
   s_bin_builder_arg a;
   a.p_int = x;
   a.type = 3;
   return a;
}

s_bin_builder_arg BinBuilder::aDouble(t_vm_double x)
{
   s_bin_builder_arg a;
   a.p_double = x;
   a.type = 4;
   return a;
}

s_bin_builder_arg BinBuilder::aChar(t_vm_char x)
{
   s_bin_builder_arg a;
   a.p_char = x;
   a.type = 5;
   return a;
}

s_bin_builder_arg BinBuilder::aBool(t_vm_bool x)
{
   s_bin_builder_arg a;
   a.p_bool = x;
   a.type = 6;
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

void BinBuilder::addSpace(t_vm_addr len)
{
   bin_builder_add_space(_builder, 0, len);
}

void BinBuilder::addSpace(const std::string& label, t_vm_addr len)
{
   bin_builder_add_space(_builder, getLabel(label), len);
}

void BinBuilder::addVar()
{
   bin_builder_add_var(_builder, 0);
}

void BinBuilder::addVar(const std::string& label)
{
   bin_builder_add_var(_builder, getLabel(label));
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

void BinBuilder::addiFcall(t_vm_saddr saddr, t_vm_saddr pos, t_vm_int size)
{
   bin_builder_addi_fcall(_builder, 0, saddr, pos, size);
}

void BinBuilder::addiFret()
{
   bin_builder_addi_fret(_builder, 0);
}

void BinBuilder::addiBclear(t_vm_saddr saddr, t_vm_saddr size)
{
   bin_builder_addi_bclear(_builder, 0, saddr, size);
}

void BinBuilder::addiPutnull(t_vm_saddr saddr, t_vm_int mode)
{
   bin_builder_addi_putnull(_builder, 0, saddr, mode);
}

void BinBuilder::addiPutint(t_vm_saddr saddr, t_vm_int mode, t_vm_int value)
{
   bin_builder_addi_putint(_builder, 0, saddr, mode, value);
}

void BinBuilder::addiPutdouble(t_vm_saddr saddr, t_vm_int mode,
                               t_vm_double value)
{
   bin_builder_addi_putdouble(_builder, 0, saddr, mode, value);
}

void BinBuilder::addiPutchar(t_vm_saddr saddr, t_vm_int mode, t_vm_char value)
{
   bin_builder_addi_putchar(_builder, 0, saddr, mode, value);
}

void BinBuilder::addiPutbool(t_vm_saddr saddr, t_vm_int mode, t_vm_bool value)
{
   bin_builder_addi_putbool(_builder, 0, saddr, mode, value);
}

void BinBuilder::addiPutstring(t_vm_saddr saddr, t_vm_int mode,
                               const std::string& addrLabel, t_vm_int size)
{
   bin_builder_addi_putstring(_builder, 0, saddr, mode,
                              getLabel(addrLabel), size);
}

void BinBuilder::addiPutfunction(t_vm_saddr saddr, t_vm_int mode,
                                 const std::string& addrLabel)
{
   bin_builder_addi_putfunction(_builder, 0, saddr, mode, getLabel(addrLabel));
}

void BinBuilder::addiPutsyscall(t_vm_saddr saddr, t_vm_int mode,
                                t_vm_int syscall)
{
   bin_builder_addi_putsyscall(_builder, 0, saddr, mode, syscall);
}

void BinBuilder::addiPutarr(t_vm_saddr saddr, t_vm_int mode,
                            t_vm_saddr it, t_vm_int size)
{
   bin_builder_addi_putarr(_builder, 0, saddr, mode, it, size);
}

void BinBuilder::addiPutset(t_vm_saddr saddr, t_vm_int mode,
                   t_vm_saddr it, t_vm_int size)
{
   bin_builder_addi_putset(_builder, 0, saddr, mode, it, size);
}

void BinBuilder::addiPutmap(t_vm_saddr saddr, t_vm_int mode,
                   t_vm_saddr it, t_vm_int size)
{
   bin_builder_addi_putmap(_builder, 0, saddr, mode, it, size);
}

void BinBuilder::addiPutobj(t_vm_saddr saddr, t_vm_int mode,
                   t_vm_int id)
{
   bin_builder_addi_putobj(_builder, 0, saddr, mode, id);
}

void BinBuilder::addiPutclass(t_vm_saddr saddr, t_vm_int mode,
                     t_vm_int id)
{
   bin_builder_addi_putclass(_builder, 0, saddr, mode, id);
}

void BinBuilder::addiPutvar(t_vm_saddr saddr, t_vm_int mode,
                            t_vm_saddr src)
{
   bin_builder_addi_putvar(_builder, 0, saddr, mode, src);
}

void BinBuilder::addiPutref(t_vm_saddr dst, t_vm_saddr src)
{
   bin_builder_addi_putref(_builder, 0, dst, src);
}

void BinBuilder::addiCopy(t_vm_saddr dst, t_vm_saddr src)
{
   bin_builder_addi_copy(_builder, 0, dst, src);
}

void BinBuilder::addiMove(t_vm_saddr dst, t_vm_saddr src)
{
   bin_builder_addi_move(_builder, 0, dst, src);
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

void BinBuilder::addiBind(t_vm_saddr dst, t_vm_saddr it, t_vm_int size)
{
   bin_builder_addi_bind(_builder, 0, dst, it, size);
}

void BinBuilder::addiLoad(t_vm_saddr dst, const std::string& srcLabel)
{
   bin_builder_addi_load(_builder, 0, dst, getLabel(srcLabel));
}

void BinBuilder::addiStore(t_vm_saddr src, const std::string& dstLabel)
{
   bin_builder_addi_store(_builder, 0, src, getLabel(dstLabel));
}

void BinBuilder::addiInit(t_vm_saddr src, const std::string& dstLabel)
{
   bin_builder_addi_init(_builder, 0, src, getLabel(dstLabel));
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

void BinBuilder::addiBnot(t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_bnot(_builder, 0, a1, a2);
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

void BinBuilder::addiLshift(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_lshift(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiRshift(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_rshift(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiBand(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_band(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiBxor(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bxor(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiBor(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bor(_builder, 0, a1, a2, a3);
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

void BinBuilder::addiLshifteq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_lshifteq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiRshifteq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_rshiftqeq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiBandeq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bandeq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiBxoreq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bxoreq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiBoreq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_boreq(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiSubscript(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_subscript(_builder, 0, a1, a2, a3);
}

void BinBuilder::addiTernary(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3,
                             t_vm_saddr a4)
{
   bin_builder_addi_ternary(_builder, 0, a1, a2, a3, a4);
}

void BinBuilder::addiMember(t_vm_saddr saddr, const std::string& strLabel,
                            t_vm_int size, t_vm_saddr res)
{
   bin_builder_addi_member(_builder, 0, saddr, getLabel(strLabel), size, res);
}

void BinBuilder::addiDefclass(const std::string& nameLabel, t_vm_int len,
                              t_vm_int id, t_vm_int parent)
{
   bin_builder_addi_defclass(_builder, 0, getLabel(nameLabel), len, id, parent);
}

void BinBuilder::addiDeffield(t_vm_int id, const std::string& nameLabel,
                              t_vm_int len, t_vm_int type,
                              t_vm_saddr value)
{
   bin_builder_addi_deffield(_builder, 0, id, getLabel(nameLabel), len,
                             type, value);
}

void BinBuilder::addiDefsfield(t_vm_int id, const std::string& nameLabel,
                               t_vm_int len, t_vm_int type,
                               t_vm_saddr value)
{
   bin_builder_addi_defsfield(_builder, 0, id, getLabel(nameLabel), len,
                              type, value);
}

void BinBuilder::addiDefend(t_vm_int id)
{
   bin_builder_addi_defend(_builder, 0, id);
}

void BinBuilder::addiSsuper(t_vm_saddr dst)
{
   bin_builder_addi_ssuper(_builder, 0, dst);
}

void BinBuilder::addiSetfscope(const std::string& nameLabel, t_vm_int len)
{
   bin_builder_addi_setfscope(_builder, 0, getLabel(nameLabel), len);
}

void BinBuilder::addiSetfline(t_vm_int line)
{
   bin_builder_addi_setfline(_builder, 0, line);
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

void BinBuilder::addiFcall(const std::string& label,
                           t_vm_saddr saddr, t_vm_saddr pos, t_vm_int size)
{
   bin_builder_addi_fcall(_builder, getLabel(label), saddr, pos, size);
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

void BinBuilder::addiPutnull(const std::string& label, t_vm_saddr saddr,
                             t_vm_int mode)
{
   bin_builder_addi_putnull(_builder, getLabel(label), saddr, mode);
}

void BinBuilder::addiPutint(const std::string& label,
                            t_vm_saddr saddr, t_vm_int mode, t_vm_int value)
{
   bin_builder_addi_putint(_builder, getLabel(label), saddr, mode, value);
}

void BinBuilder::addiPutdouble(const std::string& label,
                               t_vm_saddr saddr, t_vm_int mode,
                               t_vm_double value)
{
   bin_builder_addi_putdouble(_builder, getLabel(label), saddr, mode, value);
}

void BinBuilder::addiPutchar(const std::string& label,
                             t_vm_saddr saddr, t_vm_int mode, t_vm_char value)
{
   bin_builder_addi_putchar(_builder, getLabel(label), saddr, mode, value);
}

void BinBuilder::addiPutbool(const std::string& label,
                             t_vm_saddr saddr, t_vm_int mode, t_vm_bool value)
{
   bin_builder_addi_putbool(_builder, getLabel(label), saddr, mode, value);
}

void BinBuilder::addiPutstring(const std::string& label,
                               t_vm_saddr saddr, t_vm_int mode,
                               const std::string& addrLabel, t_vm_int size)
{
   bin_builder_addi_putstring(_builder, getLabel(label), saddr, mode,
                              getLabel(addrLabel), size);
}

void BinBuilder::addiPutfunction(const std::string& label,
                                 t_vm_saddr saddr, t_vm_int mode,
                                 const std::string& addrLabel)
{
   bin_builder_addi_putfunction(_builder, getLabel(label),
                                saddr, mode, getLabel(addrLabel));
}

void BinBuilder::addiPutsyscall(const std::string& label,
                                t_vm_saddr saddr, t_vm_int mode,
                                t_vm_int syscall)
{
   bin_builder_addi_putsyscall(_builder, getLabel(label), saddr, mode, syscall);
}

void BinBuilder::addiPutarr(const std::string& label,
                            t_vm_saddr saddr, t_vm_int mode,
                            t_vm_saddr it, t_vm_int size)
{
   bin_builder_addi_putarr(_builder, getLabel(label), saddr, mode, it, size);
}

void BinBuilder::addiPutset(const std::string& label,
                            t_vm_saddr saddr, t_vm_int mode,
                            t_vm_saddr it, t_vm_int size)
{
   bin_builder_addi_putset(_builder, getLabel(label), saddr, mode, it, size);
}

void BinBuilder::addiPutmap(const std::string& label,
                            t_vm_saddr saddr, t_vm_int mode,
                            t_vm_saddr it, t_vm_int size)
{
   bin_builder_addi_putmap(_builder, getLabel(label), saddr, mode, it, size);
}

void BinBuilder::addiPutobj(const std::string& label,
                            t_vm_saddr saddr, t_vm_int mode,
                            t_vm_int id)
{
   bin_builder_addi_putobj(_builder, getLabel(label), saddr, mode, id);
}

void BinBuilder::addiPutclass(const std::string& label,
                              t_vm_saddr saddr, t_vm_int mode,
                              t_vm_int id)
{
   bin_builder_addi_putclass(_builder, getLabel(label), saddr, mode, id);
}

void BinBuilder::addiPutvar(const std::string& label,
                            t_vm_saddr saddr, t_vm_int mode,
                            t_vm_saddr src)
{
   bin_builder_addi_putvar(_builder, getLabel(label), saddr, mode, src);
}

void BinBuilder::addiPutref(const std::string& label,
                            t_vm_saddr dst, t_vm_saddr src)
{
   bin_builder_addi_putref(_builder, getLabel(label), dst, src);
}

void BinBuilder::addiCopy(const std::string& label,
                          t_vm_saddr dst, t_vm_saddr src)
{
   bin_builder_addi_copy(_builder, getLabel(label), dst, src);
}

void BinBuilder::addiMove(const std::string& label,
                          t_vm_saddr dst, t_vm_saddr src)
{
   bin_builder_addi_move(_builder, getLabel(label), dst, src);
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

void BinBuilder::addiBind(const std::string& label,
                          t_vm_saddr dst, t_vm_saddr it, t_vm_int size)
{
   bin_builder_addi_bind(_builder, getLabel(label), dst, it, size);
}

void BinBuilder::addiLoad(const std::string& label,
                          t_vm_saddr dst, const std::string& srcLabel)
{
   bin_builder_addi_load(_builder, getLabel(label), dst, getLabel(srcLabel));
}

void BinBuilder::addiStore(const std::string& label,
                           t_vm_saddr src, const std::string& dstLabel)
{
   bin_builder_addi_store(_builder, getLabel(label), src, getLabel(dstLabel));
}

void BinBuilder::addiInit(const std::string& label,
                          t_vm_saddr src, const std::string& dstLabel)
{
   bin_builder_addi_init(_builder, getLabel(label), src, getLabel(dstLabel));
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

void BinBuilder::addiBnot(const std::string& label,
                          t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addi_bnot(_builder, getLabel(label), a1, a2);
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

void BinBuilder::addiLshift(const std::string& label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_lshift(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiRshift(const std::string& label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_rshift(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiBand(const std::string& label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_band(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiBxor(const std::string& label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bxor(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiBor(const std::string& label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bor(_builder, getLabel(label), a1, a2, a3);
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

void BinBuilder::addiLshifteq(const std::string& label,
                              t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_lshifteq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiRshifteq(const std::string& label,
                              t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_rshiftqeq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiBandeq(const std::string& label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bandeq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiBxoreq(const std::string& label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_bxoreq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiBoreq(const std::string& label,
                           t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_boreq(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiSubscript(const std::string& label,
                               t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addi_subscript(_builder, getLabel(label), a1, a2, a3);
}

void BinBuilder::addiTernary(const std::string& label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3,
                             t_vm_saddr a4)
{
   bin_builder_addi_ternary(_builder, getLabel(label), a1, a2, a3, a4);
}

void BinBuilder::addiMember(const std::string& label,
                            t_vm_saddr saddr, const std::string& strLabel,
                            t_vm_int size, t_vm_saddr res)
{
   bin_builder_addi_member(_builder, getLabel(label),
                           saddr, getLabel(strLabel), size, res);
}

void BinBuilder::addiDefclass(const std::string& label,
                              const std::string& nameLabel, t_vm_int len,
                              t_vm_int id, t_vm_int parent)
{
   bin_builder_addi_defclass(_builder, getLabel(label),
                             getLabel(nameLabel), len, id, parent);
}

void BinBuilder::addiDeffield(const std::string& label,
                              t_vm_int id, const std::string& nameLabel,
                              t_vm_int len, t_vm_int type,
                              t_vm_saddr value)
{
   bin_builder_addi_deffield(_builder, getLabel(label),
                             id, getLabel(nameLabel), len, type, value);
}

void BinBuilder::addiDefsfield(const std::string& label,
                               t_vm_int id, const std::string& nameLabel,
                               t_vm_int len, t_vm_int type,
                               t_vm_saddr value)
{
   bin_builder_addi_defsfield(_builder, getLabel(label),
                              id, getLabel(nameLabel), len, type, value);
}

void BinBuilder::addiDefend(const std::string& label,
                            t_vm_int id)
{
   bin_builder_addi_defend(_builder, getLabel(label), id);
}

void BinBuilder::addiSsuper(const std::string& label,
                            t_vm_saddr dst)
{
   bin_builder_addi_ssuper(_builder, getLabel(label), dst);
}

void BinBuilder::addiSetfscope(const std::string& label,
                               const std::string& nameLabel, t_vm_int len)
{
   bin_builder_addi_setfscope(_builder, getLabel(label),
                              getLabel(nameLabel), len);
}

void BinBuilder::addiSetfline(const std::string& label, t_vm_int line)
{
   bin_builder_addi_setfline(_builder, getLabel(label), line);
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
