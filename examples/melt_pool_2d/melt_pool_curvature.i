[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 0.01
    ymin = 0
    ymax = 0.01
    nx = 50
    ny = 50
    elem_type = QUAD4
  []
  [corner_node]
    type = ExtraNodesetGenerator
    new_boundary = 'pinned_node'
    # coord = '0.0 0.01'
    nodes = '0'
    input = gen
  []
  #uniform_refine = 3
[]

[Adaptivity]
  steps = 3
  marker = box
  max_h_level = 3
  initial_steps = 3
  stop_time = 1.0e-10
  [Markers]
    [box]
      bottom_left = '0.000 0.004 0'
      inside = refine
      top_right = '0.01 0.006 0'
      outside = do_nothing
      type = BoxMarker
    []
  []
[]

[ICs]
  [ls_ic]
    type = FunctionIC
    function = ls_exact
    variable = ls
  []
[]

[AuxVariables]
  [ls]
  []
  [temp]
    initial_condition = 1500
  []
[]

[Variables]
  [curvature]
  []
  [grad_ls]
    family = LAGRANGE_VEC
  []
[]

[Functions/ls_exact]
  type = LevelSetOlssonPlane
  epsilon = 0.00004
  point = '0.005 0.005 0'
  normal = '0 1 0'
[]

[Kernels]
  [curvature]
    type = LevelSetCurvatureRegularization
    level_set_regularized_gradient = grad_ls
    variable = curvature
    varepsilon = 2e-4
  []

  [grad_ls]
    type = VariableGradientRegularization
    regularized_var = ls
    variable = grad_ls
  []
[]

# [MultiApps]
#   [reinit]
#     type = LevelSetReinitializationMultiApp
#     input_files = 'reinit.i'
#     execute_on = TIMESTEP_END
#   []
# []

# [Transfers]
#   [to_sub]
#     type = MultiAppCopyTransfer
#     source_variable = ls
#     variable = ls
#     to_multi_app = reinit
#     execute_on = 'timestep_end'
#   []

#   [to_sub_init]
#     type = MultiAppCopyTransfer
#     source_variable = ls
#     variable = ls_0
#     to_multi_app = reinit
#     execute_on = 'timestep_end'
#   []

#   [from_sub]
#     type = MultiAppCopyTransfer
#     source_variable = ls
#     variable = ls
#     from_multi_app = reinit
#     execute_on = 'timestep_end'
#   []
# []

[Preconditioning]
  [SMP]
    type = SMP
    full = true
    solve_type = 'NEWTON'
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  dt = 1e-3
  nl_abs_tol = 1e-7
  num_steps = 1000
  line_search = 'l2'

  # petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -pc_factor_mat_solver_package -ksp_type'
  # petsc_options_value = 'lu NONZERO 1e-10 superlu_dist preonly'

  petsc_options_iname = '-pc_type -sub_pc_factor_levels -ksp_gmres_restart'
  petsc_options_value = 'asm      6                     200'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -options_left -ksp_monitor_singular_value'

  # petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'
  # petsc_options = '-snes_converged_reason -ksp_converged_reason -options_left -ksp_monitor_singular_value'

  # petsc_options = '-snes_converged_reason -ksp_converged_reason -options_left'
  # petsc_options_iname = '-ksp_gmres_restart -pc_factor_shift_type -pc_factor_shift_amount -pc_type'
  # petsc_options_value = '100                NONZERO               1e-10                   ilu'

  # petsc_options_iname = '-pc_type -ksp_grmres_restart -sub_ksp_type -sub_pc_type -pc_asm_overlap'
  # petsc_options_value = '  asm         101              preonly         ilu        3'

  # petsc_options_iname = '-ksp_max_it -ksp_gmres_restart -pc_type -snes_max_funcs -sub_pc_factor_levels'
  # petsc_options_value = ' 100         100                asm      1000000         3'

  nl_forced_its = 3
  nl_max_its = 12
  l_max_its = 100

  nl_div_tol = 1e20
  #automatic_scaling = true
[]

[Debug]
  # show_var_residual = 'temp curvature p'
  show_var_residual_norms = true
[]

[Outputs]
  exodus = true
[]
