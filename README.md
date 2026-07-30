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

## Установка / Запуск

1. Скачайте нужный бинарник из папки [`Utills/`](Utills/)
2. Сделайте его исполняемым:
   ```bash
   chmod +x FantominsightSU
   chmod +x MTKConverter
   chmod +x shutdown
   chmod +x snake3d
   ```
3. Запустите:
   ```bash
   ./имя_утилиты
   ```

**Важно:** в файлах репозитория всегда находятся **только последние версии** бинарников.  
Старые версии можно скачать в [Releases](https://github.com/fantominsight/linux-utills/releases).

---

## Сборка из исходников

### FantominsightSU
```bash
g++ source-code/CLI/allinone.cpp -o FantominsightSU
```

### MTKConverter и shutdown
```bash
g++ -o MTKConverter source-code/MTKConverter/main.cpp
g++ -o shutdown source-code/shutdown/main.cpp
```

### snake3d
Требуется CMake, FreeGLUT, GLM и OpenGL-библиотеки.  
Подробности сборки смотрите в `CMakeLists.txt` внутри папки проекта.

---

## Автор

**Ли** (fantominsight)

Связаться можно в Telegram: [@developover](https://t.me/developover)  
*(менеджер ближайший месяц занят — просьба не писать ему)*

---

## Лицензия

MIT License — делайте с программами что хотите.  
Полный текст лицензии находится в файле [LICENSE](LICENSE).
