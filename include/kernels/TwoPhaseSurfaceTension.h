/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "ADKernelValue.h"

class TwoPhaseSurfaceTension : public ADVectorKernelValue
{
public:
  static InputParameters validParams();  
  TwoPhaseSurfaceTension(const InputParameters & parameters);

protected:
  virtual ADRealVectorValue precomputeQpResidual() override;

  const ADVectorVariableValue & _grad_c;
  const ADVariableValue & _curvature;
  const Real _sigma;
};
