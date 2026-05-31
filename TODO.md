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
    - Propogate the changes to other systems 
        - Post-process 
            - Try things out with just a default post-process pass 
            - Add another pass that is more wild 
            - Test out everything
            - Finalize and polish
    - Add a useful logger func to `sokol_gfx`
    - Check ImGui
    - Change the UI renderer as well

- Web build
    - Yep. We need to either overhaul the graphics backend completely or just switch to SOKOL. It's a mess, dude...

- Update the README after finishing the graphics backend refactor and succefully running a working web build

- Audio
    - OpenAL is a bloated mess that needs to be completely removed. Please use MiniAudio instead. Thanks.

- Renderer
    - Dynamic 2D lighting

- Misc.
    - Test more shit with compute shaders. Particles, post-processing, and other effects
    - A more detecated LUA layer
    - Memory arenas and custom allocaters

## Fixes 

- The way file watchers work in the asset group sucks. A bunch of allocations for no reason _at all_. Please fix.
- Check all `@TEMP` and `@TODO` in the codebase...
