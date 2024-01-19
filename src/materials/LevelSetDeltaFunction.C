/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "LevelSetDeltaFunction.h"

registerMooseObject("MalamuteApp", LevelSetDeltaFunction);

InputParameters
LevelSetDeltaFunction::validParams()
{
  InputParameters params = ADMaterial::validParams();
  params.addClassDescription("Computes delta function given by a level set.");
  params.addRequiredCoupledVar("level_set", "Level set variable");
  params.addRequiredParam<Real>("epsilon", "epsilon is half of transition thinkness.");
  return params;
}

LevelSetDeltaFunction::LevelSetDeltaFunction(const InputParameters & parameters)
  : ADMaterial(parameters),
    _c(adCoupledValue("level_set")),
    _epsilon(getParam<Real>("epsilon")),
    _delta_function(declareADProperty<Real>("delta_function"))
{
}

void
LevelSetDeltaFunction::initQpStatefulProperties()
{
  _delta_function[_qp] = 0.0;
};

void
LevelSetDeltaFunction::computeQpProperties()
{
  Real c = MetaPhysicL::raw_value(_c[_qp]);
  if (c <= _epsilon && c >= -_epsilon)
    _delta_function[_qp] =
        0.5 / _epsilon + 0.5 / _epsilon * std::cos(libMesh::pi * _c[_qp] / _epsilon);
  else
    _delta_function[_qp] = 0.0;
}
