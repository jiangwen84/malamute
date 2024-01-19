/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "ADMaterial.h"

/**
 * This class computes Heaviside function given by a level set
 */
class LevelSetHeavisideFunction : public ADMaterial
{
public:
  static InputParameters validParams();

  LevelSetHeavisideFunction(const InputParameters & parameters);

protected:
  void initQpStatefulProperties() override;

  void computeQpProperties() override;

  /// Level set variable
  const ADVariableValue & _c;

  const Real _epsilon;

  /// Heaviside function
  ADMaterialProperty<Real> & _heaviside_function;
};
