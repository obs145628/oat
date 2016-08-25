#ifndef BIN_BUILDER_HH_
# define BIN_BUILDER_HH_

# include <string>
# include <vector>
# include <map>
# include "../vm/bin-builder.h"



class BinBuilder
{

public:
   BinBuilder();
   ~BinBuilder();

   BinBuilder(const BinBuilder&) = delete;

   void saveToFile(const std::string& outPath);

   std::string getUniqueLabel(const std::string& prefix = "");

   s_bin_builder_arg aLabel(const std::string& x);
   s_bin_builder_arg aAdddr(t_vm_addr x);
   s_bin_builder_arg aSadddr(t_vm_saddr x);
   s_bin_builder_arg aInt(t_vm_int x);
   s_bin_builder_arg aDouble(t_vm_double x);
   s_bin_builder_arg aChar(t_vm_char x);
   s_bin_builder_arg aBool(t_vm_bool x);

   void addInt(t_vm_int x);
   void addInt(const std::string& label, t_vm_int x);
   void addDouble(t_vm_double x);
   void addDouble(const std::string& label, t_vm_double x);
   void addChar(t_vm_char x);
   void addChar(const std::string& label, t_vm_char x);
   void addBool(t_vm_bool x);
   void addBool(const std::string& label, t_vm_bool x);
   void addString(const std::string& x);
   void addString(const std::string& label, const std::string& x);
   void addSpace(t_vm_addr len);
   void addSpace(const std::string& label, t_vm_addr len);
   void addVar();
   void addVar(const std::string& label);

   std::string addSharedString(const std::string& x);

