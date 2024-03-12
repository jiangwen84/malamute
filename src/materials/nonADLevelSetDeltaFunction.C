/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "nonADLevelSetDeltaFunction.h"

registerMooseObject("MalamuteApp", nonADLevelSetDeltaFunction);

InputParameters
nonADLevelSetDeltaFunction::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription("Computes delta function given by a level set.");
  params.addRequiredCoupledVar("level_set", "Regularized gradient of the level set variable");
  return params;
}

nonADLevelSetDeltaFunction::nonADLevelSetDeltaFunction(const InputParameters & parameters)
  : Material(parameters),
    _grad_c(coupledGradient("level_set")),
    _delta_function(declareProperty<Real>("delta_function")),
    _delta_function_dc(declareProperty<RealVectorValue>("delta_function_dc"))
{
}

void
nonADLevelSetDeltaFunction::computeQpProperties()
{
  _delta_function[_qp] =
      (_grad_c[_qp] + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm();

  _delta_function_dc[_qp] =
      _grad_c[_qp] /
      (_grad_c[_qp] + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm();
}
