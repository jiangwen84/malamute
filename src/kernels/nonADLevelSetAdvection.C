//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "nonADLevelSetAdvection.h"

registerMooseObject("MalamuteApp", nonADLevelSetAdvection);

InputParameters
nonADLevelSetAdvection::validParams()
{
  InputParameters params = KernelValue::validParams();
  params.addClassDescription("Implements the level set advection equation: $\\vec{v}\\cdot\\nabla "
                             "u = 0$, where the weak form is $(\\psi_i, \\vec{v}\\cdot\\nabla u) = "
                             "0$.");
  params.addRequiredCoupledVar("velocity_x", "Velocity x variable.");
  params.addCoupledVar("velocity_y", 0.0, "Velocity y variable.");
  params.addCoupledVar("velocity_z", 0.0, "Velocity z variable.");
  return params;
}

nonADLevelSetAdvection::nonADLevelSetAdvection(const InputParameters & parameters)
  : KernelValue(parameters),
    _velocity_x(coupledValue("velocity_x")),
    _velocity_y(coupledValue("velocity_y")),
    _velocity_z(coupledValue("velocity_z")),
    _velocity_x_id(coupled("velocity_x")),
    _velocity_y_id(coupled("velocity_y")),
    _velocity_z_id(coupled("velocity_z"))
{
}

Real
nonADLevelSetAdvection::precomputeQpResidual()
{
  RealVectorValue velocity(_velocity_x[_qp], _velocity_y[_qp], _velocity_z[_qp]);
  return velocity * _grad_u[_qp];
}

Real
nonADLevelSetAdvection::precomputeQpJacobian()
{
  RealVectorValue velocity(_velocity_x[_qp], _velocity_y[_qp], _velocity_z[_qp]);
  return velocity * _grad_phi[_j][_qp];
}

Real
nonADLevelSetAdvection::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _velocity_x_id)
    return _phi[_j][_qp] * _grad_u[_qp](0);
  else if (jvar == _velocity_y_id)
    return _phi[_j][_qp] * _grad_u[_qp](1);
  else if (jvar == _velocity_z_id)
    return _phi[_j][_qp] * _grad_u[_qp](2);

  return 0.0;
}
