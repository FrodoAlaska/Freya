## TODO

- Find a way to listen to a directory of files, so that we can reload assets without having to exit the application

- Web build testing
    - Still some errors on the web side. Some exceptions on the JavaScript side

- ECS 
    - Camera controller component
    - Animation controller component
    - Make sure to destroy an audio source if it is attached to a soon-to-be destroyed entity
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

- Check all `@TEMP` and `@TODO` in the codebase...
