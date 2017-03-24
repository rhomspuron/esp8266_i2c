#include "mathexpression.h"

ExpExpression::ExpExpression(double coeffcients[], int order){
  order = order;
  coeffcients = new double[order+1];
  if (coeffcients==0) Serial.print("Error no memory");
  for(int i=0; i<order+1; i++){
    coeffcients[i] = coeffcients[i];
  }
}

double ExpExpression::calc(double x){
  double result = 0;
  for(int i=0, j=order; j > 0; i++, j--){
    result += coeffcients[i]*pow(x,j);
  }  
  result += coeffcients[order+1];
  return result;
}
ExpExpression::~ExpExpression(){
  delete[] coeffcients;
   
}
