/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "LevelSetHeavisideFunction.h"

registerMooseObject("MalamuteApp", LevelSetHeavisideFunction);

InputParameters
LevelSetHeavisideFunction::validParams()
{
  InputParameters params = ADMaterial::validParams();
  params.addClassDescription("Computes Heaviside function given by a level set.");
  params.addRequiredCoupledVar("level_set", "Level set variable");
  params.addRequiredParam<Real>("epsilon", "epsilon is half of transition thinkness.");
  return params;
}

LevelSetHeavisideFunction::LevelSetHeavisideFunction(const InputParameters & parameters)
  : ADMaterial(parameters),
    _c(adCoupledValue("level_set")),
    _epsilon(getParam<Real>("epsilon")),
    _heaviside_function(declareADProperty<Real>("heaviside_function"))
{
}

void
LevelSetHeavisideFunction::initQpStatefulProperties()
{
  _heaviside_function[_qp] = 0.0;
};

void
LevelSetHeavisideFunction::computeQpProperties()
{
  Real c = MetaPhysicL::raw_value(_c[_qp]);
  if (c < -_epsilon)
    _heaviside_function[_qp] = 0.0;
  else if (c <= _epsilon)
    _heaviside_function[_qp] = 0.5 * (1 + _c[_qp] / _epsilon +
                                      1 / libMesh::pi * std::sin(libMesh::pi * _c[_qp] / _epsilon));
  else
    _heaviside_function[_qp] = 1.0;
}
