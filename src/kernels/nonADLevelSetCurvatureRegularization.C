/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "nonADLevelSetCurvatureRegularization.h"

registerMooseObject("MalamuteApp", nonADLevelSetCurvatureRegularization);

InputParameters
nonADLevelSetCurvatureRegularization::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription("Computes regularized interface curvature that is represented by a "
                             "level set function (Elin Olsson et al, JCP 225 (2007) 785-807).");
  params.addRequiredCoupledVar("level_set", "Variable of level set.");
  params.addRequiredParam<Real>("varepsilon", "Regulizatione parameter.");
  return params;
}

nonADLevelSetCurvatureRegularization::nonADLevelSetCurvatureRegularization(
    const InputParameters & parameters)
  : Kernel(parameters),
    _grad_c(coupledGradient("level_set")),
    _c_var(coupled("level_set")),
    _varepsilon(getParam<Real>("varepsilon"))
{
}

Real
nonADLevelSetCurvatureRegularization::computeQpResidual()
{
  const auto grad_c_norm = MooseUtils::isZero(_grad_c[_qp]) ? Real(1e-42) : _grad_c[_qp].norm();
  if (grad_c_norm > libMesh::TOLERANCE)
    return _test[_i][_qp] * _u[_qp] -
           _grad_test[_i][_qp] * ((_grad_c[_qp]) / grad_c_norm - _varepsilon * _grad_u[_qp]);
  else
    return _test[_i][_qp] * _u[_qp] + _grad_test[_i][_qp] * _varepsilon * _grad_u[_qp];
}

Real
nonADLevelSetCurvatureRegularization::computeQpJacobian()
{
  const auto grad_c_norm = MooseUtils::isZero(_grad_c[_qp]) ? Real(1e-42) : _grad_c[_qp].norm();
  if (grad_c_norm > libMesh::TOLERANCE)
    return _test[_i][_qp] * _phi[_j][_qp] -
           _grad_test[_i][_qp] * (-_varepsilon * _grad_phi[_j][_qp]);
  else
    return _test[_i][_qp] * _phi[_j][_qp] + _grad_test[_i][_qp] * _varepsilon * _grad_phi[_j][_qp];
}

Real
nonADLevelSetCurvatureRegularization::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _c_var)
  {
    const auto grad_c_norm = MooseUtils::isZero(_grad_c[_qp]) ? Real(1e-42) : _grad_c[_qp].norm();
    if (grad_c_norm > libMesh::TOLERANCE)
      return -_grad_test[_i][_qp] * ((_grad_phi[_j][_qp]) / grad_c_norm);
  }

  return 0.0;
}
