# Whitespace

Whitespace is a small scale Editor for 2D scenes.
Scenes will be saved as YAML files.

## Project Structure
A project consitst of one project file (.wsproj) and up to multiple scene files (.wsscene).
The project file should be at the root of the directory structure.
For reference see the example project under ``example/``.

## Creating new Scenes
The current version doesn't allow for creating scenes through the editor.
Therefore user must create ``.wsscene`` files themself.
It is recommended to copy an existing scene file and rename it.
Be aware that for the new scene to be registered to the editor scene select you must restart the editor, alternatively you can reopen the project with the ``File/Open Recent..``.

## Keyboard Shortcuts
These are the currently supported keyboard shortcuts:
- ``DEL`` : delete current selected scene element from scene
- ``SPACE+LMB``: pan the scene
- ``LMB``: select scene element; drag scene elements

## Possible Future Features
- undo and redo of actions
- copying/duplicating of scene elements already placed in the scene
- mulit scene element select and operations
- creating scenes through the editor
- resizeability of the window
