//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LevelSetAdvectionEvp.h"

registerMooseObject("MalamuteApp", LevelSetAdvectionEvp);

InputParameters
LevelSetAdvectionEvp::validParams()
{
  InputParameters params = ADKernelValue::validParams();
  params.addClassDescription("Implements the level set advection equation: $\\vec{v}\\cdot\\nabla "
                             "u = 0$, where the weak form is $(\\psi_i, \\vec{v}\\cdot\\nabla u) = "
                             "0$.");
  params.addRequiredParam<Real>("rho_l", "Liquid density.");
  params.addRequiredParam<Real>("rho_g", "Gas density.");
  params.addRequiredCoupledVar("velocity", "Velocity vector variable.");
  return params;
}

LevelSetAdvectionEvp::LevelSetAdvectionEvp(const InputParameters & parameters)
  : ADKernelValue(parameters),
    _velocity(adCoupledVectorValue("velocity")),
    _melt_pool_mass_rate(getADMaterialProperty<Real>("melt_pool_mass_rate")),
    _rho_l(getParam<Real>("rho_l")),
    _rho_g(getParam<Real>("rho_g"))
{
}

ADReal
LevelSetAdvectionEvp::precomputeQpResidual()
{
  ADReal s = (_grad_u[_qp] + RealVectorValue(libMesh::TOLERANCE)).norm();

  return _velocity[_qp] * _grad_u[_qp] +
         _melt_pool_mass_rate[_qp] * s * (_u[_qp] / _rho_l + (1.0 - _u[_qp]) / _rho_g);
}
