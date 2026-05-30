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

- Graphics 
    - Remove any extra files
    - Propogate the changes to other systems 
        - Physics debug draw 
        - Post-process 
    - Add a useful logger func to `sokol_gfx`
    - Change the UI renderer as well

- Web build
    - Yep. We need to either overhaul the graphics backend completely or just switch to SOKOL. It's a mess, dude...

- Update the README after finishing the graphics backend refactor and succefully running a working web build

- Audio
    - OpenAL is a bloated mess that needs to be completely removed. Please use MiniAudio instead. Thanks.

- Renderer
    - Dynamic 2D lighting

- Misc.
    - A more detecated LUA layer
    - Memory arenas and custom allocaters

## Fixes 

- The way file watchers work in the asset group sucks. A bunch of allocations for no reason _at all_. Please fix.
- Check all `@TEMP` and `@TODO` in the codebase...
