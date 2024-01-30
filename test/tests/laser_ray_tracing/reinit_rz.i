[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 2e-3
    ymin = 0
    ymax = 4e-3
    nx = 161
    ny = 321
    elem_type = QUAD4
  []
  coord_type = RZ
  #uniform_refine = 1
[]

# [Adaptivity]
#   steps = 3
#   marker = box
#   max_h_level = 3
#   initial_steps = 3
#   stop_time = 1.0e-10
#   [Markers]
#     [box]
#       bottom_left = '0.000 0.001 0'
#       inside = refine
#       top_right = '0.005 0.007 0'
#       outside = do_nothing
#       type = BoxMarker
#     []
#   []
# []

[Variables]
  [ls]
    order = FIRST
  []
  [grad_ls]
    family = LAGRANGE_VEC
  []
[]

[AuxVariables]
  [ls_0]
    order = FIRST
  []
  # [combo]
  #   family = MONOMIAL
  #   order = CONSTANT
  # []
[]

[Kernels]
  [grad_ls]
    type = VariableGradientRegularization
    regularized_var = ls_0
    variable = grad_ls
  []
  [time]
    type = TimeDerivative
    variable = ls
  []
  [reinit]
    type = LevelSetGradientRegularizationReinitialization
    variable = ls
    level_set_gradient = grad_ls
    epsilon = 0.04e-3
  []
[]

[Problem]
  type = LevelSetReinitializationProblem
[]

# [UserObjects]
#   [arnold]
#     type = LevelSetOlssonTerminator
#     tol = 0.5
#     min_steps = 10
#   []
# []

[Executioner]
  type = Transient
  solve_type = NEWTON
  start_time = 0
  num_steps = 5
  nl_abs_tol = 1e-10
  nl_forced_its = 3
  nl_max_its = 10
  line_search = none
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package -ksp_type'
  petsc_options_value = 'lu NONZERO superlu_dist preonly'
  # petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -sub_ksp_type'
  # petsc_options_value = ' asm      lu           2               100                 preonly'
  dt = 0.00001
[]

[Outputs]
  exodus = false
  execute_on = 'TIMESTEP_END'
[]
