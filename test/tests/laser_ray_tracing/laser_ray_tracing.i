[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 100
    ny = 100
    xmax = 5
    ymax = 5
  []
[]

[Variables/phase]
  [InitialCondition]
    type = FunctionIC
    function = ls_exact
    #function = '(x > 2.25) * 0.5 + (x > 2.5) * 0.5'
  []
[]

[AuxVariables]
  [phi]
    order = FIRST
    family = LAGRANGE
  []
[]

[AuxVariables] #Creates second constant phase
  [phase2]
  []
[]

[ICs] #Sets the IC for the second constant phase
  [phase2_IC] #Creates circles with smooth interfaces at random locations
    variable = phase2
    type = MultiSmoothCircleIC
    int_width = 0.3
    numbub = 20
    bubspac = 0.75
    radius = 0.25
    outvalue = 0
    invalue = 1
  []
[]

# [AuxKernels]
#   [initial_cond_aux]
#     type = SolutionAux
#     solution = solu_phi
#     execute_on = INITIAL
#     variable = phi
#   []
# []

# [UserObjects]
#   [solu_phi]
#     type = SolutionUserObject
#     execute_on = INITIAL
#     mesh = save_ls.e
#     timestep = LATEST
#     system_variables = phi
#   []
# []

[AuxVariables]
  [refractive_index]
    [InitialCondition]
      type = FunctionIC
      function = '1.0 + (x > 2.5) * 0.3'
    []
  []
  [ls]
    [InitialCondition]
      type = FunctionIC
      function = ls_exact
    []
  []
  [deposition]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Functions/ls_exact]
  #type = LevelSetOlssonPlane
  # epsilon = 0.05
  # point = '2.5 2.5 0'
  # normal = '1 0 0'
  type = LevelSetOlssonBubble
  epsilon = 0.05
  center = '2.5 2.5 0'
  radius = 1.
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
    phase = ls
    refractive_index = refractive_index
  []
  [deposition]
    type = LaserDepositionRayKernel
    variable = deposition
    depends_on = refraction
    phase = ls
  []
[]

[UserObjects/study]
  type = RepeatableRayStudy
  names = 'diag'
  start_points = '0 0 0'
  #end_points = '5 5 0'
  directions = '0.5 0.5 0'

  execute_on = INITIAL

  # Needed to cache trace information for RayTracingMeshOutput
  always_cache_traces = true
  # Needed to cache Ray data for RayTracingMeshOutput
  data_on_cache_traces = true
[]

# [UserObjects/study]
#   type = ConeRayStudy

#   start_points = '0 2.5 0'
#   directions = '1 0 0'
#   half_cone_angles = 10
#   ray_data_name = weight

#   azimuthal_quad_orders = 100

#   # Must be set with RayKernels that
#   # contribute to the residual
#   execute_on = TIMESTEP_BEGIN

#   # For outputting Rays
#   always_cache_traces = true
#   data_on_cache_traces = true
# []

[Executioner]
  type = Steady
[]

[Problem]
  solve = false
[]

[Outputs]
  exodus = true
  csv = true
  [rays]
    type = RayTracingExodus
    study = study
    execute_on = FINAL
  []
[]
