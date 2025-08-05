# Telegram Bot — Камень, Ножницы, Бумага

Этот проект — Telegram-бот на C++ с использованием библиотеки [tgbot-cpp](https://github.com/reo7sp/tgbot-cpp), который позволяет играть в "Камень, ножницы, бумага" как с ботом, так и с другом через комнаты.

## Возможности
- **Игра с ботом** — до трёх побед.
- **Игра с другом** — создание комнаты и игра до трёх побед.
- **Поддержка имён пользователей** — отображаются username или имя из профиля.
- **Подсчёт очков** — автоматическая остановка игры при достижении 3 побед.

## Команды
- `/start` — выбор режима игры.
- `/botgame` — игра с ботом.
- `/friendgame` — игра с другом.
- `/create` — создать комнату.
- `/join <код>` — присоединиться к комнате.

## Требования
- **C++17 и выше**
- **CMake 3.20+**
- [Boost](https://www.boost.org/) (модуль property_tree)
- [tgbot-cpp](https://github.com/reo7sp/tgbot-cpp)
- [vcpkg](https://github.com/microsoft/vcpkg) (рекомендуется для установки зависимостей)

## Установка зависимостей через vcpkg
1. Клонировать vcpkg:
```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
```
2. Установить необходимые библиотеки:
vcpkg install boost-property-tree:x64-windows
vcpkg install tgbot-cpp:x64-windows

## Сборка
1. Клонировать репозиторий:
```bash
git clone https://github.com/mEmmEmmEm/tg_bot_rps.git
cd tg_bot_rps
```
2. Создать папку build:
```bash
mkdir build
cd build
```
3. Запустить CMake с указанием пути к vcpkg:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/.../vcpkg/scripts/buildsystems/vcpkg.cmake"
```
4. Собрать проект:
```bash
cmake --build build --config Release
```