   void addiNop();
   void addiJump(const std::string& addrLabel);
   void addiCjump(t_vm_saddr saddr, const std::string& addrLabel);
   void addiFjump(const std::string& addrLabel, t_vm_saddr saddr);
   void addiFcall(t_vm_saddr saddr, t_vm_saddr pos, t_vm_int size);
   void addiFret();
   void addiBclear(t_vm_saddr saddr, t_vm_saddr size);
   void addiPutnull(t_vm_saddr saddr, t_vm_int mode);
   void addiPutint(t_vm_saddr saddr, t_vm_int mode, t_vm_int value);
   void addiPutdouble(t_vm_saddr saddr, t_vm_int mode, t_vm_double value);
   void addiPutchar(t_vm_saddr saddr, t_vm_int mode, t_vm_char value);
   void addiPutbool(t_vm_saddr saddr, t_vm_int mode, t_vm_bool value);
   void addiPutstring(t_vm_saddr saddr, t_vm_int mode,
                      const std::string& addrLabel, t_vm_int size);
   void addiPutfunction(t_vm_saddr saddr, t_vm_int mode,
                      const std::string& addrLabel);
   void addiPutsyscall(t_vm_saddr saddr, t_vm_int mode,
                      t_vm_int syscall);
   void addiPutarr(t_vm_saddr saddr, t_vm_int mode,
                   t_vm_saddr it, t_vm_int size);
   void addiPutset(t_vm_saddr saddr, t_vm_int mode,
                   t_vm_saddr it, t_vm_int size);
   void addiPutmap(t_vm_saddr saddr, t_vm_int mode,
                   t_vm_saddr it, t_vm_int size);
   void addiPutobj(t_vm_saddr saddr, t_vm_int mode,
                   t_vm_int id);
   void addiPutclass(t_vm_saddr saddr, t_vm_int mode,
                     t_vm_int id);
   void addiPutvar(t_vm_saddr saddr, t_vm_int mode,
                   t_vm_saddr src);
   void addiPutref(t_vm_saddr dst, t_vm_saddr src);
   void addiCopy(t_vm_saddr dst, t_vm_saddr src);
   void addiMove(t_vm_saddr dst, t_vm_saddr src);
   void addiSpup(t_vm_saddr saddr);
   void addiSpdown(t_vm_saddr saddr);
   void addiSyscall(t_vm_int value);
   void addiBind(t_vm_saddr dst, t_vm_saddr it, t_vm_int size);
   void addiLoad(t_vm_saddr dst, const std::string& srcLabel);
   void addiStore(t_vm_saddr src, const std::string& dstLabel);
   void addiInit(t_vm_saddr src, const std::string& dstLabel);
   void addiPostinc(t_vm_saddr a1, t_vm_saddr a2);
   void addiPostdec(t_vm_saddr a1, t_vm_saddr a2);
   void addiPreinc(t_vm_saddr a1, t_vm_saddr a2);
   void addiPredec(t_vm_saddr a1, t_vm_saddr a2);
   void addiUplus(t_vm_saddr a1, t_vm_saddr a2);
   void addiUminus(t_vm_saddr a1, t_vm_saddr a2);
   void addiLnot(t_vm_saddr a1, t_vm_saddr a2);
   void addiBnot(t_vm_saddr a1, t_vm_saddr a2);
   void addiMul(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiDiv(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiMod(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBplus(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBminus(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiGt(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiLt(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiGeq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiLeq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiEq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiNeq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiLand(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiLor(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiLshift(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiRshift(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBand(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBxor(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBor(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiAssign(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiPluseq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiMinuseq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiMuleq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiDiveq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiModeq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiLshifteq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiRshifteq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBandeq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBxoreq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBoreq(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiSubscript(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiTernary(t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3,
                     t_vm_saddr a4);
   void addiMember(t_vm_saddr saddr, const std::string& strLabel,
                   t_vm_int size, t_vm_saddr res);
   void addiDefclass(const std::string& nameLabel, t_vm_int len,
                     t_vm_int id, t_vm_int parent);
   void addiDeffield(t_vm_int id, const std::string& nameLabel,
                     t_vm_int len, t_vm_int type,
                     t_vm_saddr value);
   void addiDefsfield(t_vm_int id, const std::string& nameLabel,
                      t_vm_int len, t_vm_int type,
                      t_vm_saddr value);
   void addiDefend(t_vm_int id);
   void addiSsuper(t_vm_saddr dst);
   void addiSetfscope(const std::string& nameLabel, t_vm_int len);
   void addiSetfline(t_vm_int line);

   void addiNop(const std::string& label);
   void addiJump(const std::string& label, const std::string& addrLabel);
   void addiCjump(const std::string& label, t_vm_saddr saddr,
                  const std::string& addrLabel);
   void addiFjump(const std::string& label, const std::string& addrLabel,
                  t_vm_saddr saddr);
   void addiFcall(const std::string& label,
                  t_vm_saddr saddr, t_vm_saddr pos, t_vm_int size);
   void addiFret(const std::string& label);
   void addiBclear(const std::string& label, t_vm_saddr saddr,
                   t_vm_saddr size);
   void addiPutnull(const std::string& label, t_vm_saddr saddr, t_vm_int mode);
   void addiPutint(const std::string& label, t_vm_saddr saddr, t_vm_int mode,
                   t_vm_int value);
   void addiPutdouble(const std::string& label, t_vm_saddr saddr, t_vm_int mode,
                      t_vm_double value);
   void addiPutchar(const std::string& label, t_vm_saddr saddr, t_vm_int mode,
                    t_vm_char value);
   void addiPutbool(const std::string& label, t_vm_saddr saddr, t_vm_int mode,
                    t_vm_bool value);
   void addiPutstring(const std::string& label,
                      t_vm_saddr saddr, t_vm_int mode,
                      const std::string& addrLabel, t_vm_int size);
   void addiPutfunction(const std::string& label,
                        t_vm_saddr saddr, t_vm_int mode,
                      const std::string& addrLabel);
   void addiPutsyscall(const std::string& label,
                       t_vm_saddr saddr, t_vm_int mode,
                      t_vm_int syscall);
   void addiPutarr(const std::string& label, t_vm_saddr saddr, t_vm_int mode,
                   t_vm_saddr it, t_vm_int size);
   void addiPutset(const std::string& label, t_vm_saddr saddr, t_vm_int mode,
                   t_vm_saddr it, t_vm_int size);
   void addiPutmap(const std::string& label, t_vm_saddr saddr, t_vm_int mode,
                   t_vm_saddr it, t_vm_int size);
   void addiPutobj(const std::string& label, t_vm_saddr saddr, t_vm_int mode,
                   t_vm_int id);
   void addiPutclass(const std::string& label, t_vm_saddr saddr, t_vm_int mode,
                     t_vm_int id);
   void addiPutvar(const std::string& label, t_vm_saddr saddr, t_vm_int mode,
                   t_vm_saddr src);
   void addiPutref(const std::string& label, t_vm_saddr dst, t_vm_saddr src);
   void addiCopy(const std::string& label, t_vm_saddr dst, t_vm_saddr src);
   void addiMove(const std::string& label, t_vm_saddr dst, t_vm_saddr src);
   void addiSpup(const std::string& label, t_vm_saddr saddr);
   void addiSpdown(const std::string& label, t_vm_saddr saddr);
   void addiSyscall(const std::string& label, t_vm_int value);
   void addiBind(const std::string& label,
                 t_vm_saddr dst, t_vm_saddr it, t_vm_int size);
   void addiLoad(const std::string& label,
                 t_vm_saddr dst, const std::string& srcLabel);
   void addiStore(const std::string& label,
                  t_vm_saddr src, const std::string& dstLabel);
   void addiInit(const std::string& label,
                 t_vm_saddr src, const std::string& dstLabel);
   void addiPostinc(const std::string& label, t_vm_saddr a1, t_vm_saddr a2);
   void addiPostdec(const std::string& label, t_vm_saddr a1, t_vm_saddr a2);
   void addiPreinc(const std::string& label, t_vm_saddr a1, t_vm_saddr a2);
   void addiPredec(const std::string& label, t_vm_saddr a1, t_vm_saddr a2);
   void addiUplus(const std::string& label, t_vm_saddr a1, t_vm_saddr a2);
   void addiUminus(const std::string& label, t_vm_saddr a1, t_vm_saddr a2);
   void addiLnot(const std::string& label, t_vm_saddr a1, t_vm_saddr a2);
   void addiBnot(const std::string& label, t_vm_saddr a1, t_vm_saddr a2);
   void addiMul(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                t_vm_saddr a3);
   void addiDiv(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                t_vm_saddr a3);
   void addiMod(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                t_vm_saddr a3);
   void addiBplus(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                  t_vm_saddr a3);
   void addiBminus(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                   t_vm_saddr a3);
   void addiGt(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
               t_vm_saddr a3);
   void addiLt(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
               t_vm_saddr a3);
   void addiGeq(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                t_vm_saddr a3);
   void addiLeq(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                t_vm_saddr a3);
   void addiEq(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
               t_vm_saddr a3);
   void addiNeq(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                t_vm_saddr a3);
   void addiLand(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                 t_vm_saddr a3);
   void addiLor(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                t_vm_saddr a3);
   void addiLshift(const std::string& label,
                   t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiRshift(const std::string& label,
                   t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBand(const std::string& label,
                 t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBxor(const std::string& label,
                 t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBor(const std::string& label,
                t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiAssign(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                   t_vm_saddr a3);
   void addiPluseq(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                   t_vm_saddr a3);
   void addiMinuseq(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                    t_vm_saddr a3);
   void addiMuleq(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                  t_vm_saddr a3);
   void addiDiveq(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                  t_vm_saddr a3);
   void addiModeq(const std::string& label, t_vm_saddr a1, t_vm_saddr a2,
                  t_vm_saddr a3);
   void addiLshifteq(const std::string& label,
                     t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiRshifteq(const std::string& label,
                     t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBandeq(const std::string& label,
                   t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBxoreq(const std::string& label,
                   t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiBoreq(const std::string& label,
                  t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiSubscript(const std::string& label,
                      t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
   void addiTernary(const std::string& label,
                    t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3,
                     t_vm_saddr a4);
   void addiMember(const std::string& label,
                   t_vm_saddr saddr, const std::string& strLabel,
                   t_vm_int size, t_vm_saddr res);
   void addiDefclass(const std::string& label,
                     const std::string& nameLabel, t_vm_int len,
                     t_vm_int id, t_vm_int parent);
   void addiDeffield(const std::string& label,
                     t_vm_int id, const std::string& nameLabel,
                     t_vm_int len, t_vm_int type,
                     t_vm_saddr value);
   void addiDefsfield(const std::string& label,
                      t_vm_int id, const std::string& nameLabel,
                      t_vm_int len, t_vm_int type,
                      t_vm_saddr value);
   void addiDefend(const std::string& label,
                   t_vm_int id);
   void addiSsuper(const std::string& label,
                   t_vm_saddr dst);
   void addiSetfscope(const std::string& label,
                      const std::string& nameLabel, t_vm_int len);
   void addiSetfline(const std::string& label,
                     t_vm_int line);


   void addia0(t_vm_ins code);
   void addia1(t_vm_ins code, s_bin_builder_arg a1);
   void addia2(t_vm_ins code, s_bin_builder_arg a1, s_bin_builder_arg a2);
   void addia3(t_vm_ins code, s_bin_builder_arg a1, s_bin_builder_arg a2,
               s_bin_builder_arg a3);
   void addia4(t_vm_ins code, s_bin_builder_arg a1, s_bin_builder_arg a2,
               s_bin_builder_arg a3, s_bin_builder_arg a4);
   void addia5(t_vm_ins code, s_bin_builder_arg a1, s_bin_builder_arg a2,
               s_bin_builder_arg a3, s_bin_builder_arg a4,
               s_bin_builder_arg a5);

   void addia0(const std::string& label, t_vm_ins code);
   void addia1(const std::string& label, t_vm_ins code, s_bin_builder_arg a1);
   void addia2(const std::string& label, t_vm_ins code, s_bin_builder_arg a1,
               s_bin_builder_arg a2);
   void addia3(const std::string& label, t_vm_ins code, s_bin_builder_arg a1,
               s_bin_builder_arg a2, s_bin_builder_arg a3);
   void addia4(const std::string& label, t_vm_ins code, s_bin_builder_arg a1,
               s_bin_builder_arg a2, s_bin_builder_arg a3,
               s_bin_builder_arg a4);
   void addia5(const std::string& label, t_vm_ins code, s_bin_builder_arg a1,
               s_bin_builder_arg a2, s_bin_builder_arg a3,
               s_bin_builder_arg a4, s_bin_builder_arg a5);


private:
   s_bin_builder* _builder;
   std::vector<std::string*> _strs;
   std::size_t _unique;
   std::map<std::string, std::string> _sharedStrs;

   const char* getLabel(const std::string& str);

};

# endif //!BIN_BUILDER_HH_
