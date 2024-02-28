/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "AuxRayKernel.h"

/**
 * RayKernel that deposits energy from Rays into an aux variable
 */
class LaserDepositionNumberRayKernel : public AuxRayKernel
{
public:
  LaserDepositionNumberRayKernel(const InputParameters & params);

  static InputParameters validParams();

  virtual void onSegment() override;

protected:
  /// The field variable that contains the phase
  const VariableValue & _phase;
  /// The gradient of the field variable that contains the phase
  const VariableGradient & _grad_phase;
};
