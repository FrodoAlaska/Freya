## TODO

### Major Systems

- Flecs vs. EnTT

- Misc.
    - Async asset loading using `sokol_fetch`
    - Test more shit with compute shaders. Particles, post-processing, and other effects
    - A more detecated LUA layer
    - Memory arenas and custom allocaters

## Fixes 

- (Renderer): 
    - MSAA is currently very fucked when used with post-processing effects. We should probably use MSAA-resolve attachments to fix this. But I don't know. Research more.
    - Resize post-processing effects. We need to probably use a combination of `sg_alloc_*`, `sg_init_*`, and `sg_uninit` in order to implement this. It should be easy, but good luck either way.

- The way file watchers work in the asset group sucks. A bunch of allocations for no reason _at all_. Please fix.
- Check all `@TEMP` and `@TODO` in the codebase...
