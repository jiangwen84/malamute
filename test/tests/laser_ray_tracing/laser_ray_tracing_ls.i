[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 1.5e-3
    ymin = 0
    ymax = 3e-3
    nx = 20
    ny = 40
    elem_type = QUAD4
  []
  uniform_refine = 2
[]

[Variables]
  [phi]
  []
[]

[AuxVariables]
  [velocity]
    family = LAGRANGE_VEC
  []
[]

[ICs]
  [phi_ic]
    type = FunctionIC
    function = ls_exact
    variable = phi
  []
  [vel_ic]
    type = VectorFunctionIC
    variable = velocity
    function = velocity_func
  []
[]

[Functions]
  [velocity_func]
    type = ParsedVectorFunction
    # expression_x = '4*y'
    # expression_y = '-4*x'
    #expression_x = 'if(y<2.8 & y>2.2, 1, 0)'
    expression_y = '0'
    expression_x = '0'
  []
[]

[Kernels]
  [time]
    type = TimeDerivative
    variable = phi
  []

  [advection]
    type = LevelSetAdvection
    velocity = velocity
    variable = phi
  []

  [advection_supg]
    type = LevelSetAdvectionSUPG
    velocity = velocity
    variable = phi
  []
  [time_supg]
    type = LevelSetTimeDerivativeSUPG
    velocity = velocity
    variable = phi
  []
[]

[AuxVariables]
  [refractive_index]
    [InitialCondition]
      type = FunctionIC
      function = '1.0 + (x > 2.5) * 0.3'
    []
  []
  [deposition]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Functions/ls_exact]
  type = LevelSetOlssonPlane
  epsilon = 0.04e-3
  point = '1.5e-3 1.5e-3 0'
  normal = '0 1 0'
[]

[RayBCs]
  [kill]
    type = KillRayBC
    boundary = 'top right bottom left'
  []
  # [reflect]
  #   type = ReflectRayBC
  #   boundary = 'left'
  # []
[]

[RayKernels]
  [refraction]
    type = LaserReflectionRayKernel
    phase = phi
    refractive_index = refractive_index
  []
  [deposition]
    type = LaserDepositionRayKernel
    variable = deposition
    depends_on = refraction
    phase = phi
  []
[]

# [UserObjects/study]
#   type = RepeatableRayStudy
#   names = 'diag'
#   start_points = '0 0 0'
#   #end_points = '5 5 0'
#   directions = '0.5 0.45 0'

#   execute_on = TIMESTEP_BEGIN

#   # Needed to cache trace information for RayTracingMeshOutput
#   always_cache_traces = true
#   # Needed to cache Ray data for RayTracingMeshOutput
#   data_on_cache_traces = true
# []

# [UserObjects/study]
#   type = ConeRayStudy

#   start_points = '0 1.6e-3 0'
#   directions = '1 0 0'
#   half_cone_angles = 20
#   ray_data_name = weight

#   azimuthal_quad_orders = 1

#   # Must be set with RayKernels that
#   # contribute to the residual
#   execute_on = TIMESTEP_BEGIN

#   # For outputting Rays
#   always_cache_traces = true
#   data_on_cache_traces = true
# []

[UserObjects/study]
  type = RepeatableRayStudy
  names = 'ray_1
  ray_2
  ray_3
  ray_4
  ray_5'
  start_points = '0 2e-3 0
  1e-5 2e-3 0
  2e-5 2e-3 0
  3e-5 2e-3 0
  4e-5 2e-3 0'
  #end_points = '5 5 0'
  directions = '0 -1 0
  0 -1 0
  0 -1 0
  0 -1 0
  0 -1 0'
  execute_on = TIMESTEP_BEGIN

  # Needed to cache trace information for RayTracingMeshOutput
  always_cache_traces = true
  # Needed to cache Ray data for RayTracingMeshOutput
  data_on_cache_traces = true
[]

# [MultiApps]
#   [reinit]
#     type = LevelSetReinitializationMultiApp
#     input_files = 'sub.i'
#     execute_on = 'timestep_end'
#   []
# []

# [Transfers]
#   [to_sub]
#     type = MultiAppCopyTransfer
#     source_variable = phi
#     variable = phi
#     to_multi_app = reinit
#     execute_on = 'timestep_end'
#   []
#   [to_sub_init]
#     type = MultiAppCopyTransfer
#     source_variable = phi
#     variable = phi_0
#     to_multi_app = reinit
#     execute_on = 'timestep_end'
#   []
#   [from_sub]
#     type = MultiAppCopyTransfer
#     source_variable = phi
#     variable = phi
#     from_multi_app = reinit
#     execute_on = 'timestep_end'
#   []
# []

[Executioner]
  type = Transient
  solve_type = PJFNK
  start_time = 0
  end_time = 1.570796
  scheme = crank-nicolson
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  dt = 0.01
  num_steps = 150
[]

[Outputs]
  exodus = true
  csv = true
  [rays]
    type = RayTracingExodus
    study = study
    execute_on = TIMESTEP_END
  []
[]
