# TODO

- Maybe a Freya logo?

## Additional Modules

- A tweening module using the `tweeny` library. It'll be a good and cheap way of making animations
- Also, perhaps a library for dynamic code reloading
- Memory arenas and custom allocaters

## Tests

- Test more shit with compute shaders. Particles, post-processing, and other effects

## Improvements

- Async asset loading using `sokol_fetch` and `EnkiTS`
- Improve the UI module
- A more detecated LUA layer

## Fixes 

- (Renderer): 
    - MSAA is currently very fucked when used with post-processing effects. We should probably use MSAA-resolve attachments to fix this. But I don't know. Research more.
    - Resize post-processing effects. We need to probably use a combination of `sg_alloc_*`, `sg_init_*`, and `sg_uninit` in order to implement this. It should be easy, but good luck either way.

- The way file watchers work in the asset group sucks. A bunch of allocations for no reason _at all_. Please fix.
- Check all `@TEMP` and `@TODO` in the codebase...
