# Edge color regressions

Build MeshLab and its plugins together; `CEdgeO` now includes an RGBA color component.
The mesh-model data mask records whether an edge-color attribute is present.

```sh
cmake -S . -B build -DMESHLAB_BUILD_TESTS=ON
cmake --build build --parallel 6
ctest --test-dir build -R '^edge_color_' --output-on-failure
```

The tests use the configured VCGLib dependency and Qt 5:

- `edge_color_io`: the example PLY, ASCII and binary roundtrips, both binary byte
  orders, optional alpha, absent/incomplete colors, invalid input, duplicate edges,
  empty edge collections, copying, state restoration, and compaction.
- `edge_color_policy`: initial point/edge/face visibility, independent color
  attributes, and legacy/versioned rendering-settings serialization.
- `edge_color_render`: pixel checks with GPU buffers and the immediate-mode
  fallback, multiple viewers, edits, mixed geometry, buffer cleanup, generated
  polygonal boundaries, and textured faces.
- `edge_color_ui`: wire color/source controls and PLY export options. When
  `io_base` is built, this also loads the real plugin and checks its save/reload
  behavior, including default exports of uncolored edges.

Rendering needs a desktop compatibility OpenGL context. On Linux without a
desktop session, run the CTest command under `xvfb-run -a` with Mesa installed.
The rendering test reports a skip if it cannot create a context. The UI test
uses Qt's offscreen platform.

The rendering test writes `edge-color-gpu.png` and `edge-color-fallback.png` in
the CTest working directory (`build/tests`). Both show three RGB points and
uniform yellow/cyan edges from [the example fixture](fixtures/colored_edges.ply).

The PLY and GPU implementation lives in the VCGLib submodule. Keep its commit
and the MeshLab submodule reference together when transferring this change.
