/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "TwoPhaseSurfaceTension.h"

registerMooseObject("MalamuteApp", TwoPhaseSurfaceTension);

InputParameters
TwoPhaseSurfaceTension::validParams()
{
  InputParameters params = ADKernelValue::validParams();
  params.addRequiredCoupledVar("level_set_gradient", "Regularized gradient of Level set variable");
  params.addRequiredCoupledVar("curvature", "Regularized curvature variable");
  params.addRequiredParam<Real>("surface_tension", "Surface tension coefficient.");
  return params;
}

TwoPhaseSurfaceTension::TwoPhaseSurfaceTension(const InputParameters & parameters)
  : ADVectorKernelValue(parameters),
    _grad_c(adCoupledVectorValue("level_set_gradient")),
    _curvature(adCoupledValue("curvature")),
    _sigma(getParam<Real>("surface_tension"))
{
}

ADRealVectorValue
TwoPhaseSurfaceTension::precomputeQpResidual()
{
  return -_sigma * _curvature[_qp] * _grad_c[_qp];
}
