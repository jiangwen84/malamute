/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "nonADLevelSetHeavisideFunction.h"

registerMooseObject("MalamuteApp", nonADLevelSetHeavisideFunction);

InputParameters
nonADLevelSetHeavisideFunction::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription("Computes Heaviside function given by a level set.");
  params.addRequiredCoupledVar("level_set", "Level set variable");
  return params;
}

nonADLevelSetHeavisideFunction::nonADLevelSetHeavisideFunction(const InputParameters & parameters)
  : Material(parameters),
    _c(coupledValue("level_set")),
    _heaviside_function(declareProperty<Real>("heaviside_function")),
    _heaviside_function_dc(declareProperty<Real>("heaviside_function_dc"))
{
}

void
nonADLevelSetHeavisideFunction::computeQpProperties()
{
  _heaviside_function[_qp] = _c[_qp];

  _heaviside_function_dc[_qp] = 1.0;
}
