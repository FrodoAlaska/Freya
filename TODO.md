## TODO

- Find a way to listen to a directory of files, so that we can reload assets without having to exit the application
- We have to find a way to add a custom asset in the asset group to make integration with applications easier
- Add LUA as a configuration file asset using `sol3`?

- Web build testing
    - Main loop function for both the web and desktop

- ECS 
    - Camera controller component
    - Animation controller component
    - Make sure to destroy an audio source if it is attached to a soon-to-be destroyed entity
    - Improve the UI situation to make it more compatibale with the ECS
    - Search through the world 
    - Custom component

- Tilemap
    - Tilemap colliders? Maybe using chain shapes?
    - Optimize the tilemap

- Renderer
    - Dynamic 2D lighting

- Misc.
    - Memory arenas and custom allocaters

## Fixes 

- Check all `@TEMP` and `@TODO` in the codebase...
