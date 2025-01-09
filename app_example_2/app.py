from flask import Flask, request, render_template, redirect, url_for, flash
import platform
import os

if platform.system() == "Windows":
    os.add_dll_directory(os.getcwd())

# Подключение модулей
from ame import audioIDAVX as audioID
from ame import audioPasswordAVX as audioPassword

from polka import fileID, filePassword  # Новый модуль polka

from werkzeug.utils import secure_filename

app = Flask(__name__)
app.secret_key = 'supersecretkey'  # Для использования flash-сообщений

# Папка для временного сохранения загруженных файлов
UPLOAD_FOLDER = "uploads"
ALLOWED_EXTENSIONS = {"wav", "mp3", "flac"}  # Разрешённые форматы файлов
app.config["UPLOAD_FOLDER"] = UPLOAD_FOLDER

# Создаём папку для загрузок, если её нет
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

def allowed_file(filename):
    return '.' in filename and filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

@app.route("/", methods=["GET", "POST"])
def index():
    result = ""
    if request.method == "POST":
        audio_file = request.files.get("audio_file")
        action = request.form.get("action")
        algorithm = request.form.get("algorithm")  # Выбранный алгоритм

        if not audio_file:
            flash("No file selected")
            return redirect(request.url)

        if not allowed_file(audio_file.filename):
            flash(f"Invalid file format. Allowed formats: {', '.join(ALLOWED_EXTENSIONS)}")
            return redirect(request.url)

        filename = secure_filename(audio_file.filename)
        file_path = os.path.join(app.config["UPLOAD_FOLDER"], filename)
        audio_file.save(file_path)

        try:
            # Логика вызова функций в зависимости от алгоритма
            if algorithm == "sha512":
                if action == "Generate ID":
                    result = f"Generated ID (SHA-512-based): {fileID(file_path)[:25]}"
                elif action == "Generate Password":
                    result = f"Generated Password (SHA-512-based): {filePassword(file_path)}"
            else:  # По умолчанию Custom
                if action == "Generate ID":
                    result = f"Generated ID (Custom): {audioID(file_path)}"
                elif action == "Generate Password":
                    result = f"Generated Password (Custom): {audioPassword(file_path)}"
        except Exception as e:
            flash(f"Invalid file format") #{str(e)}
        finally:
            if os.path.exists(file_path):
                os.remove(file_path)

    return render_template("index.html", result=result)

if __name__ == "__main__":
    app.run(debug=True)
