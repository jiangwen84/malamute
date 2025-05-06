#include "EnergyDepositionPostprocessor.h"

registerMooseObject("MalamuteApp", EnergyDepositionPostprocessor);

InputParameters
EnergyDepositionPostprocessor::validParams()
{
  InputParameters params = ElementVectorPostprocessor::validParams();
  params.addRequiredCoupledVar("deposition_number", "The laser deposition number variable");
  params.addRequiredCoupledVar("deposition", "The laser deposition variable");
  return params;
}

EnergyDepositionPostprocessor::EnergyDepositionPostprocessor(const InputParameters & parameters)
  : ElementVectorPostprocessor(parameters),
    _deposition_number(coupledValue("deposition_number")),
    _deposition(coupledValue("deposition")),
    _x(declareVector("x")),
    _y(declareVector("y")),
    _z(declareVector("z")),
    _energy(declareVector("energy"))
{
}

void
EnergyDepositionPostprocessor::initialize()
{
  _x.clear();
  _y.clear();
  _z.clear();
  _energy.clear();
}

void
EnergyDepositionPostprocessor::execute()
{
  if (_deposition_number[0] >= 1)
  {

    Point centroid = _current_elem->centroid();

    _x.push_back(centroid(0));
    _y.push_back(centroid(1));
    _z.push_back(centroid(2));
    _energy.push_back(_deposition[0]);
  }
}

void
EnergyDepositionPostprocessor::finalize()
{
  comm().gather(0, _x);
  comm().gather(0, _y);
  comm().gather(0, _z);
  comm().gather(0, _energy);
}

void
EnergyDepositionPostprocessor::threadJoin(const UserObject & y)
{
  const auto & vpp = static_cast<const EnergyDepositionPostprocessor &>(y);
  _x.insert(_x.end(), vpp._x.begin(), vpp._x.end());
  _y.insert(_y.end(), vpp._y.begin(), vpp._y.end());
  _z.insert(_z.end(), vpp._z.begin(), vpp._z.end());
  _energy.insert(_energy.end(), vpp._energy.begin(), vpp._energy.end());
}