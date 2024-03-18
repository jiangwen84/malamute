//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

// MOOSE includes
#include "LevelSetOlssonOneStepReinitialization.h"

registerMooseObject("MalamuteApp", LevelSetOlssonOneStepReinitialization);

InputParameters
LevelSetOlssonOneStepReinitialization::validParams()
{
  InputParameters params = ADKernelGrad::validParams();
  params.addClassDescription("The re-initialization equation defined by Olsson et. al. (2007).");
  params.addRequiredParam<Real>("reinit_speed", "Reinitialization Speed.");
  params.addRequiredParam<PostprocessorName>(
      "epsilon", "The epsilon coefficient to be used in the reinitialization calculation.");
  return params;
}

LevelSetOlssonOneStepReinitialization::LevelSetOlssonOneStepReinitialization(
    const InputParameters & parameters)
  : ADKernelGrad(parameters),
    _reinit_speed(getParam<Real>("reinit_speed")),
    _epsilon(getPostprocessorValue("epsilon"))
{
}

ADRealVectorValue
LevelSetOlssonOneStepReinitialization::precomputeQpResidual()
{
  ADReal s = (_grad_u[_qp] + RealVectorValue(libMesh::TOLERANCE)).norm() + Real(libMesh::TOLERANCE);

  ADRealVectorValue n = _grad_u[_qp] / s;
  ADRealVectorValue f = _u[_qp] * (1 - _u[_qp]) * n;
  return (-f + _epsilon * ((_grad_u[_qp] * n) * n)) * _reinit_speed;
}
