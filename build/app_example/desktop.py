import flet as ft
from flet import ElevatedButton, FilePicker, FilePickerResultEvent, Tab, Tabs, Text, TextField, IconButton, Row, Column, Switch, AppBar, Page, Container
import os
import platform
if platform.system() == "Windows":
    os.add_dll_directory("C:\\MinGW\\mingw64\\bin")

import ame  # Модуль для получения ID песни
import threading

# Проверка поддерживаемых форматов
SUPPORTED_FORMATS = {".mp3", ".wav", ".flac"}

def main(page: Page):
    # Настройка темы
    def toggle_theme(e):
        page.theme_mode = "dark" if theme_switch.value else "light"
        page.update()

    # Создаём переключатель
    theme_switch = Switch(label="Тёмная тема", on_change=toggle_theme)

    # Оборачиваем переключатель в контейнер с отступами
    theme_switch_container = Container(
        content=theme_switch,
        padding=10  # Отступ со всех сторон
    )

    # Вкладка Q&A
    qa_tab = Column(
        [
            Text(
                "Добро пожаловать в приложение для получения уникального ID аудиофайла!\n"
                "1. Перейдите на вкладку 'Аудио ID'.\n"
                "2. Выберите файл через проводник или введите путь вручную.\n"
                "3. Поддерживаемые форматы: MP3, WAV, FLAC.\n"
                "4. Нажмите кнопку 'Получить ID', чтобы узнать уникальный ID песни.",
                size=16,
                selectable=True,
            ),
            Row(
                [
                    Text("Ссылка на GitHub проекта: "),
                    ft.TextButton(
                        "Открыть",
                        on_click=lambda _: page.launch_url("https://github.com/orion787/AME__Audio-Mapping-Encoder"),
                    ),
                ]
            ),
        ],
        spacing=20,
    )

    # Вкладка для работы с аудио
    file_path_field = TextField(label="Путь:", expand=True)
    file_picker = FilePicker(on_result=lambda e: update_file_path(e, file_path_field))
    result_text = Text("", size=16, selectable=True)

    def update_file_path(e: FilePickerResultEvent, field: TextField):
        if e.files and e.files[0]:
            field.value = e.files[0].path
            page.update()

    def process_audio(e):
        def generate_id():
            try:
                song_id = ame.audioID(file_path)  # Длительная операция
                result_text.value = f"ID песни: {song_id}"  # Результат генерации
            except Exception as err:
                result_text.value = f"Ошибка: {err}"  # Обработка исключений
            page.update()  # Финальное обновление интерфейса

        file_path = file_path_field.value.strip()
        if not file_path:
            result_text.value = "Ошибка: путь к файлу не указан."
            page.update()  # Обновляем интерфейс сразу
        elif not any(file_path.endswith(ext) for ext in SUPPORTED_FORMATS):
            result_text.value = f"Ошибка: неподдерживаемый формат файла.\nПоддерживаются: {', '.join(SUPPORTED_FORMATS)}"
            page.update()  # Обновляем интерфейс сразу
        else:
            result_text.value = "Генерация..."  # Показываем сообщение во время генерации
            page.update()  # Обновляем интерфейс сразу
            # Запускаем генерацию ID в отдельном потоке
            threading.Thread(target=generate_id, daemon=True).start()

    audio_tab = Column(
        [
            Row([file_path_field, IconButton(icon=ft.icons.FOLDER_OPEN, on_click=lambda _: file_picker.pick_files())]),
            ElevatedButton("Получить ID", on_click=process_audio),
            result_text,
        ],
        spacing=20,
    )

    # Добавление вкладок
    page.add(
        AppBar(
            title=Text("AME (Audio Mapping Encoder)"),
            actions=[theme_switch_container],  # Контейнер с переключателем темы
        ),
        Tabs(
            tabs=[
                Tab(text="Q&A", content=qa_tab),
                Tab(text="Аудио ID", content=audio_tab),
            ]
        ),
        file_picker,
    )

ft.app(target=main, view=ft.FLET_APP)
