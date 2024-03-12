//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MeltPoolMomentumTimeDerivative.h"

registerMooseObject("MalamuteApp", MeltPoolMomentumTimeDerivative);

InputParameters
MeltPoolMomentumTimeDerivative::validParams()
{
  InputParameters params = TimeDerivative::validParams();
  params.addClassDescription("This class computes the time derivative for the incompressible "
                             "Navier-Stokes momentum equation.");
  params.addRequiredCoupledVar("level_set", "Level set variable.");
  params.addRequiredCoupledVar("temperature", "Temperature variable.");
  return params;
}

MeltPoolMomentumTimeDerivative::MeltPoolMomentumTimeDerivative(const InputParameters & parameters)
  : TimeDerivative(parameters),
    _rho(getMaterialProperty<Real>("rho")),
    _rho_dc(getMaterialProperty<Real>("rho_dc")),
    _rho_dT(getMaterialProperty<Real>("rho_dT")),
    _c_id(coupled("level_set")),
    _temp_id(coupled("temperature"))
{
}

Real
MeltPoolMomentumTimeDerivative::computeQpResidual()
{
  return _rho[_qp] * TimeDerivative::computeQpResidual();
}

Real
MeltPoolMomentumTimeDerivative::computeQpJacobian()
{
  return _rho[_qp] * TimeDerivative::computeQpJacobian();
}

Real
MeltPoolMomentumTimeDerivative::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _c_id)
  {
    return _rho_dc[_qp] * _phi[_j][_qp] * TimeDerivative::computeQpResidual();
  }
  else if (jvar == _temp_id)
  {
    return _rho_dT[_qp] * _phi[_j][_qp] * TimeDerivative::computeQpResidual();
  }

  return 0.;
}
