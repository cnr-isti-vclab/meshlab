# Getting Started {#gettingstarted}

Since Release v1.1.x we improved the usability of Super4PCS as an application or as a library.

To run the application, you just need to:

 1. Compile the source code (checkout the @ref compilation page), or download the latest [release](https://github.com/nmellado/Super4PCS/releases) to obtain the Super4PCS binary.
 2. Go to the script folder, and run the `run-example` script from there.

Then, depending on plans, you also might want to look at:
 - our @ref demos, the use the library with the Point Cloud Library or with a Meshlab plugin.
 - the @ref datasets page, to *further test the application on real datasets*, including models from the original paper.
 - the @ref usage page for more details on the Super4PCS parameters.
 - the [Match4PCSBase](@ref GlobalRegistration::Match4PCSBase) class, if you plan to *use the library in you own software*.
   This is the base class for registration algorithms, providing the [ComputeTransformation](@ref GlobalRegistration::Match4PCSBase::ComputeTransformation) method. Two different algorithms are today available: [4PCS](@ref GlobalRegistration::Match4PCS) and its more efficient variant [Super4PCS](@ref GlobalRegistration::MatchSuper4PCS)

