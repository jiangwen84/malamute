/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "ADMaterial.h"

/**
 * This class computes Heaviside function given by a level set
 */
class LevelSetOlssonHeavisideFunction : public ADMaterial
{
public:
  static InputParameters validParams();

  LevelSetOlssonHeavisideFunction(const InputParameters & parameters);

protected:
  void computeQpProperties() override;

  /// Level set variable
  const ADVariableValue & _c;

  /// Heaviside function
  ADMaterialProperty<Real> & _heaviside_function;
};
