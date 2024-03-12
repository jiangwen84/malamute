//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "nonADLevelSetAdvectionSUPG.h"

registerMooseObject("MalamuteApp", nonADLevelSetAdvectionSUPG);

InputParameters
nonADLevelSetAdvectionSUPG::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription(
      "SUPG stablization term for the advection portion of the level set equation.");
  params.addRequiredCoupledVar("velocity_x", "Velocity x variable.");
  params.addCoupledVar("velocity_y", 0.0, "Velocity y variable.");
  params.addCoupledVar("velocity_z", 0.0, "Velocity z variable.");
  return params;
}

nonADLevelSetAdvectionSUPG::nonADLevelSetAdvectionSUPG(const InputParameters & parameters)
  : Kernel(parameters),
    _velocity_x(coupledValue("velocity_x")),
    _velocity_y(coupledValue("velocity_y")),
    _velocity_z(coupledValue("velocity_z")),
    _velocity_x_id(coupled("velocity_x")),
    _velocity_y_id(coupled("velocity_y")),
    _velocity_z_id(coupled("velocity_z"))
{
}

Real
nonADLevelSetAdvectionSUPG::computeQpResidual()
{
  RealVectorValue velocity(_velocity_x[_qp], _velocity_y[_qp], _velocity_z[_qp]);
  Real tau = _current_elem->hmin() /
             (2 * (velocity + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm());
  return (tau * velocity) * (velocity * _grad_u[_qp]) * _grad_test[_i][_qp];
}

Real
nonADLevelSetAdvectionSUPG::computeQpJacobian()
{
  RealVectorValue velocity(_velocity_x[_qp], _velocity_y[_qp], _velocity_z[_qp]);
  Real tau = _current_elem->hmin() /
             (2 * (velocity + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm());

  return (tau * velocity) * (velocity * _grad_phi[_j][_qp]) * _grad_test[_i][_qp];
}

Real
nonADLevelSetAdvectionSUPG::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _velocity_x_id || jvar == _velocity_y_id || jvar == _velocity_z_id)
  {
    RealVectorValue velocity(_velocity_x[_qp], _velocity_y[_qp], _velocity_z[_qp]);
    Real tau = _current_elem->hmin() /
               (2 * (velocity + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm());

    RealVectorValue dtau =
        -_current_elem->hmin() /
        (2 * (velocity + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm_sq()) *
        velocity / (velocity + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm();

    if (jvar == _velocity_x_id)
      return (tau * velocity) * (_phi[_j][_qp] * _grad_u[_qp](0)) * _grad_test[_i][_qp] +
             (tau * _phi[_j][_qp]) * (velocity * _grad_u[_qp]) * _grad_test[_i][_qp](0) +
             (dtau(0) * _phi[_j][_qp] * velocity) * (velocity * _grad_u[_qp]) * _grad_test[_i][_qp];
    else if (jvar == _velocity_y_id)
      return (tau * velocity) * (_phi[_j][_qp] * _grad_u[_qp](1)) * _grad_test[_i][_qp] +
             (tau * _phi[_j][_qp]) * (velocity * _grad_u[_qp]) * _grad_test[_i][_qp](1) +
             (dtau(1) * _phi[_j][_qp] * velocity) * (velocity * _grad_u[_qp]) * _grad_test[_i][_qp];
    else if (jvar == _velocity_z_id)
      return (tau * velocity) * (_phi[_j][_qp] * _grad_u[_qp](2)) * _grad_test[_i][_qp] +
             (tau * _phi[_j][_qp]) * (velocity * _grad_u[_qp]) * _grad_test[_i][_qp](2) +
             (dtau(2) * _phi[_j][_qp] * velocity) * (velocity * _grad_u[_qp]) * _grad_test[_i][_qp];
  }
  return 0.0;
}
