[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 80
  ny = 80
  nz = 40
  xmax = 40
  ymax = 40
  zmax = 20
  zmin = 0
[]

[Variables]
  [temp]
    initial_condition = 300
  []
[]

[Kernels]
  [heat_cond]
    type = ADHeatConduction
    thermal_conductivity = thermal_conductivity
    variable = temp
  []
  [heat_time]
    type = ADHeatConductionTimeDerivative
    specific_heat = specific_heat
    density_name = rho
    variable = temp
  []
[]

[AuxVariables]
  [q]
  []
[]

[AuxKernels]
  [Q]
    type = FunctionAux
    variable = q
    function = source
  []
[]

[Materials]
  [rho]
    type = ADGenericConstantMaterial
    prop_names = 'rho specific_heat thermal_conductivity'
    prop_values = '1000 500 4000'
  []
[]

[Functions]
  [source]
    type = ParsedFunction
    expression = 'if((x - 20) * (x - 20) + (y - 20) * (y - 20) > 25, 0, 2.0*1000000000/(25*3.14159)*exp(-2.0/25*((x - 20) * (x - 20) + (y - 20) * (y - 20))))'
  []
  # [source]
  #   type = ParsedFunction
  #   expression = 'if((x - 20) * (x - 20) + (y - 20) * (y - 20) > 25, 0, 1000000000)'
  # []
[]

[BCs]
  [front]
    type = FunctionNeumannBC
    function = source
    boundary = front
    variable = temp
  []
[]

[Postprocessors]
  [total_temp]
    type = ElementAverageValue
    variable = temp
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  dt = 10000
  nl_abs_tol = 1e-6
  num_steps = 1
  nl_max_its = 12
  l_max_its = 100

  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -sub_ksp_type'
  petsc_options_value = ' asm      lu           2               100                 preonly'

[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
    solve_type = 'NEWTON'
  []
[]

[Outputs]
  exodus = true
  csv = true
[]
