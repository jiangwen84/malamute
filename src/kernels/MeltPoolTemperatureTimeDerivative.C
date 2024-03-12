//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MeltPoolTemperatureTimeDerivative.h"

registerMooseObject("NavierStokesApp", MeltPoolTemperatureTimeDerivative);

InputParameters
MeltPoolTemperatureTimeDerivative::validParams()
{
  InputParameters params = TimeDerivative::validParams();
  params.addClassDescription("This class computes the time derivative for the incompressible "
                             "Navier-Stokes momentum equation.");
  params.addRequiredCoupledVar("level_set", "Level set variable.");
  params.addRequiredCoupledVar("temperature", "Temperature variable.");
  return params;
}

MeltPoolTemperatureTimeDerivative::MeltPoolTemperatureTimeDerivative(
    const InputParameters & parameters)
  : TimeDerivative(parameters),
    _c_id(coupled("level_set")),
    _temp_id(coupled("temperature")),

    // Material Properties
    _rho(getMaterialProperty<Real>("rho")),
    _rho_dc(getMaterialProperty<Real>("rho_dc")),
    _rho_dT(getMaterialProperty<Real>("rho_dT")),
    _cp(getMaterialProperty<Real>("cp")),
    _cp_dc(getMaterialProperty<Real>("cp")),
    _cp_dT(getMaterialProperty<Real>("cp"))
{
}

Real
MeltPoolTemperatureTimeDerivative::computeQpResidual()
{
  return _rho[_qp] * _cp[_qp] * TimeDerivative::computeQpResidual();
}

Real
MeltPoolTemperatureTimeDerivative::computeQpJacobian()
{
  return _rho[_qp] * _cp[_qp] * TimeDerivative::computeQpJacobian();
}

Real
MeltPoolTemperatureTimeDerivative::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _c_id)
  {
    return _rho_dc[_qp] * _phi[_j][_qp] * _cp[_qp] * TimeDerivative::computeQpResidual() +
           _rho[_qp] * _phi[_j][_qp] * _cp_dc[_qp] * TimeDerivative::computeQpResidual();
  }
  else if (jvar == _temp_id)
  {
    return _rho_dT[_qp] * _phi[_j][_qp] * _cp[_qp] * TimeDerivative::computeQpResidual() +
           _rho[_qp] * _phi[_j][_qp] * _cp_dT[_qp] * TimeDerivative::computeQpResidual();
  }
  else
    return 0.;
}
