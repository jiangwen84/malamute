#pragma once
#include "ElementVectorPostprocessor.h"

class EnergyDepositionPostprocessor : public ElementVectorPostprocessor
{
public:
  static InputParameters validParams();
  EnergyDepositionPostprocessor(const InputParameters & parameters);
  virtual void initialize() override;
  virtual void execute() override;
  virtual void finalize() override;
  virtual void threadJoin(const UserObject & y) override;

private:
  const VariableValue & _deposition_number;
  const VariableValue & _deposition;
  VectorPostprocessorValue & _x;
  VectorPostprocessorValue & _y;
  VectorPostprocessorValue & _z;
  VectorPostprocessorValue & _energy;
};