import os
import platform
import subprocess
import sys

def get_python_paths_windows():
    """
    Пытается автоматически определить пути к include и libs Python на Windows.
    Если не удаётся, возвращает None.
    """
    try:
        # Путь к Python
        python_exe = sys.executable
        python_base = os.path.dirname(python_exe)
        include_dir = os.path.join(python_base, 'include')
        libs_dir = os.path.join(python_base, 'libs')

        if os.path.exists(include_dir) and os.path.exists(libs_dir):
            return include_dir, libs_dir
    except Exception:
        pass

    return None

def build_for_windows():
    """
    Сборка программы для Windows.
    """
    print("Сборка для Windows...")

    # Получение путей к include и libs
    python_paths = get_python_paths_windows()
    if python_paths:
        include_dir, libs_dir = python_paths
        print(f"Найдены пути Python: include={include_dir}, libs={libs_dir}")
    else:
        # Запрос путей у пользователя
        include_dir = input("Введите путь к папке include Python: ").strip()
        libs_dir = input("Введите путь к папке libs Python: ").strip()

    # Команда компиляции
    compiler_command = [
        'g++', '-O3', '-std=c++20', '-shared', '-fPIC', '-static-libgcc', '-static-libstdc++', '-Wall',
        '-lmingw32', '-mwindows',
        f'-I{include_dir}', f'-I./src/polka/include',
        f'-L{libs_dir}',
        './src/polka/bind.cpp',
        '-o', './build/polka.pyd',
        '-lpython311'
    ]

    # Выполнение команды
    try:
        print("Запуск сборки:", " ".join(compiler_command))
        subprocess.check_call(compiler_command)
        print("Сборка для Windows завершена успешно.")
    except subprocess.CalledProcessError as e:
        print(f"Ошибка сборки: {e}")
        sys.exit(1)

def build_for_linux():
    """
    Сборка программы для Linux.
    """
    print("Сборка для Linux...")

    # Команда компиляции
    compiler_command = [
        'g++', '-O3', '-Wall', '-shared', '-std=c++20', '-fPIC', '-Wall',
        '$(python3 -m pybind11 --includes)',
        './src/polka/bind.cpp',
        '-o', './build/polka$(python3-config --extension-suffix)',
    ]

    # Выполнение команды
    try:
        print("Запуск сборки:", " ".join(compiler_command))
        subprocess.check_call(" ".join(compiler_command), shell=True, executable='/bin/bash')
        print("Сборка для Linux завершена успешно.")
    except subprocess.CalledProcessError as e:
        print(f"Ошибка сборки: {e}")
        sys.exit(1)

def main():
    # Определяем ОС
    system = platform.system()

    # Создаём директорию build, если её нет
    os.makedirs('./build', exist_ok=True)

    if system == 'Windows':
        build_for_windows()
    elif system == 'Linux':
        build_for_linux()
    else:
        print(f"Сборка для ОС {system} не поддерживается.")
        sys.exit(1)

if __name__ == "__main__":
    main()
