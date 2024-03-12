//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.MeltPoolMass

#pragma once

#include "INSBase.h"

// Forward Declarations

/**
 * This class computes the mass equation residual and Jacobian
 * contributions for the incompressible Navier-Stokes momentum
 * equation.
 */
class MeltPoolMass : public INSBase
{
public:
  static InputParameters validParams();

  MeltPoolMass(const InputParameters & parameters);

  virtual ~MeltPoolMass() {}

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned jvar);

  virtual Real computeQpPGResidual();
  virtual Real computeQpPGJacobian();
  virtual Real computeQpPGOffDiagJacobian(unsigned comp);

  virtual RealVectorValue strongMeltPoolTerm();
  virtual Real dstrongMeltPoolTerm(unsigned comp);

  bool _pspg;
  const Function & _x_ffn;
  const Function & _y_ffn;
  const Function & _z_ffn;

  const unsigned int _c_id;
  const unsigned int _temp_id;
  const unsigned int _curvature_id;

  /// Gradient of the level set variable
  const VariableGradient & _grad_c;

  /// Temperature variable
  const VariableValue & _temp;

  /// Gradient of temperature variable
  const VariableGradient & _grad_temp;

  /// Curvature variable
  const VariableValue & _curvature;

  /// Permeability in Darcy term
  const MaterialProperty<Real> & _permeability;
  const MaterialProperty<Real> & _permeability_dT;

  /// Surface tension coefficient
  const Real & _sigma;

  /// Thermal-capillary coefficient
  const Real & _sigmaT;

  /// Level set delta function
  const MaterialProperty<Real> & _delta_function;
  const MaterialProperty<RealVectorValue> & _delta_function_dc;

  /// Level set Heaviside function
  const MaterialProperty<Real> & _heaviside_function;
  const MaterialProperty<Real> & _heaviside_function_dc;

  /// Saturated vapor pressure
  const MaterialProperty<Real> & _saturated_vapor_pressure;
  const MaterialProperty<Real> & _saturated_vapor_pressure_dT;

  /// Liquid mass fraction
  const MaterialProperty<Real> & _f_l;
};
