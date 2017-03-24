#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "com.h"
#include <math.h>

class Expression{
  public:
    virtual double calc(double x){Serial.print("base expression");return 0;};
};

class ExpExpression: public Expression{
  public:
    ExpExpression(double coeffcients[], int order);
    ~ExpExpression();
    double calc(double x);
  private:
    double* coeffcients;
    int order;
    
};

#endif
