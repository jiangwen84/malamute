/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "LevelSetNormalRegularization.h"

registerMooseObject("MalamuteApp", LevelSetNormalRegularization);

InputParameters
LevelSetNormalRegularization::validParams()
{
  InputParameters params = VectorKernel::validParams();
  params.addClassDescription(
      "Performs L2 projection of a variable's gradient onto a new vector variable.");
  params.addRequiredCoupledVar("regularized_var", "The variable to be regularized.");
  return params;
}

LevelSetNormalRegularization::LevelSetNormalRegularization(const InputParameters & parameters)
  : VectorKernel(parameters), _grad_c(coupledGradient("regularized_var"))
{
}

Real
LevelSetNormalRegularization::computeQpResidual()
{
  Real s = _grad_c[_qp].norm() + libMesh::TOLERANCE;
  if (MetaPhysicL::raw_value(_grad_c[_qp].norm()) > 1.0e-4)
    return _test[_i][_qp] * (_u[_qp] - _grad_c[_qp] / s);
  else
    return _test[_i][_qp] * _u[_qp];
}

Real
LevelSetNormalRegularization::computeQpJacobian()
{
  return _test[_i][_qp] * (_phi[_j][_qp]);
}
