# Библиотека *inventory*

Библиотека функций для работы с инвентарем.

```lua
-- Возвращает id предмета и его количество. id = 0 (core:empty) обозначает, что слот пуст.
inventory.get(
    -- id инвентаря
    invid: int,
    -- индекс слота
    slot: int
) -> int, int

-- Устанавливает содержимое слота.
inventory.set(
    -- id инвентаря
    invid: int,
    -- индекс слота
    slot: int,
    -- id предмета
    itemid: int,
    -- количество предмета
    count: int
)

-- Возвращает размер инвентаря (число слотов).
-- Если указанного инвентаря не существует, бросает исключение.
inventory.size(invid: int) -> int

-- Добавляет предмет в инвентарь. 
-- Если не удалось вместить все количество, возвращает остаток.
inventory.add(
    -- id инвентаря
    invid: int, 
    -- id предмета
    itemid: int, 
    -- количество предмета
    count: int
) -> int

-- Функция возвращает id инвентаря блока.
-- Если блок не может иметь инвентарь - возвращает 0.
inventory.get_block(x: int, y: int, z: int) -> int

-- Привязывает указанный инвентарь к блоку.
inventory.bind_block(invid: int, x: int, y: int, z: int)

-- Отвязывает инвентарь от блока.
inventory.unbind_block(x: int, y: int, z: int)

-- Удаляет инвентарь.
inventory.remove(invid: int)
```

> [!WARNING]
> Инвентари, не привязанные ни к одному из блоков, удаляются при выходе из мира.

```lua
-- Создаёт инвентарь и возвращает id.
inventory.create(size: int) -> int

-- Создает копию инвентаря и возвращает id копии.
-- Если копируемого инвентаря не существует, возвращает 0.
inventory.clone(invid: int) -> int

-- Перемещает предмет из slotA инвентаря invA в slotB инвентаря invB.
-- invA и invB могут указывать на один инвентарь.
-- slotB будет выбран автоматически, если не указывать явно.
inventory.move(invA: int, slotA: int, invB: int, slotB: int)
```
