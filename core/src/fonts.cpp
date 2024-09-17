#include "main.h"
#include "RobotoItalic.h" // Сгенерированный заголовочный файл
//const char font_path[] = "core/src/Roboto-Italic.ttf";

void LoadFonts()
{
    //ImGuiIO& io = ImGui::GetIO();

    var.io->Fonts->AddFontDefault(); // Загрузка стандартного шрифта
    //io.Fonts->AddFontDefault(); // Загрузка стандартного шрифта


    // Проверка наличия файла шрифта
    // std::ifstream font_file(font_path);
    // if (!font_file)
    // {
    //     std::cerr << "Ошибка: файл шрифта не найден по пути: " << font_path << std::endl;
    //     return;
    // }

    // Загрузка шрифта с поддержкой кириллицы
    // ImFont* font = io.Fonts->AddFontFromFileTTF(font_path, 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());

    // Загрузка шрифта с поддержкой кириллицы из встроенного массива байтов
    //ImFont* font = io.Fonts->AddFontFromMemoryTTF(Roboto_Italic_ttf, Roboto_Italic_ttf_len, 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImFont* font = var.io->Fonts->AddFontFromMemoryTTF(Roboto_Italic_ttf, Roboto_Italic_ttf_len, 16.0f, NULL, var.io->Fonts->GetGlyphRangesCyrillic());

    if (font == NULL)
    {
        std::cerr << "Ошибка загрузки шрифта!" << std::endl;
    }
    else
    {
        var.io->FontDefault = font; // Установка загруженного шрифта как шрифта по умолчанию
    }
}
