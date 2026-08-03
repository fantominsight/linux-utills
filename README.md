# linux-utills

Небольшой набор утилит для Linux, которые я пишу параллельно с изучением C++.

Все программы распространяются бесплатно.  
Стараюсь обновлять репозиторий примерно раз в неделю и иногда добавлять небольшие «игрушки».

> **Проверка безопасности**  
> Перед запуском любых бинарников рекомендую проверить их на [VirusTotal](https://www.virustotal.com) или другими средствами.

---

## Утилиты

### 1. FantominsightSU (All-in-One CLI)
Универсальный системный комбайн с удобным интерактивным меню.

**Возможности:**
- Поиск программ (`which` / `whereis`)
- **FantomPing** — проверка доступности серверов и сайтов с замером задержки
- Системная информация: текущая директория (`pwd`) и имя пользователя (`whoami`)
- Управление питанием: перезагрузка и выключение компьютера

**Как пользоваться:**
```bash
./FantominsightSU
```

**Исходный код:** [`source-code/CLI/allinone.cpp`](source-code/CLI/allinone.cpp)

**Релиз:** [CLI_v1](https://github.com/fantominsight/linux-utills/releases/tag/CLI_v1)

---

### 2. MTKConverter
Простой конвертер миль в километры.

**Как пользоваться:**
```bash
./MTKConverter
```
Программа запрашивает расстояние в милях и выводит результат в километрах.  
Для выхода введите `exit`.

**Исходный код:** [`source-code/MTKConverter/`](source-code/MTKConverter/)

---

### 3. shutdown
Утилита для отложенного выключения компьютера.

При запуске открывает новое окно терминала с 30-секундным таймером.  
По истечении времени выполняется `poweroff`.

**Как пользоваться:**
```bash
./shutdown
```

Чтобы **отменить** выключение — просто закройте появившееся окно терминала.

> Требуется `gnome-terminal`.

**Исходный код:** [`source-code/shutdown/`](source-code/shutdown/)

---

### 4. snake3d
Полноценная 3D-змейка, написанная с использованием OpenGL (FreeGLUT + GLM).

**Особенности:**
- Трёхмерная сетка 20×20
- Плавная камера
- Освещение и объёмные кубики
- Пульсирующая еда
- Подсчёт очков, пауза, победа/поражение

**Как запускать:**
```bash
./snake3d
```

Управление:
- Стрелки — направление
- `P`, `SPACE` — пауза
- `R`, `SPACE` — рестарт (после окончания игры)

**Исходный код и сборка:** [`source-code/Python-In-Terminal/snake3d/`](source-code/Python-In-Terminal/snake3d/)

---

### 5. FantominsightUI (GUI на Qt6)
Графический набор сетевых и системных инструментов (Qt 6, C++20).

**Возможности:**
- 🌐 **Сеть** — проверка сайта (ping + сводка), ping, DNS-запросы (A/AAAA/MX/TXT/CNAME/NS/SRV/PTR), трассировка маршрута, проверка TCP-портов
- 💻 **Система** — информация о ПК, загрузка CPU в реальном времени, использование RAM, смонтированные диски
- 🔧 **Инструменты** — хеширование файлов (MD5…SHA3-512), генератор паролей, Base64

**Как пользоваться:**
```bash
./FantominsightUI
```

**Исходный код:** [`source-code/FantominsightUI/`](source-code/FantominsightUI/)

**Сборка (требуется Qt 6 и CMake):**
```bash
cmake -S source-code/FantominsightUI -B build
cmake --build build -j
```

---

## Установка / Запуск

1. Скачайте нужный бинарник из папки [`Utills/`](Utills/)
2. Сделайте его исполняемым:
   ```bash
   chmod +x FantominsightSU
   chmod +x MTKConverter
   chmod +x shutdown
   chmod +x snake3d
   chmod +x FantominsightUI
   ```
3. Запустите:
   ```bash
   ./имя_утилиты
   ```

**Важно:** в файлах репозитория всегда находятся **только последние версии** бинарников.  
Старые версии можно скачать в [Releases](https://github.com/fantominsight/linux-utills/releases).

---

## Возможные проблемы и решения

### 1. `Permission denied` при запуске
Бинарник не имеет прав на выполнение.

**Решение:**
```bash
chmod +x имя_утилиты
```

---

### 2. FantominsightUI не запускается / ошибка про библиотеки Qt
Бинарник динамически связан с Qt 6. Если Qt 6 не установлен в системе, программа не запустится.

**Решение (Ubuntu / Debian):**
```bash
sudo apt update
sudo apt install qt6-base-dev libqt6widgets6 libqt6network6
```

**Решение (Fedora):**
```bash
sudo dnf install qt6-qtbase qt6-qtbase-gui
```

**Решение (Arch):**
```bash
sudo pacman -S qt6-base
```

Если ошибка остаётся — соберите из исходников (см. раздел «Сборка из исходников»).

---

### 3. `shutdown` не открывает окно / ничего не происходит
Утилита требует `gnome-terminal`.

**Решение:**
```bash
sudo apt install gnome-terminal   # Ubuntu/Debian
# или
sudo dnf install gnome-terminal   # Fedora
```

Также команда `poweroff` может требовать прав администратора. В этом случае используйте:
```bash
sudo ./shutdown
```

---

### 4. snake3d не запускается / ошибка OpenGL
Не хватает библиотек OpenGL / FreeGLUT.

**Решение (Ubuntu / Debian):**
```bash
sudo apt install freeglut3 libgl1 libglu1-mesa
```

**Решение (Fedora):**
```bash
sudo dnf install freeglut mesa-libGL mesa-libGLU
```

---

### 5. Ping / трассировка не работают в FantominsightUI или FantominsightSU
- Утилита `ping` или `traceroute` не установлена
- На некоторых системах для `ping` нужны дополнительные права

**Решение:**
```bash
sudo apt install iputils-ping traceroute   # Ubuntu/Debian
sudo dnf install iputils traceroute        # Fedora
```

Если `ping` пишет «Operation not permitted»:
```bash
sudo setcap cap_net_raw+ep $(which ping)
```

---

### 6. Ошибка `No such file or directory` при запуске бинарника
Обычно означает, что система не нашла динамическую библиотеку (особенно часто с Qt).

**Диагностика:**
```bash
ldd ./FantominsightUI | grep "not found"
```

Установите недостающие пакеты (см. пункт 2) или соберите программу из исходников.

---

### 7. Бинарник не запускается на другой архитектуре
Все бинарники собраны под **x86_64**. На ARM (Raspberry Pi, Apple Silicon через эмуляцию и т.д.) они работать не будут.

**Решение:** соберите из исходников на своей машине.

---

## Сборка из исходников

### 1. Где взять исходники

**Вариант A — клонировать репозиторий** (нужен Git):
```bash
git clone https://github.com/fantominsight/linux-utills.git
cd linux-utills
```

**Вариант B — скачать архив:**
- На странице репозитория нажмите зелёную кнопку **Code → Download ZIP**
- Либо в разделе [Releases](https://github.com/fantominsight/linux-utills/releases) — ссылки `Source code (zip)` / `Source code (tar.gz)` у нужной версии

Все исходники лежат в папке **`source-code/`**. Команды ниже выполняются из корня репозитория.

> **Про архитектуру.** Готовые бинарники в `Utills/` собраны под **x86_64**. Для другой архитектуры (ARM64 / AArch64 — Raspberry Pi, многие SBC и серверы; ARM32 / armhf; x86) исходники нужно собирать **нативно, на самой целевой машине** — в этом случае бинарник соберётся под её архитектуру. Инструкции ниже универсальны для любой из них: просто выполните их на нужной системе.

### 2. Общие требования

- Компилятор C++ (g++ или clang++)
- **CMake** (для FantominsightUI и snake3d)

### 3. Установка зависимостей по ОС

**Ubuntu / Debian** (в т.ч. Raspberry Pi OS):
```bash
sudo apt update
sudo apt install build-essential cmake git        # компилятор, CMake, Git
sudo apt install qt6-base-dev libqt6widgets6 libqt6network6   # Qt 6 для FantominsightUI
sudo apt install freeglut3-dev libglm-dev libgl1-mesa-dev libglu1-mesa-dev  # для snake3d
```

**Fedora / RHEL / CentOS Stream:**
```bash
sudo dnf install gcc-c++ cmake git qt6-qtbase-devel freeglut-devel glm-devel mesa-libGL-devel mesa-libGLU-devel
```

**Arch Linux / Manjaro:**
```bash
sudo pacman -S gcc cmake git qt6-base freeglut glm mesa
```

**openSUSE:**
```bash
sudo zypper install gcc-c++ cmake git qt6-base-devel freeglut-devel glm-devel Mesa-libGL-devel Mesa-libGLU-devel
```

### 4. Сборка по проектам

#### FantominsightUI (GUI на Qt6) — `source-code/FantominsightUI/`
Требуется Qt 6 (Widgets, Network) и CMake ≥ 3.16:
```bash
cmake -S source-code/FantominsightUI -B build-fantominsightui
cmake --build build-fantominsightui -j
./build-fantominsightui/FantominsightUI
```
Для работы сетевых функций нужны системные утилиты `ping` и `traceroute` (см. раздел «Возможные проблемы»).

#### FantominsightSU (All-in-One CLI) — `source-code/CLI/allinone.cpp`
```bash
g++ source-code/CLI/allinone.cpp -o FantominsightSU
./FantominsightSU
```

#### MTKConverter — `source-code/MTKConverter/main.cpp`
```bash
g++ source-code/MTKConverter/main.cpp -o MTKConverter
./MTKConverter
```

#### shutdown — `source-code/shutdown/main.cpp`
```bash
g++ source-code/shutdown/main.cpp -o shutdown
```
Для запуска требуется `gnome-terminal`, а команда `poweroff` может требовать прав администратора (`sudo ./shutdown`).

#### snake3d — `source-code/Python-In-Terminal/snake3d/`
Требуются FreeGLUT, GLM и OpenGL-библиотеки:
```bash
cmake -S source-code/Python-In-Terminal/snake3d -B build-snake3d
cmake --build build-snake3d -j
./build-snake3d/snake3d
```
> **Примечание:** `CMakeLists.txt` этого проекта заточен под локальный sysroot автора (`/tmp/local/sysroot`). На «чистой» системе может потребоваться поправить пути в этом файле либо собрать вручную против системных библиотек:
> ```bash
> g++ source-code/Python-In-Terminal/snake3d/src/main.cpp -o snake3d -lglut -lGLU -lGL -lm
> ```

### 5. Где окажется результат

Собранные бинарники появятся в указанной при сборке папке (`build-fantominsightui/`, `build-snake3d/` и т.д.) или в текущей директории (для `g++`). Папки сборки (`build*/`, `cmake-build-*/`) в репозиторий **не** добавляются — они игнорируются файлом `.gitignore`.

### 6. Windows

Для Windows существует отдельный порт FantominsightUI — репозиторий [fantominsight/windows-utills](https://github.com/fantominsight/windows-utills) (исходники и инструкции по сборке там же).

---

## Автор

**Ли** (fantominsight)

Связаться можно в Telegram: [@developover](https://t.me/developover)  
*(менеджер ближайший месяц занят — просьба не писать ему)*

---

## Лицензия

MIT License — делайте с программами что хотите.  
Полный текст лицензии находится в файле [LICENSE](LICENSE).
