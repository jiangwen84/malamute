/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "Kernel.h"

/**
 * This class computes regularized interface curvature that is represented by a level set
 * function.
 */
class nonADLevelSetCurvatureRegularization : public Kernel
{
public:
  static InputParameters validParams();

  nonADLevelSetCurvatureRegularization(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  /// Gradient of regularized gradient of level set
  const VariableGradient & _grad_c;

  const unsigned int _c_var;

  /// regulization parameter
  const Real _varepsilon;
};
