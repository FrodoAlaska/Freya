## TODO

- Major systems to rework 
    - OpenAL is a bloated mess that needs to be completely removed. Please use MiniAudio instead. Thanks.

- Update the project template. It's super out of date.
- Load particle emitter from config file

- Web build testing
    - Still some errors on the web side. Some exceptions on the JavaScript side

- ECS 
    - Animation controller component
        - Add in the GUI
    - Camera controller component
    - Improve the UI situation to make it more compatibale with the ECS
    - GUI component to render through the GUI

- Tilemap
    - Tilemap colliders? Maybe using chain shapes?
    - Optimize the tilemap

- Renderer
    - Dynamic 2D lighting

- Misc.
    - A more detecated LUA layer
    - Memory arenas and custom allocaters

## Fixes 

- The way file watchers work in the asset group sucks. A bunch of allocations for no reason _at all_. Please fix.
- Check all `@TEMP` and `@TODO` in the codebase...
