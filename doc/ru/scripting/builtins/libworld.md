# Библиотека *world*

```lua
-- Проверяет, открыт ли мир
world.is_open() -> bool

-- Возвращает информацию о мирах.
world.get_list() -> массив таблиц {
    -- название мира
    name: str,
    -- предпросмотр (автоматически загружаемая текстура)
    icon: str
}

-- Возвращает текущее игровое время от 0.0 до 1.0, где 0.0 и 1.0 - полночь, 0.5 - полдень.
world.get_day_time() -> number

-- Устанавливает указанное игровое время.
world.set_day_time(time: number)

-- Устанавливает указанную скорость смены времени суток.
world.set_day_time_speed(value: number)

-- Возвращает скорость скорость смены времени суток.
world.get_day_time_speed() -> number

-- Возвращает суммарное время, прошедшее в мире.
world.get_total_time() -> number

-- Возвращает зерно мира.
world.get_seed() -> int

-- Возвращает имя генератора.
world.get_generator() -> str

-- Проверяет существование мира по имени.
world.exists() -> bool

-- Проверяет является ли текущее время днём. От 0.333(8 утра) до 0.833(8 вечера).
world.is_day() -> bool

-- Проверяет является ли текущее время ночью. От 0.833(8 вечера) до 0.333(8 утра).
world.is_night() -> bool
```