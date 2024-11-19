# Скриптинг

В качестве языка сценариев используется LuaJIT

Подразделы:
- [События движка](scripting/events.md)
- [Пользовательский ввод](scripting/user-input.md)
- [Файловая система и сериализация](scripting/filesystem.md)
- [Свойства и методы UI элементов](scripting/ui.md)
- [Сущности и компоненты](scripting/ecs.md)
- [Библиотеки](#)
    - [block](scripting/builtins/libblock.md)
    - [cameras](scripting/builtins/libcameras.md)
    - [entities](scripting/builtins/libentities.md)
    - [file](scripting/builtins/libfile.md)
    - [gui](scripting/builtins/libgui.md)
    - [hud](scripting/builtins/libhud.md)
    - [inventory](scripting/builtins/libinventory.md)
    - [item](scripting/builtins/libitem.md)
    - [mat4](scripting/builtins/libmat4.md)
    - [pack](scripting/builtins/libpack.md)
    - [player](scripting/builtins/libplayer.md)
    - [quat](scripting/builtins/libquat.md)
    - [rules](scripting/builtins/librules.md)
    - [time](scripting/builtins/libtime.md)
    - [utf8](scripting/builtins/libutf8.md)
    - [vec2, vec3, vec4](scripting/builtins/libvecn.md)
    - [world](scripting/builtins/libworld.md)
- [Расширения стандартных библиотек](scripting/extensions.md)
- [Модуль core:bit_converter](scripting/modules/core_bit_converter.md)
- [Модуль core:data_buffer](scripting/modules/core_data_buffer.md)
- [Модули core:vector2, core:vector3](scripting/modules/core_vector2_vector3.md)

## Аннотации типов данных

В документации к Lua библиотекам используются аннотации типов,
не являющиеся частью синтаксиса Lua.

- vector - массив из трех или четырех чисел
- vec2 - массив из двух чисел
- vec3 - массив из трех чисел
- vec4 - массив из четырех чисел
- quat - массив из четырех чисел - кватернион
- matrix - массив из 16 чисел - матрица

```lua
require "контентпак:имя_модуля" -- загружает lua модуль из папки modules (расширение не указывается)
```
