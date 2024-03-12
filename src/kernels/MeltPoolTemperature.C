//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MeltPoolTemperature.h"
#include "MooseMesh.h"

registerMooseObject("NavierStokesApp", MeltPoolTemperature);

InputParameters
MeltPoolTemperature::validParams()
{
  InputParameters params = Kernel::validParams();

  params.addClassDescription("This class computes the residual and Jacobian contributions for the "
                             "incompressible Navier-Stokes temperature (energy) equation.");
  // Coupled variables
  params.addRequiredCoupledVar("u", "x-velocity");
  params.addCoupledVar("v", "y-velocity"); // only required in 2D and 3D
  params.addCoupledVar("w", "z-velocity"); // only required in 3D

  params.addRequiredCoupledVar("level_set", "Level set variable.");
  params.addRequiredCoupledVar("temperature", "Temperature variable.");

  return params;
}

MeltPoolTemperature::MeltPoolTemperature(const InputParameters & parameters)
  : Kernel(parameters),

    // Coupled variables
    _u_vel(coupledValue("u")),
    _v_vel(_mesh.dimension() >= 2 ? coupledValue("v") : _zero),
    _w_vel(_mesh.dimension() == 3 ? coupledValue("w") : _zero),

    // Variable numberings
    _u_vel_var_number(coupled("u")),
    _v_vel_var_number(_mesh.dimension() >= 2 ? coupled("v") : libMesh::invalid_uint),
    _w_vel_var_number(_mesh.dimension() == 3 ? coupled("w") : libMesh::invalid_uint),

    _c_id(coupled("level_set")),
    _temp_id(coupled("temperature")),

    // Material Properties
    _rho(getMaterialProperty<Real>("rho")),
    _rho_dc(getMaterialProperty<Real>("rho_dc")),
    _rho_dT(getMaterialProperty<Real>("rho_dT")),
    _k(getMaterialProperty<Real>("k")),
    _k_dc(getMaterialProperty<Real>("k_dc")),
    _k_dT(getMaterialProperty<Real>("k_dT")),
    _cp(getMaterialProperty<Real>("cp")),
    _cp_dc(getMaterialProperty<Real>("cp")),
    _cp_dT(getMaterialProperty<Real>("cp"))
{
}

Real
MeltPoolTemperature::computeQpResidual()
{
  // The convection part, rho * cp u.grad(T) * v.
  // Note: _u is the temperature variable, _grad_u is its gradient.
  Real convective_part = _rho[_qp] * _cp[_qp] *
                         (_u_vel[_qp] * _grad_u[_qp](0) + _v_vel[_qp] * _grad_u[_qp](1) +
                          _w_vel[_qp] * _grad_u[_qp](2)) *
                         _test[_i][_qp];

  // Thermal conduction part, k * grad(T) * grad(v)
  Real conduction_part = _k[_qp] * _grad_u[_qp] * _grad_test[_i][_qp];

  return convective_part + conduction_part;
}

Real
MeltPoolTemperature::computeQpJacobian()
{
  RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);

  Real convective_part = _rho[_qp] * _cp[_qp] * (U * _grad_phi[_j][_qp]) * _test[_i][_qp];
  Real conduction_part = _k[_qp] * (_grad_phi[_j][_qp] * _grad_test[_i][_qp]);

  return convective_part + conduction_part;
}

Real
MeltPoolTemperature::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _u_vel_var_number)
  {
    Real convective_part = _rho[_qp] * _cp[_qp] * _phi[_j][_qp] * _grad_u[_qp](0) * _test[_i][_qp];
    return convective_part;
  }

  else if (jvar == _v_vel_var_number)
  {
    Real convective_part = _rho[_qp] * _cp[_qp] * _phi[_j][_qp] * _grad_u[_qp](1) * _test[_i][_qp];
    return convective_part;
  }

  else if (jvar == _w_vel_var_number)
  {
    Real convective_part = _rho[_qp] * _cp[_qp] * _phi[_j][_qp] * _grad_u[_qp](2) * _test[_i][_qp];
    return convective_part;
  }
  else if (jvar == _c_id)
  {
    Real convective_part = _rho_dc[_qp] * _phi[_j][_qp] * _cp[_qp] *
                               (_u_vel[_qp] * _grad_u[_qp](0) + _v_vel[_qp] * _grad_u[_qp](1) +
                                _w_vel[_qp] * _grad_u[_qp](2)) *
                               _test[_i][_qp] +
                           _rho[_qp] * _cp_dc[_qp] * _phi[_j][_qp] *
                               (_u_vel[_qp] * _grad_u[_qp](0) + _v_vel[_qp] * _grad_u[_qp](1) +
                                _w_vel[_qp] * _grad_u[_qp](2)) *
                               _test[_i][_qp];

    // Thermal conduction part, k * grad(T) * grad(v)
    Real conduction_part = _k_dc[_qp] * _phi[_j][_qp] * _grad_u[_qp] * _grad_test[_i][_qp];

    return convective_part + conduction_part;
  }
  else if (jvar == _temp_id)
  {
    Real convective_part = _rho_dT[_qp] * _phi[_j][_qp] * _cp[_qp] *
                               (_u_vel[_qp] * _grad_u[_qp](0) + _v_vel[_qp] * _grad_u[_qp](1) +
                                _w_vel[_qp] * _grad_u[_qp](2)) *
                               _test[_i][_qp] +
                           _rho[_qp] * _cp_dT[_qp] * _phi[_j][_qp] *
                               (_u_vel[_qp] * _grad_u[_qp](0) + _v_vel[_qp] * _grad_u[_qp](1) +
                                _w_vel[_qp] * _grad_u[_qp](2)) *
                               _test[_i][_qp];

    // Thermal conduction part, k * grad(T) * grad(v)
    Real conduction_part = _k_dT[_qp] * _phi[_j][_qp] * _grad_u[_qp] * _grad_test[_i][_qp];

    return convective_part + conduction_part;
  }
  else
    return 0;
}
