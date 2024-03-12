//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "KernelValue.h"

/**
 * Advection Kernel for the levelset equation.
 *
 * \psi_i \vec{v} \nabla u,
 * where \vec{v} is the interface velocity that is a set of
 * coupled variables.
 */
class nonADLevelSetAdvection : public KernelValue
{
public:
  static InputParameters validParams();

  nonADLevelSetAdvection(const InputParameters & parameters);

protected:
  virtual Real precomputeQpResidual() override;

  virtual Real precomputeQpJacobian() override;

  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  /// Velocity vector variable
  const VariableValue & _velocity_x;
  const VariableValue & _velocity_y;
  const VariableValue & _velocity_z;

  const unsigned int _velocity_x_id;
  const unsigned int _velocity_y_id;
  const unsigned int _velocity_z_id;
};
