/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "LevelSetGradientRegularizationReinitialization.h"

registerMooseObject("MalamuteApp", LevelSetGradientRegularizationReinitialization);

InputParameters
LevelSetGradientRegularizationReinitialization::validParams()
{
  InputParameters params = KernelGrad::validParams();
  params.addClassDescription("The re-initialization equation that uses regularized gradient.");
  params.addRequiredCoupledVar("level_set_gradient",
                               "Regularized gradient of the level set variable");
  params.addRequiredParam<Real>(
      "epsilon", "The epsilon coefficient to be used in the reinitialization calculation.");
  return params;
}

LevelSetGradientRegularizationReinitialization::LevelSetGradientRegularizationReinitialization(
    const InputParameters & parameters)
  : KernelGrad(parameters),
    _grad_c(coupledVectorValue("level_set_gradient")),
    _epsilon(getParam<Real>("epsilon"))
{
}

RealGradient
LevelSetGradientRegularizationReinitialization::precomputeQpResidual()
{
  RealGradient n_hat = _grad_c[_qp];
  RealGradient f = _u[_qp] * (1 - _u[_qp]) * n_hat;
  return (-f + _epsilon * _grad_u[_qp]);
  // return (-f + _epsilon * (_grad_u[_qp] * n_hat) * n_hat);
}

RealGradient
LevelSetGradientRegularizationReinitialization::precomputeQpJacobian()
{
  RealGradient n_hat = _grad_c[_qp];
  RealGradient df = _phi[_j][_qp] * (1 - _u[_qp]) * n_hat + _u[_qp] * (-_phi[_j][_qp]) * n_hat;
  // return (-df + _epsilon * (_grad_phi[_j][_qp] * n_hat) * n_hat);
  return (-df + _epsilon * _grad_phi[_j][_qp]);
}
