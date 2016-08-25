#ifndef AST_VALUES_HH_
#define AST_VALUES_HH_

# include "ast.hh"
# include <string>
# include "../vm/const.h"

class ASTInt : public AST
{
public:
   ASTInt(Token token);

   t_vm_int getValue() const;

   virtual void accept(ASTVisitor& v) override;
};

class ASTChar : public AST
{
public:
   ASTChar(Token token);

   t_vm_char getValue() const;

   virtual void accept(ASTVisitor& v) override;
};

class ASTString : public AST
{
public:
   ASTString(Token token);

   std::string getValue() const;

   virtual void accept(ASTVisitor& v) override;
};

class ASTTrue : public AST
{
public:
   ASTTrue(Token token);

   virtual void accept(ASTVisitor& v) override;
};

class ASTFalse : public AST
{
public:
   ASTFalse(Token token);

   virtual void accept(ASTVisitor& v) override;
};

class ASTNull : public AST
{
public:
   ASTNull(Token token);

   virtual void accept(ASTVisitor& v) override;
};


class ASTDouble : public AST
{
public:
   ASTDouble(Token token);

   t_vm_double getValue() const;

   virtual void accept(ASTVisitor& v) override;
};

class ASTSymbol : public AST
{
public:
   ASTSymbol(Token token);

   std::string getName() const;

   virtual void accept(ASTVisitor& v) override;
};

class ASTSymbolValue : public AST
{
public:
   ASTSymbolValue(Token token);

   std::string getName() const;

   virtual void accept(ASTVisitor& v) override;
};

class ASTArray: public AST
{

public:
   ASTArray(Token token, const std::vector<AST*>& children);

   virtual void accept(ASTVisitor& v) override;
};

class ASTSet: public AST
{

public:
   ASTSet(Token token, const std::vector<AST*>& children);

   virtual void accept(ASTVisitor& v) override;
};

class ASTMap: public AST
{

public:
   ASTMap(Token token, const std::vector<AST*>& children);

   virtual void accept(ASTVisitor& v) override;
};

class ASTThis : public AST
{
public:
   ASTThis(Token token);

   virtual void accept(ASTVisitor& v) override;
};

#endif //!AST_VALUES_HH_
