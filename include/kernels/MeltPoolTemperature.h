//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Kernel.h"

// Forward Declarations

/**
 * This class computes the residual and Jacobian contributions for the
 * incompressible Navier-Stokes temperature (energy) equation.
 */
class MeltPoolTemperature : public Kernel
{
public:
  static InputParameters validParams();

  MeltPoolTemperature(const InputParameters & parameters);

  virtual ~MeltPoolTemperature() {}

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned jvar);

  // Coupled variables
  const VariableValue & _u_vel;
  const VariableValue & _v_vel;
  const VariableValue & _w_vel;

  // Variable numberings
  unsigned _u_vel_var_number;
  unsigned _v_vel_var_number;
  unsigned _w_vel_var_number;
  unsigned _c_id;
  unsigned _temp_id;

  // Required parameters
  const MaterialProperty<Real> & _rho;
  const MaterialProperty<Real> & _rho_dc;
  const MaterialProperty<Real> & _rho_dT;
  const MaterialProperty<Real> & _k;
  const MaterialProperty<Real> & _k_dc;
  const MaterialProperty<Real> & _k_dT;
  const MaterialProperty<Real> & _cp;
  const MaterialProperty<Real> & _cp_dc;
  const MaterialProperty<Real> & _cp_dT;
};
