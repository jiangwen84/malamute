[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 0.001
    ymin = 0.0005
    ymax = 0.0015
    nx = 100
    ny = 100
    elem_type = QUAD4
  []
  #uniform_refine = 2
[]

# [AuxVariables]
#   [marker]
#     family = MONOMIAL
#     order = CONSTANT
#   []
# []

# [Adaptivity]
#   marker = marker
#   max_h_level = 2
#   cycles_per_step = 2
# []

[Adaptivity]
  steps = 2
  marker = box
  max_h_level = 2
  initial_steps = 2
  stop_time = 1.0e-10
  [Markers]
    [box]
      bottom_left = '0.000 0.0007 0'
      inside = refine
      top_right = '0.0009 0.00125 0'
      outside = do_nothing
      type = BoxMarker
    []
  []
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
    # level_set_gradient = grad_ls
    level_set = ls_0
    epsilon = 0.00002
  []
[]

[Problem]
  type = LevelSetReinitializationProblem
[]

# [UserObjects]
#   [arnold]
#     type = LevelSetOlssonTerminator
#     tol = 0.5
#     min_steps = 5
#   []
# []

[Executioner]
  type = Transient
  solve_type = NEWTON
  start_time = 0
  num_steps = 20
  nl_abs_tol = 1e-14
  nl_max_its = 10
  line_search = none
  # petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package -ksp_type'
  # petsc_options_value = 'lu NONZERO superlu_dist preonly'
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -sub_ksp_type'
  petsc_options_value = ' asm      lu           1               31                 preonly'
  dt = 1e-4
[]

[Outputs]
  exodus = false
  execute_on = 'TIMESTEP_END'
[]
