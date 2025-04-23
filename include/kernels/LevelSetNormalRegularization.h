/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "VectorKernel.h"

/**
 * This class performs L2 projection of a variable's gradient onto a new vector variable.
 */
class LevelSetNormalRegularization : public VectorKernel
{
public:
  static InputParameters validParams();

  LevelSetNormalRegularization(const InputParameters & parameters);

protected:
  Real computeQpResidual() override;
  Real computeQpJacobian() override;

  /// Gradient of the variable that needs regulization
  const VariableGradient & _grad_c;
};
