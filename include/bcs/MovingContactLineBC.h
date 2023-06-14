/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "ADVectorIntegratedBC.h"
#include "MooseEnum.h"

// Forward Declarations

/**
 * This class implements moving contact lines BC.
 */

class MovingContactLineBC : public ADVectorIntegratedBC
{
public:
  static InputParameters validParams();

  MovingContactLineBC(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  const ADMaterialProperty<Real> & _mu;

  /// Gradient of the level set variable
  const ADVectorVariableValue & _grad_c;

  const Real & _beta_n;
  const Real & _beta_cl;

  const Real & _penalty;

  /// Surface tension coefficient
  const Real & _sigma;

  const Real & _theta;
};
