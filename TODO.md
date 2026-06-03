## TODO

### Components

- Tilemap
    - Tilemap colliders? Maybe using chain shapes?
    - Optimize the tilemap

- ECS 
    - Camera controller component
    - Improve the UI situation to make it more compatibale with the ECS
    - GUI component to render through the GUI

### Major Systems

- Update the project template again. Get help from the latest game. Especially the CMake and script stuff

- Audio
    - OpenAL is a bloated mess that needs to be completely removed. Please use MiniAudio instead. Thanks.

- Renderer
    - Dynamic 2D lighting

- Misc.
    - Test more shit with compute shaders. Particles, post-processing, and other effects
    - A more detecated LUA layer
    - Memory arenas and custom allocaters

## Fixes 

- (Renderer): 
    - MSAA is currently very fucked when used with post-processing effects. We should probably use MSAA-resolve attachments 
to fix this. But I don't know. Research more.
    - Resize post-processing effects. We need to probably use a combination of `sg_alloc_*`, `sg_init_*`, and `sg_uninit` in 
order to implement this. It should be easy, but good luck either way.

- The way file watchers work in the asset group sucks. A bunch of allocations for no reason _at all_. Please fix.
- Check all `@TEMP` and `@TODO` in the codebase...
