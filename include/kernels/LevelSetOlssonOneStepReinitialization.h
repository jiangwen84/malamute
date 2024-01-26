//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

// MOOSE includes
#include "ADKernelGrad.h"

/**
 * Implements the re-initialization equation proposed by Olsson et. al. (2007).
 */
class LevelSetOlssonOneStepReinitialization : public ADKernelGrad
{
public:
  static InputParameters validParams();

  LevelSetOlssonOneStepReinitialization(const InputParameters & parameters);

protected:
  virtual ADRealVectorValue precomputeQpResidual() override;

  const Real & _reinit_speed;

  /// Interface thickness
  const PostprocessorValue & _epsilon;

  using ADKernelGrad::getPostprocessorValue;
};
