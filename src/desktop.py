import flet as ft
from flet import ElevatedButton, FilePicker, FilePickerResultEvent, Tab, Tabs, Text, TextField, IconButton, Row, Column, Switch, AppBar, Page
import os
import platform
if platform.system() == "Windows":
	os.add_dll_directory("C:\\MinGW\\mingw64\\bin")
  
import ame  # Ваш модуль для получения ID песни

# Проверка поддерживаемых форматов
SUPPORTED_FORMATS = {".mp3", ".wav", ".flac"}

def main(page: Page):
    # Настройка темы
    def toggle_theme(e):
        page.theme_mode = "dark" if theme_switch.value else "light"
        page.update()

    theme_switch = Switch(label="Тёмная тема", on_change=toggle_theme)

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
                        on_click=lambda _: page.launch_url("https://github.com/your_project_link"),
                    ),
                ]
            ),
        ],
        spacing=20,
    )

    # Вкладка для работы с аудио
    file_path_field = TextField(label="Путь к файлу", expand=True)
    file_picker = FilePicker(on_result=lambda e: update_file_path(e, file_path_field))
    result_text = Text("", size=16, selectable=True)

    def update_file_path(e: FilePickerResultEvent, field: TextField):
        if e.files and e.files[0]:
            field.value = e.files[0].path
            page.update()

    def process_audio(e):
        file_path = file_path_field.value.strip()
        if not file_path:
            result_text.value = "Ошибка: путь к файлу не указан."
        elif not any(file_path.endswith(ext) for ext in SUPPORTED_FORMATS):
            result_text.value = f"Ошибка: неподдерживаемый формат файла.\nПоддерживаются: {', '.join(SUPPORTED_FORMATS)}"
        else:
            try:
                song_id = ame.audioID(file_path)
                result_text.value = f"ID песни: {song_id}"
            except Exception as err:
                result_text.value = f"Ошибка при обработке файла: {err}"
        page.update()

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
        AppBar(title=Text("Audio Identifier"), actions=[theme_switch]),
        Tabs(
            tabs=[
                Tab(text="Q&A", content=qa_tab),
                Tab(text="Аудио ID", content=audio_tab),
            ]
        ),
        file_picker,
    )


ft.app(target=main, view=ft.FLET_APP)
