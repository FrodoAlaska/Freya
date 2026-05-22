## TODO

### Components

- Improve art integration into the engine. How? Figure it out.

- Tilemap
    - Tilemap colliders? Maybe using chain shapes?
    - Optimize the tilemap

- ECS 
    - Camera controller component
    - Improve the UI situation to make it more compatibale with the ECS
    - GUI component to render through the GUI

### Major Systems

- Major systems to rework 
    - OpenAL is a bloated mess that needs to be completely removed. Please use MiniAudio instead. Thanks.

- Web build testing
    - Still some errors on the web side. Some exceptions on the JavaScript side

- Renderer
    - Dynamic 2D lighting

- Misc.
    - A more detecated LUA layer
    - Memory arenas and custom allocaters

## Fixes 

- The way file watchers work in the asset group sucks. A bunch of allocations for no reason _at all_. Please fix.
- Check all `@TEMP` and `@TODO` in the codebase...
