import os
import platform
import subprocess

def main():
    # Определяем корневую директорию проекта
    project_root = os.path.abspath(os.path.dirname(__file__))
    src_dir = os.path.join(project_root, 'src')
    build_dir = os.path.join(project_root, 'build')
    include_dir = os.path.join(src_dir, 'include')
    main_file = os.path.join(src_dir, 'main.cc')

    # Проверяем, что исходный файл существует
    if not os.path.exists(main_file):
        print(f"Ошибка: Файл {main_file} не найден.")
        return

    # Определяем команду для сборки в зависимости от ОС
    system = platform.system()
    if system == 'Windows':
        compiler_command = [
            'g++', '-std=c++20', '-O3', '-flto',
            f'-I{include_dir}',
            f'-L{build_dir}',
            main_file,
            '-lsndfile', '-lmpg123-0',
            '-o', os.path.join(build_dir, 'final.exe')
        ]
    elif system == 'Linux':
        compiler_command = [
            'g++', '-std=c++20', '-O3', '-flto',
            f'-I{include_dir}',
            main_file,
            '-lsndfile', '-lmpg123',
            '-o', os.path.join(build_dir, 'final.out')
        ]
    else:
        print(f"Неизвестная операционная система: {system}")
        return

    # Создаем директорию для сборки, если её нет
    os.makedirs(build_dir, exist_ok=True)

    # Запускаем команду сборки
    try:
        print("Запуск сборки:", " ".join(compiler_command))
        subprocess.check_call(compiler_command)
        print("Сборка завершена успешно.")
    except subprocess.CalledProcessError as e:
        print(f"Ошибка сборки: {e}")

if __name__ == "__main__":
    main()
