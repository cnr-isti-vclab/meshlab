# Roadmap  {#roadmap}

Super4PCS refactoring started with v1.1.x is now almost done.
Current changes are now focusing on:
* stabilizing the API
* improving testing
* adding wrappers to point-cloud and mesh processing libraries/softwares.
See the Pull Request to the next release v1.1.3 [here](https://github.com/nmellado/Super4PCS/pull/45).

At long term, our goal is to:
* implement other approaches such as [Generalized4PCS](http://dl.acm.org/citation.cfm?id=2758287) or the recent [2 -points+normal](http://ieeexplore.ieee.org/abstract/document/7989664/) variant.
* add bridge to local registration implementation (PCL ?).

In parallel, we also plan to release datasets with ground-truth transformations, to ease comparison between registration techniques.
