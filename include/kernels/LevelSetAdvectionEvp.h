//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADKernelValue.h"

/**
 * Advection Kernel for the levelset equation.
 *
 * \psi_i \vec{v} \nabla u,
 * where \vec{v} is the interface velocity that is a set of
 * coupled variables.
 */
class LevelSetAdvectionEvp : public ADKernelValue
{
public:
  static InputParameters validParams();

  LevelSetAdvectionEvp(const InputParameters & parameters);

protected:
  virtual ADReal precomputeQpResidual() override;

  /// Velocity vector variable
  const ADVectorVariableValue & _velocity;

  /// Mass transfer rate
  const ADMaterialProperty<Real> & _melt_pool_mass_rate;

  /// Liquid density
  const Real _rho_l;

  const Real _rho_g;
};
