################################################################################

if(NOT (${CMAKE_VERSION} VERSION_LESS "3.8.0"))
  # For CMake 3.8 and above, we can use meta features directly provided by CMake itself
  set(CXX11_FEATURES cxx_std_11)
  set(CXX14_FEATURES cxx_std_14)
  set(CXX17_FEATURES cxx_std_17)
  return()
endif()

################################################################################

set(CXX11_FEATURES
  cxx_auto_type
  cxx_constexpr
)

set(CXX14_FEATURES
  cxx_generic_lambdas
)

set(CXX17_FEATURES

)

################################################################################

# https://cmake.org/cmake/help/v3.1/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html
# cxx_aggregate_default_initializers    Aggregate default initializers, as defined in N3605.
# cxx_alias_templates                   Template aliases, as defined in N2258.
# cxx_alignas                           Alignment control alignas, as defined in N2341.
# cxx_alignof                           Alignment control alignof, as defined in N2341.
# cxx_attributes                        Generic attributes, as defined in N2761.
# cxx_attribute_deprecated              deprecated]] attribute, as defined in N3760.
# cxx_auto_type                         Automatic type deduction, as defined in N1984.
# cxx_binary_literals                   Binary literals, as defined in N3472.
# cxx_constexpr                         Constant expressions, as defined in N2235.
# cxx_contextual_conversions            Contextual conversions, as defined in N3323.
# cxx_decltype_incomplete_return_types  Decltype on incomplete return types, as defined in N3276.
# cxx_decltype                          Decltype, as defined in N2343.
# cxx_decltype_auto                     decltype(auto) semantics, as defined in N3638.
# cxx_default_function_template_args    Default template arguments for function templates, as defined in DR226
# cxx_defaulted_functions               Defaulted functions, as defined in N2346.
# cxx_defaulted_move_initializers       Defaulted move initializers, as defined in N3053.
# cxx_delegating_constructors           Delegating constructors, as defined in N1986.
# cxx_deleted_functions                 Deleted functions, as defined in N2346.
# cxx_digit_separators                  Digit separators, as defined in N3781.
# cxx_enum_forward_declarations         Enum forward declarations, as defined in N2764.
# cxx_explicit_conversions              Explicit conversion operators, as defined in N2437.
# cxx_extended_friend_declarations      Extended friend declarations, as defined in N1791.
# cxx_extern_templates                  Extern templates, as defined in N1987.
# cxx_final                             Override control final keyword, as defined in N2928, N3206 and N3272.
# cxx_func_identifier                   Predefined __func__ identifier, as defined in N2340.
# cxx_generalized_initializers          Initializer lists, as defined in N2672.
# cxx_generic_lambdas                   Generic lambdas, as defined in N3649.
# cxx_inheriting_constructors           Inheriting constructors, as defined in N2540.
# cxx_inline_namespaces                 Inline namespaces, as defined in N2535.
# cxx_lambdas                           Lambda functions, as defined in N2927.
# cxx_lambda_init_captures              Initialized lambda captures, as defined in N3648.
# cxx_local_type_template_args          Local and unnamed types as template arguments, as defined in N2657.
# cxx_long_long_type                    long long type, as defined in N1811.
# cxx_noexcept                          Exception specifications, as defined in N3050.
# cxx_nonstatic_member_init             Non-static data member initialization, as defined in N2756.
# cxx_nullptr                           Null pointer, as defined in N2431.
# cxx_override                          Override control override keyword, as defined in N2928, N3206 and N3272.
# cxx_range_for                         Range-based for, as defined in N2930.
# cxx_raw_string_literals               Raw string literals, as defined in N2442.
# cxx_reference_qualified_functions     Reference qualified functions, as defined in N2439.
# cxx_relaxed_constexpr                 Relaxed constexpr, as defined in N3652.
# cxx_return_type_deduction             Return type deduction on normal functions, as defined in N3386.
# cxx_right_angle_brackets              Right angle bracket parsing, as defined in N1757.
# cxx_rvalue_references                 R-value references, as defined in N2118.
# cxx_sizeof_member                     Size of non-static data members, as defined in N2253.
# cxx_static_assert                     Static assert, as defined in N1720.
# cxx_strong_enums                      Strongly typed enums, as defined in N2347.
# cxx_thread_local                      Thread-local variables, as defined in N2659.
# cxx_trailing_return_types             Automatic function return type, as defined in N2541.
# cxx_unicode_literals                  Unicode string literals, as defined in N2442.
# cxx_uniform_initialization            Uniform initialization, as defined in N2640.
# cxx_unrestricted_unions               Unrestricted unions, as defined in N2544.
# cxx_user_literals                     User-defined literals, as defined in N2765.
# cxx_variable_templates                Variable templates, as defined in N3651.
# cxx_variadic_macros                   Variadic macros, as defined in N1653.
# cxx_variadic_templates                Variadic templates, as defined in N2242.
# cxx_template_template_parameters      Template template parameters, as defined in ISO/IEC 14882:1998.
