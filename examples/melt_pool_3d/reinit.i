<<<<<<< HEAD
[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 3
    xmin = 0
    xmax = 0.01
    ymin = 0
    ymax = 0.01
    zmin = 0
    zmax = 0.01
    nx = 50
    ny = 50
    nz = 50
    elem_type = HEX8
  []
=======
[Mesh/gen]
  type = GeneratedMeshGenerator
  dim = 3
  xmin = 0
  xmax = 0.004
  ymin = 0
  ymax = 0.004
  zmin = 0
  zmax = 0.004
  nx = 15
  ny = 15
  nz = 15
  elem_type = HEX8
>>>>>>> 17c6faf (update 3d input file, test on cluster)
[]

[Adaptivity]
  marker = marker
  max_h_level = 3
  #cycles_per_step = 2
  #initial_steps = 1
[]

[Variables]
  [ls]
    order = FIRST
  []
  # [grad_ls]
  #   family = LAGRANGE_VEC
  # []
[]

[AuxVariables]
  [ls_0]
    order = FIRST
  []
[marker]
  family = MONOMIAL
  order = CONSTANT
[]
[]

[Kernels]
  # [grad_ls]
  #   type = VariableGradientRegularization
  #   regularized_var = ls_0
  #   variable = grad_ls
  # []
  [time]
    type = TimeDerivative
    variable = ls
  []
  [reinit]
    type = LevelSetGradientRegularizationReinitialization
    variable = ls
    level_set = ls
    epsilon = 0.0002
  []
[]

[Problem]
  type = LevelSetReinitializationProblem
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  start_time = 0
  num_steps = 10
  nl_abs_tol = 1e-9
  nl_max_its = 10
  nl_forced_its = 3
  line_search = none
  petsc_options_iname = '-ksp_type -ksp_gmres_restart -pc_type -pc_hypre_type'
  petsc_options_value = 'gmres    300              hypre  boomeramg'
  dt = 0.0000001
[]

[Outputs]
  exodus = false
  execute_on = 'TIMESTEP_END'
[]
