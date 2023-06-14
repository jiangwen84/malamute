[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 1
    ymin = 0
    ymax = 2
    nx = 32
    ny = 64
    elem_type = QUAD4
  []
  [corner_node]
    type = ExtraNodesetGenerator
    new_boundary = 'pinned_node'
    nodes = '0'
    input = gen
  []
  uniform_refine = 1
[]

# [GlobalParams]
#   gravity = '0 -0.98 0'
# []

[AuxVariables]
  [vel_x]
  []
  [vel_y]
  []
[]

[ICs]
  [ls_ic]
    type = FunctionIC
    function = ls_exact
    variable = ls
  []
  [velocity]
    type = VectorConstantIC
    x_value = 1e-10
    y_value = 1e-10
    variable = velocity
  []
[]

[AuxKernels]
  [vel_x]
    type = VectorVariableComponentAux
    variable = vel_x
    vector_variable = velocity
    component = 'x'
  []
  [vel_y]
    type = VectorVariableComponentAux
    variable = vel_y
    vector_variable = velocity
    component = 'y'
  []
[]

[Variables]
  [ls]
  []
  [grad_ls]
    family = LAGRANGE_VEC
  []
  [velocity]
    family = LAGRANGE_VEC
  []
  [p]
  []
  [curvature]
  []
[]

[Problem]
  type = LevelSetProblem
[]

[Functions/ls_exact]
  type = LevelSetOlssonPlane
  epsilon = 0.02
  point = '0 1 0'
  normal = '0 1 0'
[]

[Kernels]
  [grad_ls]
    type = VariableGradientRegularization
    regularized_var = ls
    variable = grad_ls
  []
  [curvature]
    type = LevelSetCurvatureRegularization
    level_set_regularized_gradient = grad_ls
    variable = curvature
    varepsilon = 2e-3
  []

  [mass]
    type = INSADMass
    variable = p
  []
  [mass_pspg]
    type = INSADMassPSPG
    variable = p
  []

  [momentum_time]
    type = INSADMomentumTimeDerivative
    variable = velocity
  []

  [momentum_convection]
    type = INSADMomentumAdvection
    variable = velocity
  []

  [momentum_viscous]
    type = INSADMomentumViscous
    variable = velocity
    viscous_form = 'traction'
  []

  [momentum_pressure]
    type = INSADMomentumPressure
    variable = velocity
    pressure = p
    integrate_p_by_parts = true
  []

  # [gravity]
  #   type = INSADGravityForce
  #   variable = velocity
  # []

  [momentum_supg]
    type = INSADMomentumSUPG
    variable = velocity
    velocity = velocity
  []

  [surface_tension]
    type = TwoPhaseSurfaceTension
    variable = velocity
    level_set_gradient = grad_ls
    curvature = curvature
    surface_tension = 1
  []

  [level_set_time]
    type = ADTimeDerivative
    variable = ls
  []

  [level_set_advection]
    type = LevelSetAdvection
    velocity = velocity
    variable = ls
  []

  [level_set_advection_supg]
    type = LevelSetAdvectionSUPG
    velocity = velocity
    variable = ls
  []

  [level_set_time_supg]
    type = LevelSetTimeDerivativeSUPG
    velocity = velocity
    variable = ls
  []

[]

[BCs]
  # [inflow]
  #   type = VectorFunctionDirichletBC
  #   variable = velocity
  #   boundary = 'bottom'
  #   function_y = '(1-4*(x-0.5)^2)*10'
  #   #function_y = 1
  #   function_x = 0
  # []

  [contact_line]
    type = MovingContactLineBC
    variable = velocity
    level_set_gradient = grad_ls
    beta_n = 1
    beta_cl = 1
    penalty = 10000
    surface_tension = 1
    static_contact_angle = 1.0471975512
    boundary = 'left right'
  []

  [pressure_pin]
    type = DirichletBC
    variable = p
    boundary = 'pinned_node'
    value = 0
  []
[]

[Materials]
  [rho]
    type = LevelSetTwoPhaseMaterial
    prop_name = rho
    prop_value_1 = 1
    prop_value_2 = 1
    phi = ls
    outputs = exodus
  []
  [mu]
    type = LevelSetTwoPhaseMaterial
    prop_name = mu
    prop_value_1 = 1
    prop_value_2 = 1
    phi = ls
    outputs = exodus
  []
  # [./const]
  #   type = GenericConstantMaterial
  #   prop_names = 'rho mu'
  #   prop_values = '1  1'
  # [../]
  [ins_mat]
    type = INSADTauMaterial
    velocity = velocity
    pressure = p
    alpha = .1
  []

  # [ins_melt_pool_mat]
  #     type = INSMeltPoolMaterial
  #     level_set_gradient = grad_ls
  #     velocity = velocity
  #     pressure = p
  #     alpha = .1
  #     curvature = curvature
  #     surface_tension = 24
  #   []
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
    solve_type = 'NEWTON'
  []
[]

[Postprocessors]
  [cfl]
    type = LevelSetCFLCondition
    velocity = velocity
    execute_on = 'initial timestep_end'
  []
  [area]
    type = LevelSetVolume
    threshold = 0.5
    variable = ls
    location = outside
    execute_on = 'timestep_begin timestep_end'
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  start_time = 0
  dt = 0.02
  end_time = 1000
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type -ksp_type'
  petsc_options_value = 'lu superlu_dist preonly'
  line_search = 'none'
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-8
  nl_max_its = 15
  l_tol = 1e-6
  l_max_its = 20
  # dtmax = 0.001
  # [TimeStepper]
  #   type = PostprocessorDT
  #   postprocessor = cfl
  #   scale = 0.01
  #   dt = 0.005
  # []
[]

[Outputs]
  exodus = true
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
